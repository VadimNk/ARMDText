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
int ParseEventNumber(ARMDEventId* armd_event_out, ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data)
{
	int function_result = ERROR_OK;
	int result;
	ARMDEventId armd_event;
	short event_index_in_header_info;
	//определяем номер события(event_data->event), подставляя текущий процесс УЧПУ и индекс события (event_index_in_header_info) 
	//в массив, который содержит информацио о событиях (current_process_event_info) в заголовке (armd_header_info)
	result = GetValFromBuf(&event_index_in_header_info, armd_parser_data, sizeof(short));
	if (result >= ERROR_OK)
	{
		ProcInfo* current_process_header_info = armd_header_info->proc_info + current_proc_data->proc;
		SysARMDInfo* current_process_event_info = current_process_header_info->event_info;
		armd_event = (current_process_event_info + event_index_in_header_info)->event_id;
	}
	else
		function_result = ERROR_OUT_OF_RANGE;
	*armd_event_out = function_result >= ERROR_OK ? armd_event : EVENT_ERROR_EVENT;
	return function_result;
}

int ParceEventValue(ARMDEventValue* value, ARMDEventId armd_event, ARMDParserData* armd_parser_data, BOOL* no_event_state)
{
	int function_result = ERROR_OK;
	int status = ERROR_OK;
	switch (armd_event)
	{
	case EVENT_NO_EVENT:
		*no_event_state = TRUE; //устанавливаем флаг события "нет события"
								//если событие окажется последним в буфере, то флаг просигнализирует о том, 
								//что надо начать считывать файл с последней позиции в файле, увеличенной на размер прочитанного буфера и уменьшенной на длину события "нет события"
		armd_parser_data->flag |= NO_EVENT_STATE;
		break;
	case EVENT_SYSTEM_START:
		status = EventSystemStart(&value->system_start_data, armd_parser_data);
		break;
	case  EVENT_NEW_DATE:
		status = EventDate(&value->time, armd_parser_data);
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
		status = GetValFromBuf(&value->Char, armd_parser_data, sizeof(char));
		break;
	case EVENT_FEED:
	case EVENT_SPINDLE_SPEED:
	case EVENT_CONTROL_PANEL_SWITCH_JOG:
	case EVENT_CONTROL_PANEL_SWITCH_FEED:
	case EVENT_CONTROL_PANEL_SWITCH_SPINDLE:
	case EVENT_SPINDLE_POWER:
		status = GetValFromBuf(&value->Float, armd_parser_data, sizeof(float));
		break;
	case EVENT_EMERGENCY_ERROR_MESSAGE:
		status = EventEmergencyErrorMessage(&value->emergency_error, armd_parser_data);
		break;
	case EVENT_PROGRAM_NAME:
		status = EventProgramName(&value->prog_name, armd_parser_data);
		break;
	case EVENT_BLOCK_NUMB_CTRL_PROG:
		status = GetValFromBuf(&value->Long, armd_parser_data, sizeof(value->Long));
		break;
	case EVENT_TOOL_NUMBER:
	case EVENT_CORRECTOR_NUMBER:
		status = GetValFromBuf(&value->Word, armd_parser_data, sizeof(value->Word));
		break;
	case EVENT_MACHINE_IDLETIME_CAUSE:
		status = EventMachineIdletimeCause(&value->machine_idletime, armd_parser_data);
		break;
	case EVENT_ALARM_PLC_ERR:
		status = GetARMDLine(&value->alarm_plc_error, armd_parser_data);
		break;
	case EVENT_MESS_PLC_ERR:
		status = GetARMDLine(&value->mess_plc_error, armd_parser_data);
		break;
	case EVENT_PROCESS_COMMAND_LINE:
	case EVENT_PROCESS_BLOCK_LINE:
	case EVENT_COMMAND_LINE:
		status = GetARMDLine(&value->command_line, armd_parser_data);
		break;
	case EVENT_G_FUNCTIONS:
		status = GetARMDLine(&value->g_functions, armd_parser_data);
		break;
	case EVENT_WNCMT: case EVENT_WNPRT: case EVENT_WPROG: case EVENT_WIZKD:
		status = GetARMDLine(&value->subroutine_info, armd_parser_data);
		break;
	case EVENT_TIME_SYNCH:
		value->Char = 1;
		break;
	case EVENT_ARMD_SERVICE:
		status = GetValFromBuf(&value->Char, armd_parser_data, sizeof(char));
		break;
	default:
		status = ERROR_COMMON;
	}
	if (status < ERROR_OK)
	{
		FreeEventValue(armd_event, value);
		function_result = status;
	}
	return function_result;
}

int ParceEventData(ARMDEventData* event_data, ARMDEventId armd_event, ARMDParserData* armd_parser_data, BOOL* no_event_state)
{
	int status;
	status = ParceEventValue(&event_data->value, armd_event, armd_parser_data, no_event_state);
	if (status >= ERROR_OK)
		event_data->event_id = armd_event;
	return status;
}


