#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include "ARMDError.h"
#include "ARMDParser.h"
#include "ARMDMessageParser.h"
#include "ARMDDisplayStrings.h"
#include "EventProgramName.h"
#include "EventSystemStart.h"
#include "EventDate.h"
#include "EventEmergencyErrorMessage.h"
#include "EventMachineIdletimeCause.h"
#include "Misc.h"

BYTE CheckMessageData(const BYTE * const buffer, const DWORD start_index, const DWORD finish_index)
{
	BYTE check = 0;
	for (DWORD ch = start_index; ch < finish_index; ch++)
		check ^= buffer[ch];
	return check;
}




int ParceEventsByProcesses(ARMDMessageData* armd_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state, BYTE number_of_processes)
{
	short armd_inf_pos;
	short i_proc;
	for (i_proc = 0; i_proc < number_of_processes; i_proc++) //перебираем процессы
	{
		short number_of_events;
		short i_event;
		GetValFromBuf(&armd_data->proc_data[i_proc].proc, armd_parser_data, sizeof(BYTE)); //текущий процесс УЧПУ
		GetValFromBuf(&number_of_events, armd_parser_data, sizeof(short)); //количесво событий
		armd_data->proc_data[i_proc].event_data = (ARMDEventData*)calloc(number_of_events, sizeof(ARMDEventData));
		for (i_event = 0; i_event < number_of_events; i_event++) //перебираем события
		{
			ARMDEventData* event_data = &armd_data->proc_data[i_proc].event_data[i_event];

			//определяем номер события(event_data->event) по индексу в массиве (armd_inf_pos)
			GetValFromBuf(&armd_inf_pos, armd_parser_data, sizeof(short)); //получаем индекс массива, который содержит информацию о событиях, в том числе номер самого события
			event_data->event = armd_header_info->proc_info[armd_data->proc_data[i_proc].proc].event_info[armd_inf_pos].event;	//получаем номер события, 
																																//подставляя текущий процесс УЧПУ и индекс 
																																//события в информацию о событиях,
																																//которую мы получили из заголовка
			switch (event_data->event)
			{
			case EVENT_NO_EVENT:
				*no_event_state = TRUE; //устанавливаем флаг события "нет события"
										//если событие окажется последним в буфере, то флаг просигнализирует о том, 
										//что надо начать считывать файл с последней позиции в файле, увеличенной на размер прочитанного буфера и уменьшенной на длину события "нет события"
				armd_parser_data->flag |= NO_EVENT_STATE;
				break;
			case EVENT_SYSTEM_START:
				EventSystemStart(&event_data->value.system_start_data, armd_parser_data);
				break;
			case  EVENT_NEW_DATE:
				EventDate(&event_data->value.time, armd_parser_data);
				break;
			case EVENT_WORK_MODE:
			case EVENT_SYSTEM_STATE:
			case EVENT_UAS:
			case EVENT_UVR:
			case EVENT_URL:
			case EVENT_COMU:
			case EVENT_CEFA:
			case EVENT_MUSP:
			case EVENT_REAZ:
			case EVENT_PART_FINISHED:
			case EVENT_RISP:
			case EVENT_CONP:
			case EVENT_SPEPN_REQ:
			case EVENT_A_SPEPN:
				GetValFromBuf(&event_data->value.Char, armd_parser_data, sizeof(char));
				break;
			case EVENT_FEED:
			case EVENT_SPINDLE_SPEED:
			case EVENT_CONTROL_PANEL_SWITCH_JOG:
			case EVENT_CONTROL_PANEL_SWITCH_FEED:
			case EVENT_CONTROL_PANEL_SWITCH_SPINDLE:
			case EVENT_SPINDLE_POWER:
				GetValFromBuf(&event_data->value.Float, armd_parser_data, sizeof(float));
				break;
			case EVENT_EMERGENCY_ERROR_MESSAGE:
				EventEmergencyErrorMessage(&event_data->value.emergency_error, armd_parser_data);
				break;
			case EVENT_PROGRAM_NAME:
				EventProgramName(&event_data->value.prog_name, armd_parser_data);
				break;
			case EVENT_BLOCK_NUMB_CTRL_PROG:
				GetValFromBuf(&event_data->value.Long, armd_parser_data, sizeof(event_data->value.Long));
				break;
			case EVENT_TOOL_NUMBER:
			case EVENT_CORRECTOR_NUMBER:
				GetValFromBuf(&event_data->value.Word, armd_parser_data, sizeof(event_data->value.Word));
				break;
			case EVENT_MACHINE_IDLETIME_CAUSE:
				EventMachineIdletimeCause(&event_data->value.machine_idletime, armd_parser_data);
				break;
			case EVENT_ALARM_PLC_ERR:
				GetARMDLine(&event_data->value.alarm_plc_error, armd_parser_data);
				break;
			case EVENT_MESS_PLC_ERR:
				GetARMDLine(&event_data->value.mess_plc_error, armd_parser_data);
				break;
			case EVENT_PROCESS_COMMAND_LINE:
			case EVENT_PROCESS_BLOCK_LINE:
			case EVENT_COMMAND_LINE:
				event_data->value.command_line = (CommandLine*)calloc(1, sizeof(CommandLine));
				GetValFromBuf(&event_data->value.command_line->len, armd_parser_data, sizeof(BYTE));
				event_data->value.command_line->str = (char*)calloc((size_t)event_data->value.command_line->len + 1, sizeof(char));
				GetValFromBuf(event_data->value.command_line->str, armd_parser_data, event_data->value.command_line->len);
				break;
			case EVENT_G_FUNCTIONS:
				event_data->value.g_functions = (GFunctions*)calloc(1, sizeof(GFunctions));
				GetValFromBuf(&event_data->value.g_functions->num, armd_parser_data, sizeof(BYTE));
				event_data->value.g_functions->g = (BYTE*)calloc(event_data->value.g_functions->num, sizeof(BYTE));
				GetValFromBuf(event_data->value.g_functions->g, armd_parser_data, event_data->value.g_functions->num * sizeof(BYTE));
				break;
			case EVENT_WNCMT: case EVENT_WNPRT: case EVENT_WPROG: case EVENT_WIZKD:
				event_data->value.subroutine_info = (SubroutineInfo*)calloc(1, sizeof(SubroutineInfo));
				GetValFromBuf(&event_data->value.subroutine_info->len, armd_parser_data, sizeof(BYTE));
				event_data->value.subroutine_info->str = (char*)calloc((size_t)event_data->value.subroutine_info->len + 1, sizeof(BYTE));
				GetValFromBuf(event_data->value.subroutine_info->str, armd_parser_data, event_data->value.subroutine_info->len);
				break;
			case EVENT_TIME_SYNCH:
				event_data->value.Char = 1;
				break;
			case EVENT_ARMD_SERVICE:
				GetValFromBuf(&event_data->value.Char, armd_parser_data, sizeof(char));
				break;
			}
		}
		armd_data->proc_data[i_proc].num_event = i_event;
	}
	armd_data->num_proc = (BYTE)i_proc;
}

