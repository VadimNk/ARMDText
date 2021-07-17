#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include "ARMDError.h"
#include "ARMDParser.h"
#include "ARMDMessageParser.h"
#include "ARMDDisplayStrings.h"
#include "ARMDEventParser.h"
#include "ARMDProcessEventsParser.h"
#include "Misc.h"

BYTE CheckMessageData(const BYTE* const buffer, const DWORD start_index, const DWORD finish_index)
{
	BYTE check = 0;
	for (DWORD ch = start_index; ch < finish_index; ch++)
		check ^= *(buffer + ch);
	return check;
}


//Функция считывает события из буфера и распределяет их по структурам
int ParseARMDMessage(ARMDMessageData* armd_data, ARMDHeaderInfo* armd_header_info, ARMDFileReaderData* armd_file_reader_data,
	BOOL* no_event_state)
{
	int function_status = ERROR_OK;
	int i;
	*no_event_state = FALSE;//сбрасываем флаг, даже если были события "нет события", т.к. буфер еще не закончился 

#define ARMD_TIME_LENGTH sizeof(DWORD) //sizeof(armd_data->time)
#define ARMD_EVENTS_LENGTH sizeof(WORD) //sizeof(armd_data->events_len)

	if (armd_file_reader_data->index + ARMD_TIME_LENGTH + ARMD_EVENTS_LENGTH < armd_file_reader_data->max_buf)
	{
		GetValFromBuf(&armd_data->time, armd_file_reader_data, sizeof(DWORD)); //время
		GetValFromBuf(&armd_data->events_len, armd_file_reader_data, sizeof(WORD)); //длина всех событий
		if ((size_t)armd_file_reader_data->index + armd_data->events_len < armd_file_reader_data->max_buf)
		{
			BYTE number_of_processes;
			GetValFromBuf(&number_of_processes, armd_file_reader_data, sizeof(BYTE));//количество всех процессов
			if (number_of_processes)
			{
				armd_data->proc_data = (ARMDProcessData*)calloc(number_of_processes, sizeof(ARMDProcessData));
				if (armd_data->proc_data)
				{
					for (i = 0; i < number_of_processes; i++) //перебираем процессы
					{
						int parse_events_by_processes_status = ParseProcessEvents(armd_data->proc_data + i,
							armd_header_info, armd_file_reader_data, no_event_state, number_of_processes);
						if (parse_events_by_processes_status < ERROR_OK)
						{
							function_status = parse_events_by_processes_status;
							break;
						}

					}
					armd_data->num_proc = (BYTE)i;
					if(function_status >= ERROR_OK)
						GetValFromBuf(&armd_data->check, armd_file_reader_data, sizeof(BYTE));
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
		FreeARMDMessage(armd_data);
	}
	return function_status;
}

int FreeARMDMessage(ARMDMessageData* armd_data)
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