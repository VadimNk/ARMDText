#include <windows.h>
#include "ARMDEventParser.h"
#include "ARMDParser.h"
#include "EventProgramName.h"
#include "EventSystemStart.h"
#include "EventDate.h"
#include "EventEmergencyErrorMessage.h"
#include "EventMachineIdletimeCause.h"

int ParseEventNumber(ARMDEventId* armd_event_out, ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDFileReaderData* armd_file_reader_data)
{
	int function_result = ERROR_OK;
	int result;
	ARMDEventId armd_event;
	short event_index_in_header_info;
	//определяем номер события(event_data->event), подставляя текущий процесс УЧПУ и индекс события (event_index_in_header_info) 
	//в массив, который содержит информацио о событиях (current_process_event_info) в заголовке (armd_header_info)
	result = GetValFromBuf(&event_index_in_header_info, armd_file_reader_data, sizeof(short));
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

int ParseEventValue(ARMDEventValue* value, ARMDEventId armd_event, ARMDFileReaderData* armd_file_reader_data, BOOL* no_event_state)
{
	int function_result = ERROR_OK;
	int status = ERROR_OK;
	switch (armd_event)
	{
	case EVENT_NO_EVENT:
		*no_event_state = TRUE; //устанавливаем флаг события "нет события"
								//если событие окажется последним в буфере, то флаг просигнализирует о том, 
								//что надо начать считывать файл с последней позиции в файле, увеличенной на размер прочитанного буфера и уменьшенной на длину события "нет события"
		armd_file_reader_data->flag |= NO_EVENT_STATE;
		break;
	case EVENT_SYSTEM_START:
		status = EventSystemStart(&value->system_start_data, armd_file_reader_data);
		break;
	case  EVENT_NEW_DATE:
		status = EventDate(&value->time, armd_file_reader_data);
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
		status = GetValFromBuf(&value->Char, armd_file_reader_data, sizeof(char));
		break;
	case EVENT_FEED:
	case EVENT_SPINDLE_SPEED:
	case EVENT_CONTROL_PANEL_SWITCH_JOG:
	case EVENT_CONTROL_PANEL_SWITCH_FEED:
	case EVENT_CONTROL_PANEL_SWITCH_SPINDLE:
	case EVENT_SPINDLE_POWER:
		status = GetValFromBuf(&value->Float, armd_file_reader_data, sizeof(float));
		break;
	case EVENT_EMERGENCY_ERROR_MESSAGE:
		status = EventEmergencyErrorMessage(&value->emergency_error, armd_file_reader_data);
		break;
	case EVENT_PROGRAM_NAME:
		status = EventProgramName(&value->prog_name, armd_file_reader_data);
		break;
	case EVENT_BLOCK_NUMB_CTRL_PROG:
		status = GetValFromBuf(&value->Long, armd_file_reader_data, sizeof(value->Long));
		break;
	case EVENT_TOOL_NUMBER:
	case EVENT_CORRECTOR_NUMBER:
		status = GetValFromBuf(&value->Word, armd_file_reader_data, sizeof(value->Word));
		break;
	case EVENT_MACHINE_IDLETIME_CAUSE:
		status = EventMachineIdletimeCause(&value->machine_idletime, armd_file_reader_data);
		break;
	case EVENT_ALARM_PLC_ERR:
		status = GetARMDLine(&value->alarm_plc_error, armd_file_reader_data);
		break;
	case EVENT_MESS_PLC_ERR:
		status = GetARMDLine(&value->mess_plc_error, armd_file_reader_data);
		break;
	case EVENT_PROCESS_COMMAND_LINE:
	case EVENT_PROCESS_BLOCK_LINE:
	case EVENT_COMMAND_LINE:
		status = GetARMDLine(&value->command_line, armd_file_reader_data);
		break;
	case EVENT_G_FUNCTIONS:
		status = GetARMDLine(&value->g_functions, armd_file_reader_data);
		break;
	case EVENT_WNCMT: case EVENT_WNPRT: case EVENT_WPROG: case EVENT_WIZKD:
		status = GetARMDLine(&value->subroutine_info, armd_file_reader_data);
		break;
	case EVENT_TIME_SYNCH:
		value->Char = 1;
		break;
	case EVENT_ARMD_SERVICE:
		status = GetValFromBuf(&value->Char, armd_file_reader_data, sizeof(char));
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

int ParseEventData(ARMDEventData* event_data, ARMDEventId armd_event, ARMDFileReaderData* armd_file_reader_data, BOOL* no_event_state)
{
	int status;
	status = ParseEventValue(&event_data->value, armd_event, armd_file_reader_data, no_event_state);
	if (status >= ERROR_OK)
		event_data->event_id = armd_event;
	return status;
}

int FreeEventValue(ARMDEventId event_id, ARMDEventValue* value)
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

int FreeEventData(ARMDProcessData* process_data)
{
	int status = ERROR_OK;
	int free_event_value_status;
	for (short i_event = 0; i_event < process_data->num_event; i_event++)
	{
		free_event_value_status = FreeEventValue((process_data->event_data + i_event)->event_id, &(process_data->event_data + i_event)->value);
		if (free_event_value_status < ERROR_OK)
		{
			status = free_event_value_status;
			break;
		}
	}
	free(process_data->event_data);
	return status;
}