//Функция считывает события из буфера и распределяет их по структурам
int ParseARMDMessage(ARMDMessageData* armd_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state)
{
	*no_event_state = FALSE;//сбрасываем флаг, даже если были события "нет события", т.к. буфер еще не закончился 

#define ARMD_TIME_LENGTH sizeof(DWORD) //sizeof(armd_data->time)
#define ARMD_EVENTS_LENGTH sizeof(WORD) //sizeof(armd_data->events_len)
	
	if (armd_parser_data->index + ARMD_TIME_LENGTH + ARMD_EVENTS_LENGTH < armd_parser_data->max_buf)
	{
		GetValFromBuf(&armd_data->time, armd_parser_data, sizeof(DWORD)); //время
		GetValFromBuf(&armd_data->events_len, armd_parser_data, sizeof(WORD)); //длина всех событий
		if ((size_t)armd_parser_data->index + armd_data->events_len < armd_parser_data->max_buf)
		{
			BYTE number_of_processes;
			GetValFromBuf(&number_of_processes, armd_parser_data, sizeof(BYTE));//количество всех процессов
			armd_data->proc_data = (ARMDProcessData*)calloc(number_of_processes, sizeof(ARMDProcessData));
			if (armd_data->proc_data)
			{
				ParceEventsByProcesses(armd_data, armd_header_info, armd_parser_data, no_event_state, number_of_processes);

				GetValFromBuf(&armd_data->check, armd_parser_data, sizeof(BYTE));
			}
			else
				return ERROR_COMMON;
		}
		else
			return ERROR_OUT_OF_RANGE;
	}
	else
		return ERROR_OUT_OF_RANGE;
	return 0;
}

