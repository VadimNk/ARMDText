#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include "ARMDParser.h"
#include "ARMDDisplayStrings.h"

int GetARMDMessage(HANDLE console_output, ARMDHeaderInfo** armd_header_info, ARMDProcessedData* armd_processed_data, ARMDFileReaderData* armd_file_reader_data, BOOL* no_event_state)
{
    int parse_armd_buffer_status = ERROR_OK;
    {//считываем сообщения
        int res;
        BOOL ensure_capacity_result = EnsureCapacityOfProcessedData(armd_processed_data);
        if (ensure_capacity_result == ERROR_OK)
        {
            DWORD index = armd_file_reader_data->index;
            ARMDMessageData* armd_data = *(armd_processed_data->data + armd_processed_data->number_items);
            //считываем начальные параметры переменных, если вызов идет сразу после считывания заголовка
            if ((res = ParseARMDMessage(armd_data, *armd_header_info,armd_file_reader_data, no_event_state)) >= 0)
            {
                if (armd_data->check == CheckMessageData(armd_file_reader_data->buf, index, armd_file_reader_data->index - 1))
                    armd_processed_data->number_items++;
                else
                {
                    _tprintf(_T("%s. %s."), GetARMDString(I_DATA_CHECK_ERROR), GetARMDString(I_DATA_IS_CORRUPTED));
                    parse_armd_buffer_status = ERROR_WRONG_CHECK;
                }
                if (parse_armd_buffer_status < ERROR_OK)
                    FreeARMDMessage(armd_data);
            }
            else
                parse_armd_buffer_status = res;
        }
        else
            parse_armd_buffer_status = ensure_capacity_result;
    }
    return parse_armd_buffer_status;
}


//функция копирует value_size байт из буфера в переменную value и сдвигает указатель на текущую позцию в буффере на value_size байт
int GetValFromBuf(void* value, ARMDFileReaderData* armd_file_reader_data, const DWORD value_size)
{
    if ((size_t)armd_file_reader_data->index + value_size <= armd_file_reader_data->max_buf)
    {
        memcpy(value, armd_file_reader_data->buf + armd_file_reader_data->index, value_size);
        armd_file_reader_data->index += value_size;
        return ERROR_OK;
    }
    else
        return ERROR_OUT_OF_RANGE;
}