int ParceEventsByProcesses(ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state, BYTE number_of_processes)
{
	int function_result = ERROR_OK;
	int status;
	short i;
	short number_of_events;

	GetValFromBuf(&current_proc_data->proc, armd_parser_data, sizeof(BYTE)); //текущий процесс УЧПУ
	GetValFromBuf(&number_of_events, armd_parser_data, sizeof(short)); //количесво событий
	current_proc_data->event_data = (ARMDEventData*)calloc(number_of_events, sizeof(ARMDEventData));
	if (current_proc_data->event_data)
	{
		for (i = 0; i < number_of_events; i++) //перебираем события
		{
			short armd_event;
			ARMDEventData* event_data = current_proc_data->event_data + i;
			status = ParseEventNumber(&armd_event, current_proc_data, armd_header_info, armd_parser_data);
			if (status >= ERROR_OK)
			{
				int parce_event_status = ParceEventData(event_data, armd_event, armd_parser_data, no_event_state);
				if (parce_event_status < ERROR_OK)
				{
					function_result = parce_event_status;
					break;
				}
			}
		}
		current_proc_data->num_event = i;
	}
	else
		function_result = ERROR_MEMORY_ALLOCATION_ERROR;
	if (function_result < ERROR_OK)
	{
		//FreeEventData(current_proc_data->event_data);
		FreeProcessesEvents(current_proc_data);
	}
	return function_result;
}

//Функция считывает события из буфера и распределяет их по структурам
int ParseARMDMessage(ARMDMessageData* armd_data, ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data,
	BOOL* no_event_state)
{
	int function_status = ERROR_OK;
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
						int parce_events_by_processes_status = ParceEventsByProcesses(armd_data->proc_data + i,
							armd_header_info, armd_parser_data, no_event_state, number_of_processes);
						if (parce_events_by_processes_status < ERROR_OK)
						{
							function_status = parce_events_by_processes_status;
							break;
						}

					}
					armd_data->num_proc = (BYTE)i;
					if(function_status >= ERROR_OK)
						GetValFromBuf(&armd_data->check, armd_parser_data, sizeof(BYTE));
				}
				else
					function_status = ERROR_COMMON;
			}
		}
		else
			function_status = ERROR_OUT_OF_RANGE;
	}
	else
		function_status = ERROR_OUT_OF_RANGE;
	if (function_status < ERROR_OK)
	{
		FreeProcData(armd_data);
	}
	return function_status;
}

int FreeProcData(ARMDMessageData* armd_data)
{
	int function_status = ERROR_OK;
	if (armd_data->proc_data)
	{
		for (short i = 0; i < armd_data->num_proc; i++)
			FreeEventData((armd_data->proc_data + i)->event_data);
		free(armd_data->proc_data);
		armd_data->proc_data = NULL;
	}
	else
		function_status = ERROR_NULL_POINTER;
	armd_data->num_proc = 0;
	return function_status;
}

int FreeEventValue(ARMDEventId event_id, ARMDEventValue *value)
{
	int function_status = ERROR_OK;
	switch (event_id)
	{
	case EVENT_NO_EVENT:
		break;
	case EVENT_SYSTEM_START:
		if (value->system_start_data)
			FreeEventSystemStart(value->system_start_data);
		break;
	case  EVENT_NEW_DATE:
		if (value->time)
			FreeEventDate(value->time);
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
		FreeEmergencyErrorMessage(value->emergency_error);
		break;
	case EVENT_PROGRAM_NAME:
		if (value->prog_name)
			EventProgramNameFree(value->prog_name);
		break;
	case EVENT_BLOCK_NUMB_CTRL_PROG:
		break;
	case EVENT_TOOL_NUMBER:
	case EVENT_CORRECTOR_NUMBER:
		break;
	case EVENT_MACHINE_IDLETIME_CAUSE:
		if (value->machine_idletime)
			FreeEventMachineIdletimeCause(value->machine_idletime);
		break;
	case EVENT_ALARM_PLC_ERR:
		FreeARMDLine(value->alarm_plc_error);
		break;
	case EVENT_MESS_PLC_ERR:
		FreeARMDLine(value->mess_plc_error);
		break;
	case EVENT_PROCESS_COMMAND_LINE:
	case EVENT_PROCESS_BLOCK_LINE:
	case EVENT_COMMAND_LINE:
		FreeARMDLine(value->command_line);
		break;
	case EVENT_G_FUNCTIONS:
		FreeARMDLine(value->g_functions);
		break;
	case EVENT_WNCMT:
	case EVENT_WNPRT:
	case EVENT_WPROG:
	case EVENT_WIZKD:
		FreeARMDLine(value->subroutine_info);
		break;
	case EVENT_TIME_SYNCH:
		break;
	case EVENT_ARMD_SERVICE:
		break;
	}
	return function_status;
}

int FreeEventData(ARMDEventData* event_data)
{
	int status = FreeEventValue(event_data->event_id, &event_data->value);
	return status;
}

int FreeProcessesEvents(ARMDProcessData* armd_process_data)
{
	int function_status = ERROR_OK;
	if (armd_process_data->event_data)
	{
		for (short event = 0; event < armd_process_data->num_event; event++)
		{
			ARMDEventData* event_data = armd_process_data->event_data + event;
			FreeEventData(event_data);
		}
		free(armd_process_data->event_data);
		armd_process_data->event_data = NULL;
	}
	else
		function_status = ERROR_NULL_POINTER;
	armd_process_data->num_event = 0;
	return function_status;
}