int FreeEventData(ARMDMessageData* armd_data)
{
	if (armd_data == NULL)
		return ERROR_INVALID_FUNCTION_PARAMETER;
	if (armd_data->proc_data == NULL)
		return ERROR_NULL_POINTER;
	for (short proc = 0; proc < armd_data->num_proc; proc++)
	{
		for (short event = 0; event < armd_data->proc_data[proc].num_event; event++)
		{
			if (armd_data->proc_data[proc].event_data == NULL)
				continue;
			ARMDEventData* event_data = &armd_data->proc_data[proc].event_data[event];
			switch (event_data->event)
			{
			case EVENT_NO_EVENT:
				break;
			case EVENT_SYSTEM_START:
				if (event_data->value.system_start_data)
					FreeEventSystemStart(event_data->value.system_start_data);
				break;
			case  EVENT_NEW_DATE:
				if (event_data->value.time)
					FreeEventDate(event_data->value.time);
				break;
			case EVENT_WORK_MODE:
			case EVENT_SYSTEM_STATE:
			case EVENT_UAS:
			case EVENT_UVR:
			case EVENT_URL:
			case EVENT_COMU:
			case EVENT_CEFA:
			case EVENT_MUSP:
			case EVENT_REAZ:
			case EVENT_PART_FINISHED:
			case EVENT_RISP:
			case EVENT_CONP:
			case EVENT_SPEPN_REQ:
			case EVENT_A_SPEPN:
				break;
			case EVENT_FEED:
			case EVENT_SPINDLE_SPEED:
			case EVENT_CONTROL_PANEL_SWITCH_JOG:
			case EVENT_CONTROL_PANEL_SWITCH_FEED:
			case EVENT_CONTROL_PANEL_SWITCH_SPINDLE:
			case EVENT_SPINDLE_POWER:
				break;
			case EVENT_EMERGENCY_ERROR_MESSAGE:
				FreeEmergencyErrorMessage(event_data->value.emergency_error);
				break;
			case EVENT_PROGRAM_NAME:
				if (event_data->value.prog_name)
					EventProgramNameFree(event_data->value.prog_name);
				break;
			case EVENT_BLOCK_NUMB_CTRL_PROG:
				break;
			case EVENT_TOOL_NUMBER:
			case EVENT_CORRECTOR_NUMBER:
				break;
			case EVENT_MACHINE_IDLETIME_CAUSE:
				if (event_data->value.machine_idletime)
					FreeEventMachineIdletimeCause(event_data->value.machine_idletime);
				break;
			case EVENT_ALARM_PLC_ERR:
				FreeARMDLine(event_data->value.alarm_plc_error);
				break;
			case EVENT_MESS_PLC_ERR:
				FreeARMDLine(event_data->value.mess_plc_error);
				break;
			case EVENT_PROCESS_COMMAND_LINE:
			case EVENT_PROCESS_BLOCK_LINE:
			case EVENT_COMMAND_LINE:
				if (event_data->value.command_line)
				{
					if (event_data->value.command_line->str)
						free(event_data->value.command_line->str);
					free(event_data->value.command_line);
				}
				break;
			case EVENT_G_FUNCTIONS:
				free(event_data->value.g_functions->g);
				free(event_data->value.g_functions);
				break;
			case EVENT_WNCMT:
			case EVENT_WNPRT:
			case EVENT_WPROG:
			case EVENT_WIZKD:
				if (event_data->value.subroutine_info)
				{
					if (event_data->value.subroutine_info->str)
						free(event_data->value.subroutine_info->str);
					free(event_data->value.subroutine_info);
				}
				break;
			case EVENT_TIME_SYNCH:
				break;
			case EVENT_ARMD_SERVICE:
				break;
			}
		}
		free(armd_data->proc_data[proc].event_data);
	}
	free(armd_data->proc_data);
	return 0;
}

