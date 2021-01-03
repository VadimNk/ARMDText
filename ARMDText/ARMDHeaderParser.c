#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include "ARMDParser.h"
#include "ARMDDisplayStrings.h"


//функция считывает заголовок и начальные значения переменных
//заголовок хранит в себе конфигурацию заданную в файле конфигурации АРМД
int LoadHeader(ARMDHeaderInfo** const armd_header_info_p, ARMDParserData* armd_parser_data)
{
    int load_header_result = 0;
    ARMDHeaderInfo* armd_header_info;

    armd_header_info = (*armd_header_info_p);

    if (armd_header_info == NULL)
        armd_header_info = (ARMDHeaderInfo*)calloc(1, sizeof(ARMDHeaderInfo));

    GetValFromBuf(armd_header_info->identifier, armd_parser_data, IDENTIFIER_LEN);
    if (strcmp(armd_header_info->identifier, "BSMN") != 0) return -1;
    GetValFromBuf(armd_header_info->ver, armd_parser_data, VERSION_LEN);
    if (strcmp(armd_header_info->ver, "0000") != 0) return -1;
    GetValFromBuf(&armd_header_info->file_size, armd_parser_data, sizeof(DWORD));//точная текущая длина файла, 
                                                                                                    //После каждой записи в файл в поле file_size устанавливается новое значение длины файла (т.е. записанных байт).
                                                                                                    //Если реальный объем файла не соотвествует данному значению, то файл поврежден.

                                                                                                    //функция ParseARMDMessage не должна обрабатывать больше байт, чем записано в данной переменной
                                                                                                    //т.к. в конце теоретически может появиться грязь
    GetValFromBuf(armd_header_info->prev_file_name, armd_parser_data, MAX_ARMD_FILE_NAME);//имя предыдущего файла истории, если нет поле пустое
    armd_header_info->prev_file_name[MAX_ARMD_FILE_NAME] = '\0';
    GetValFromBuf(armd_header_info->next_file_name, armd_parser_data, MAX_ARMD_FILE_NAME);//имя следующего файла истории, если нет поле пустое
    armd_header_info->next_file_name[MAX_ARMD_FILE_NAME] = '\0';

    GetValFromBuf(&armd_header_info->withdraw, armd_parser_data, sizeof(BYTE));//цепочка файлов АРМД была разорвана
                                                                                                   //файл, стоящий перед текущим, был удален из-за нехватки места на локальном диске ЧПУ

    GetValFromBuf(&armd_header_info->software_version_len, armd_parser_data, sizeof(BYTE));
    armd_header_info->software_version = (char*)calloc((size_t)armd_header_info->software_version_len + 1, sizeof(char));
    if (armd_header_info->software_version)
    {
        GetValFromBuf(armd_header_info->software_version, armd_parser_data, armd_header_info->software_version_len * sizeof(char));//текущая версия ПО на УЧПУ

        GetValFromBuf(armd_header_info->machine_name, armd_parser_data, MAX_ARMD_FILE_NAME);//  "имя" станка
        GetValFromBuf(&armd_header_info->system_tick, armd_parser_data, sizeof(short));//системный тик (AXCFIL;инструкция TIM,мс)
        GetValFromBuf(&armd_header_info->record_time, armd_parser_data, sizeof(long));//интервал записи данных АРМД с УЧПУ в файл АРМД
        GetValFromBuf(&armd_header_info->SysTime.wYear, armd_parser_data, sizeof(WORD));//год создания файла
        GetValFromBuf(&armd_header_info->SysTime.wMonth, armd_parser_data, sizeof(WORD));//месяц
        GetValFromBuf(&armd_header_info->SysTime.wDay, armd_parser_data, sizeof(WORD));//день

        GetValFromBuf(&armd_header_info->local, armd_parser_data, sizeof(BYTE));

        GetValFromBuf(&armd_header_info->reserved, armd_parser_data, HEADER_RESERVED_BYTES);

        GetValFromBuf(&armd_header_info->num_proc, armd_parser_data, sizeof(BYTE));//количество всех процессов, заданных в файле конфигурации АРМД 
        //может не совпадать с заданным в файле конфигурации АРМД
        armd_header_info->proc_info = (ProcInfo*)calloc(armd_header_info->num_proc, sizeof(ProcInfo));
        if (armd_header_info->proc_info)
        {
            for (short i = 0; i < armd_header_info->num_proc; i++)
            {
                GetValFromBuf(&armd_header_info->proc_info[i].num_events, armd_parser_data, sizeof(short));//общее количество событий в процессе
                armd_header_info->proc_info[i].event_info = (SysARMDInfo*)calloc(armd_header_info->proc_info[i].num_events, sizeof(SysARMDInfo));
                if (armd_header_info->proc_info[i].event_info)
                {
                    for (short j = 0; j < armd_header_info->proc_info[i].num_events; j++)
                    {
                        //параметры мониторинга
                        GetValFromBuf(&armd_header_info->proc_info[i].event_info[j].event, armd_parser_data, sizeof(short));
                        GetValFromBuf(&armd_header_info->proc_info[i].event_info[j].account_type, armd_parser_data, sizeof(int));
                        GetValFromBuf(&armd_header_info->proc_info[i].event_info[j].account_param, armd_parser_data, sizeof(float));
                        GetValFromBuf(&armd_header_info->proc_info[i].event_info[j].units, armd_parser_data, sizeof(BYTE));
                    }
                }
                else
                {
                    load_header_result = ERROR_OUT_OF_MEMORY;
                    break;
                }
            }
        }
        else
            load_header_result = ERROR_OUT_OF_MEMORY;
    }
    else
        load_header_result = ERROR_OUT_OF_MEMORY;
    if (load_header_result < 0)
    {
        FreeHeader(&armd_header_info);
    }
    (*armd_header_info_p) = armd_header_info;
    return load_header_result;
}




void FreeHeader(ARMDHeaderInfo** armd_header_info)
{
    if (armd_header_info && *armd_header_info)
    {
        if ((*armd_header_info)->proc_info)
        {
            for (short i = 0; i < (*armd_header_info)->num_proc; i++)
            {
                if ((*armd_header_info)->proc_info[i].event_info != NULL)
                    free((*armd_header_info)->proc_info[i].event_info);
            }
            free((*armd_header_info)->proc_info);
        }
        if ((*armd_header_info)->software_version)
            free((*armd_header_info)->software_version);
        free(*armd_header_info);
        *armd_header_info = NULL;
    }
}
