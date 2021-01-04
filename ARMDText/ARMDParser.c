#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include "ARMDParser.h"
#include "ARMDDisplayStrings.h"

int GetARMDMessage(HANDLE console_output, ARMDHeaderInfo** armd_header_info, ARMDProcessedData* armd_processed_data, ARMDParserData* armd_parser_data, BOOL* no_event_state)
{
    int parse_armd_buffer_status = ERROR_OK;
    {//считываем сообщения
        int res;
        BOOL ensure_capacity_result = EnsureCapacityOfProcessedData(armd_processed_data);
        if (ensure_capacity_result == ERROR_OK)
        {
            //считываем начальные параметры переменных, если вызов идет сразу после считывания заголовка
            if ((res = ParseARMDMessage(*armd_header_info, armd_processed_data, armd_parser_data, no_event_state)) >= 0)
            {
            }
            else
                parse_armd_buffer_status = res;
            armd_processed_data->number_items++;
        }
        else
            parse_armd_buffer_status = ensure_capacity_result;
    }
    return parse_armd_buffer_status;
}


//функция копирует value_size байт из буфера в переменную value и сдвигает указатель на текущую позцию в буффере на value_size байт
void GetValFromBuf(void* value, ARMDParserData* armd_parser_data, const DWORD value_size)
{
    memcpy(value, armd_parser_data->buf + armd_parser_data->index, value_size);
    armd_parser_data->index += value_size;
}
