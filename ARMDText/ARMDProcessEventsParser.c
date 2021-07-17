#include "ARMDProcessEventsParser.h"
#include "ARMDError.h"
#include "ARMDParser.h"
#include "ARMDEventParser.h"

int ParseProcessEvents(ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDFileReaderData* armd_file_reader_data,
	BOOL* no_event_state, BYTE number_of_processes)
{
	int function_result = ERROR_OK;
	int status;
	short i;
	short number_of_events;

	GetValFromBuf(&current_proc_data->proc, armd_file_reader_data, sizeof(BYTE)); //текущий процесс УЧПУ
	GetValFromBuf(&number_of_events, armd_file_reader_data, sizeof(short)); //количесво событий
	current_proc_data->event_data = (ARMDEventData*)calloc(number_of_events, sizeof(ARMDEventData));
	if (current_proc_data->event_data)
	{
		for (i = 0; i < number_of_events; i++) //перебираем события
		{
			short armd_event;
			ARMDEventData* event_data = current_proc_data->event_data + i;
			status = ParseEventNumber(&armd_event, current_proc_data, armd_header_info, armd_file_reader_data);
			if (status >= ERROR_OK)
			{
				int parse_event_status = ParseEventData(event_data, armd_event, armd_file_reader_data, no_event_state);
				if (parse_event_status < ERROR_OK)
				{
					function_result = parse_event_status;
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

int FreeProcessesEvents(ARMDProcessData* armd_process_data)
{
	int function_status = ERROR_OK;
	if (armd_process_data->event_data)
	{
		for (short event = 0; event < armd_process_data->num_event; event++)
		{
			int free_event_value_result;
			ARMDEventData* event_data = armd_process_data->event_data + event;
			free_event_value_result = FreeEventValue(event_data->event_id, &event_data->value);
			if (free_event_value_result <= ERROR_OK)
				function_status = free_event_value_result;
		}
		free(armd_process_data->event_data);
		armd_process_data->event_data = NULL;
	}
	else
		function_status = ERROR_NULL_POINTER;
	armd_process_data->num_event = 0;
	return function_status;
}
