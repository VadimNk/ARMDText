#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
//#include "ARMDFileReaderData.h"
#include "ARMDFileReader.h"
#include "ARMDError.h"
#include "ARMDHeaderParser.h"
#include "ARMDProcessedData.h"
#include "ARMDDisplayStrings.h"
#include "ARMDMessageParser.h"

int ReadARMDFile(DWORD current_file_name_max_characters, _TCHAR* current_file_name, ARMDFileReaderData* armd_file_reader_data)
{
    int ReadARMDFileResult = ERROR_COMMON;
    DWORD bytes_have_read;
    HANDLE armd_file;

    //открывать файл необходимо с флагом FILE_SHARE_READ
    //но если программное обеспечение УЧПУ использует устаревшую реализацию протокола SMB1, то доступ будет осуществляться в монопольном режиме
    //Так как УЧПУ может использовать любую реализацию протокола SMB, то время между открытием и закрытием файла должно быть минимально
    armd_file = CreateFile(current_file_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (armd_file != INVALID_HANDLE_VALUE)
    {
        DWORD new_file_len = SetFilePointer(armd_file, 0, NULL, FILE_END);//длина всего файла
        if (armd_file_reader_data->flag & NO_EVENT_STATE)
            armd_file_reader_data->parsed_file_len -= NO_EVENT_EVENT_LEN;
        DWORD file_len_to_read = new_file_len - armd_file_reader_data->parsed_file_len;//определяем количество байт, записанных в файл устройством ЧПУ с момента последнего чтения
        if (file_len_to_read > 0)
        {
            size_t realloc_max_buf = file_len_to_read;
            void* tmp = (BYTE*)realloc(armd_file_reader_data->buf, (realloc_max_buf + 1) * sizeof(BYTE));
            if (tmp)
            {
                armd_file_reader_data->buf = (BYTE*)tmp;
                armd_file_reader_data->max_buf = realloc_max_buf;
                DWORD result_offset = SetFilePointer(armd_file, armd_file_reader_data->parsed_file_len, NULL, FILE_BEGIN); //сдвигаем начальную позицию чтения из файла на уже обработанный объем данных в данном файле
                if (result_offset == armd_file_reader_data->parsed_file_len)
                {
                    BOOL read_file_result = ReadFile(armd_file, armd_file_reader_data->buf, file_len_to_read, &bytes_have_read, NULL);
                    if (read_file_result && file_len_to_read == bytes_have_read)
                        ReadARMDFileResult = ERROR_OK;
                    else
                    {
                        _tprintf(_T("%s.\n"), GetARMDString(I_FAIL_TO_READ_ARMD_FILE));
                        ReadARMDFileResult = ERROR_READING_FILE;
                    }
                }
                else
                {
                    _tprintf(_T("%s.\n"), GetARMDString(I_CANT_SET_START_FILE_POSITION));
                    ReadARMDFileResult = ERROR_MOVING_TO_POSITION_FILE;
                }
            }
            else
            {//не смогли выделить буфер
                ReadARMDFileResult = ERROR_MEMORY_ALLOCATION_ERROR;
            }
        }
        else
        {
            ReadARMDFileResult = ERROR_NOTHING_TO_PROCESS;
        }
        CloseHandle(armd_file);
        if (ReadARMDFileResult < ERROR_OK)
        {
            if (armd_file_reader_data->flag & NO_EVENT_STATE)
                armd_file_reader_data->parsed_file_len += NO_EVENT_EVENT_LEN;
        }
    }
    else
    {
        _tprintf(_T("%s:%s %s...\n"), GetARMDString(I_CANT_OPEN_ARMD_FILE), current_file_name, GetARMDString(I_RETRY));
        ReadARMDFileResult = ERROR_OPENING_FILE;
    }

    return ReadARMDFileResult;
}

void ResetFileReader(ARMDFileReaderData* armd_file_reader_data, ARMDProcessedData* armd_processed_data)
{
    armd_file_reader_data->flag &= (~NO_EVENT_STATE);
    armd_file_reader_data->index = 0;
    armd_file_reader_data->parsed_file_len = 0;
    armd_processed_data->number_items = 0;
}

