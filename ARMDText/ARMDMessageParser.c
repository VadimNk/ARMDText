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

BYTE CheckMessageData(const BYTE* const buffer, const DWORD start_index, const DWORD finish_index)
{
	BYTE check = 0;
	for (DWORD ch = start_index; ch < finish_index; ch++)
		check ^= buffer[ch];
	return check;
}

int ParceEvent(ARMDEventData* event_data, ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state)
{
	int function_result = ERROR_OK;
	short event_index_in_header_info;
	//определяем номер события(event_data->event), подставляя текущий процесс УЧПУ и индекс события (event_index_in_header_info) 
	//в массив, который содержит информацио о событиях (current_process_event_info) в заголовке (armd_header_info)
	GetValFromBuf(&event_index_in_header_info, armd_parser_data, sizeof(short));
	ProcInfo* current_process_header_info = armd_header_info->proc_info + current_proc_data->proc;
	SysARMDInfo* current_process_event_info = current_process_header_info->event_info;
	event_data->event = (current_process_event_info + event_index_in_header_info)->event;
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
		GetARMDLine(&event_data->value.command_line, armd_parser_data);
		break;
	case EVENT_G_FUNCTIONS:
		GetARMDLine(&event_data->value.g_functions, armd_parser_data);
		break;
	case EVENT_WNCMT: case EVENT_WNPRT: case EVENT_WPROG: case EVENT_WIZKD:
		GetARMDLine(&event_data->value.subroutine_info, armd_parser_data);
		break;
	case EVENT_TIME_SYNCH:
		event_data->value.Char = 1;
		break;
	case EVENT_ARMD_SERVICE:
		GetValFromBuf(&event_data->value.Char, armd_parser_data, sizeof(char));
		break;
	}
	return function_result;
}

int ParceEventsByProcesses(ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state, BYTE number_of_processes)
{
	int function_result = ERROR_OK;
	short j;
	short number_of_events;

		GetValFromBuf(&current_proc_data->proc, armd_parser_data, sizeof(BYTE)); //текущий процесс УЧПУ
		GetValFromBuf(&number_of_events, armd_parser_data, sizeof(short)); //количесво событий
		current_proc_data->event_data = (ARMDEventData*)calloc(number_of_events, sizeof(ARMDEventData));
		if (current_proc_data->event_data)
		{
			for (j = 0; j < number_of_events; j++) //перебираем события
			{
				ARMDEventData* event_data = current_proc_data->event_data + j;
				ParceEvent(event_data, current_proc_data, armd_header_info, armd_parser_data, no_event_state);
	
			}
			current_proc_data->num_event = j;
		}
		else
			function_result = ERROR_MEMORY_ALLOCATION_ERROR;
	if (function_result < ERROR_OK)
	{
		FreeEventData(current_proc_data);
	}
	return function_result;
}

//Функция считывает события из буфера и распределяет их по структурам
int ParseARMDMessage(ARMDMessageData* armd_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state)
{
	int i;
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
			if (number_of_processes)
			{
				armd_data->proc_data = (ARMDProcessData*)calloc(number_of_processes, sizeof(ARMDProcessData));
				if (armd_data->proc_data)
				{
					for (i = 0; i < number_of_processes; i++) //перебираем процессы
					{
						ParceEventsByProcesses(armd_data->proc_data + i, armd_header_info, armd_parser_data, no_event_state, number_of_processes);
					}
					armd_data->num_proc = (BYTE)i;

					GetValFromBuf(&armd_data->check, armd_parser_data, sizeof(BYTE));
				}
				else
					return ERROR_COMMON;
			}
		}
		else
			return ERROR_OUT_OF_RANGE;
	}
	else
		return ERROR_OUT_OF_RANGE;
	return ERROR_OK;
}

int FreeProcData(ARMDMessageData* armd_data)
{
	int function_status = ERROR_OK;
	if (armd_data->proc_data)
	{
		for (short i = 0; i < armd_data->num_proc; i++)
			FreeEventData(armd_data->proc_data + i);
		free(armd_data->proc_data);
		armd_data->proc_data = NULL;
	}
	else
		function_status = ERROR_NULL_POINTER;
	armd_data->num_proc = 0;
	return function_status;
}

int FreeEventData(ARMDProcessData* armd_process_data)
{
	int function_status = ERROR_OK;
	if (armd_process_data->event_data)
	{
		for (short event = 0; event < armd_process_data->num_event; event++)
		{
			ARMDEventData* event_data = armd_process_data->event_data + event;
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
				FreeARMDLine(event_data->value.command_line);
				break;
			case EVENT_G_FUNCTIONS:
				FreeARMDLine(event_data->value.g_functions);
				break;
			case EVENT_WNCMT:
			case EVENT_WNPRT:
			case EVENT_WPROG:
			case EVENT_WIZKD:
				FreeARMDLine(event_data->value.subroutine_info);
				break;
			case EVENT_TIME_SYNCH:
				break;
			case EVENT_ARMD_SERVICE:
				break;
			}
		}
		free(armd_process_data->event_data);
		armd_process_data->event_data = NULL;
	}
	else
		function_status = ERROR_NULL_POINTER;
	armd_process_data->num_event = 0;
	return function_status;
}

