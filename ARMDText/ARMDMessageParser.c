#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include "ARMDError.h"
#include "ARMDParser.h"
#include "ARMDMessageParser.h"
#include "ARMDDisplayStrings.h"

//Функция считывает события из буфера и распределяет их по структурам
int ParseARMDMessage(ARMDHeaderInfo* armd_header_info, ARMDProcessedData* armd_processed, ARMDParserData* armd_parser_data, BOOL* no_event_state)
{
    if (no_event_state != NULL)
        *no_event_state = FALSE;//сбрасываем флаг, даже если были события "нет события", т.к. буфер еще не закончился 
    else
        return ERROR_INVALID_FUNCTION_PARAMETER;
    if (armd_header_info == NULL)
        return ERROR_INVALID_FUNCTION_PARAMETER;
    if (armd_parser_data == NULL)
        return ERROR_INVALID_FUNCTION_PARAMETER;
    if (armd_processed == NULL)
        return ERROR_INVALID_FUNCTION_PARAMETER;

    short armd_inf_pos;
    ARMDMessageData* armd_data = armd_processed->data[armd_processed->number_items];
    DWORD index = armd_parser_data->index;

#define ARMD_TIME_LENGTH sizeof(DWORD) //sizeof(armd_data->time)
#define ARMD_EVENTS_LENGTH sizeof(WORD) //sizeof(armd_data->events_len)
    if (armd_parser_data->index + ARMD_TIME_LENGTH + ARMD_EVENTS_LENGTH >= armd_parser_data->max_buf)
        return ERROR_OUT_OF_RANGE;

    GetValFromBuf(&armd_data->time, armd_parser_data, sizeof(DWORD)); //время
    GetValFromBuf(&armd_data->events_len, armd_parser_data, sizeof(WORD)); //длина всех событий
    if (armd_parser_data->index + armd_data->events_len >= armd_parser_data->max_buf)
        return ERROR_OUT_OF_RANGE;
    
    BYTE number_of_processes;
    GetValFromBuf(&number_of_processes, armd_parser_data, sizeof(BYTE));//количество всех процессов
    armd_data->proc_data = (ARMDProcessData*)calloc(number_of_processes, sizeof(ARMDProcessData));
    short i_proc;
    for (i_proc = 0; i_proc < number_of_processes; i_proc++) //перебираем процессы
    {
        short number_of_events;
        short i_event;

        GetValFromBuf(&armd_data->proc_data[i_proc].proc, armd_parser_data, sizeof(BYTE)); //текущий процесс
        GetValFromBuf(&number_of_events, armd_parser_data, sizeof(short)); //количесво событий
        armd_data->proc_data[i_proc].event_data = (ARMDEventData*)calloc(number_of_events, sizeof(ARMDEventData));
        for (i_event = 0; i_event < number_of_events; i_event++) //перебираем события
        {
            GetValFromBuf(&armd_inf_pos, armd_parser_data, sizeof(short)); //получаем индекс события в заголовке
            ARMDEventData* event_data = &armd_data->proc_data[i_proc].event_data[i_event];
            event_data->event = armd_header_info->proc_info[armd_data->proc_data[i_proc].proc].event_info[armd_inf_pos].event;//получаем номер события, подставляя текущий процесс и индекс события
                                                                                                                         // в информацию о событиях, которую мы получили из заголовка
            switch (event_data->event)
            {
            case EVENT_NO_EVENT:
                *no_event_state = TRUE; //устанавливаем флаг события "нет события"
                                        //если событие окажется последним в буфере, то флаг просигнализирует о том, 
                                        //что надо начать считывать файл с последней позиции в файле, увеличенной на размер прочитанного буфера и уменьшенной на длину события "нет события"
                armd_parser_data->flag |= NO_EVENT_STATE;
                break;
            case EVENT_SYSTEM_START:
                event_data->value.system_start_data = (SystemStartData*)calloc(1, sizeof(SystemStartData));
                GetValFromBuf(&event_data->value.system_start_data->Year, armd_parser_data, sizeof(WORD));
                GetValFromBuf(&event_data->value.system_start_data->Month, armd_parser_data, sizeof(WORD));
                GetValFromBuf(&event_data->value.system_start_data->Day, armd_parser_data, sizeof(WORD));
                GetValFromBuf(&event_data->value.system_start_data->time, armd_parser_data, sizeof(DWORD));
                //-----------------------------информация о файлах характеризации-----------------------------------------------------------//
                GetValFromBuf(&event_data->value.system_start_data->character_files_num, armd_parser_data, sizeof(BYTE));
                event_data->value.system_start_data->file_data =
                    (CharacterizationFileData*)calloc(event_data->value.system_start_data->character_files_num, sizeof(CharacterizationFileData));
                for (int i = 0; i < event_data->value.system_start_data->character_files_num; i++)
                {
                    BYTE len;
                    GetValFromBuf(&len, armd_parser_data, sizeof(BYTE));
                    event_data->value.system_start_data->file_data[i].logical_name = (char*)calloc((size_t)len + 1, sizeof(char));
                    GetValFromBuf(event_data->value.system_start_data->file_data[i].logical_name, armd_parser_data, len);

                    GetValFromBuf(&len, armd_parser_data, sizeof(BYTE));
                    event_data->value.system_start_data->file_data[i].physical_name = (char*)calloc((size_t)len + 1, sizeof(char));
                    GetValFromBuf(event_data->value.system_start_data->file_data[i].physical_name, armd_parser_data, len);

                    GetValFromBuf(&len, armd_parser_data, sizeof(BYTE));
                    event_data->value.system_start_data->file_data[i].destination = (char*)calloc((size_t)len + 1, sizeof(char));
                    GetValFromBuf(event_data->value.system_start_data->file_data[i].destination, armd_parser_data, len);
                }
                //--------------------------------------------------------------------------------------------------------------------------//
                break;
            case  EVENT_NEW_DATE:
                event_data->value.time = (WORD*)calloc(3, sizeof(WORD));
                GetValFromBuf(event_data->value.time, armd_parser_data, 3 * sizeof(WORD));
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
            case EVENT_MES_ERR_PROG:
            {
                event_data->value.emergency_error = (EmergData*)calloc(1, sizeof(EmergData));
                GetValFromBuf(&event_data->value.emergency_error->error_code, armd_parser_data, sizeof(char));
                GetValFromBuf(&event_data->value.emergency_error->msg_len, armd_parser_data, sizeof(event_data->value.emergency_error->msg_len));
                if (event_data->value.emergency_error->msg_len > 0)
                {
                    event_data->value.emergency_error->msg = (char*)calloc((size_t)event_data->value.emergency_error->msg_len + 1, sizeof(char));
                    GetValFromBuf(event_data->value.emergency_error->msg, armd_parser_data, event_data->value.emergency_error->msg_len);
                }
            }
            break;
            case EVENT_PROGRAM_NAME:
            {
                BYTE str_len = 0;
                event_data->value.progname = (ProgName*)calloc(1, sizeof(ProgName));
                GetValFromBuf(&event_data->value.progname->num, armd_parser_data, sizeof(BYTE));
                event_data->value.progname->data = (ProgNameData*)calloc(event_data->value.progname->num, sizeof(ProgNameData));
                for (int i = 0; i < event_data->value.progname->num; i++)
                {
                    GetValFromBuf(&event_data->value.progname->data[i].layer, armd_parser_data, sizeof(BYTE));
                    GetValFromBuf(&str_len, armd_parser_data, sizeof(str_len));
                    event_data->value.progname->data[i].name = (char*)malloc(((size_t)str_len + 1) * sizeof(char));
                    GetValFromBuf(event_data->value.progname->data[i].name, armd_parser_data, str_len);
                    event_data->value.progname->data[i].name[str_len] = '\0';
                    GetValFromBuf(&str_len, armd_parser_data, sizeof(str_len));
                    event_data->value.progname->data[i].path = (char*)malloc(((size_t)str_len + 1) * sizeof(char));
                    GetValFromBuf(event_data->value.progname->data[i].path, armd_parser_data, str_len);
                    event_data->value.progname->data[i].path[str_len] = '\0';
                }
            }
            break;
            case EVENT_BLOCK_NUMB_CTRL_PROG:
                GetValFromBuf(&event_data->value.Long, armd_parser_data, sizeof(event_data->value.Long));
                break;
            case EVENT_TOOL_NUMBER:
            case EVENT_CORRECTOR_NUMBER:
                GetValFromBuf(&event_data->value.Word, armd_parser_data, sizeof(event_data->value.Word));
                break;
            case EVENT_MACHINE_IDLETIME_CAUSE:
                event_data->value.machine_idletime = (MachineIdleTime*)calloc(1, sizeof(MachineIdleTime));
                GetValFromBuf(&event_data->value.machine_idletime->num, armd_parser_data, sizeof(BYTE));
                event_data->value.machine_idletime->idle = (Idle*)calloc(event_data->value.machine_idletime->num, sizeof(Idle));

                for (int ai = 0; ai < event_data->value.machine_idletime->num; ai++)
                {
                    GetValFromBuf(&event_data->value.machine_idletime->idle[ai].action, armd_parser_data, sizeof(char));
                    GetValFromBuf(&event_data->value.machine_idletime->idle[ai].group_len, armd_parser_data, sizeof(BYTE));
                    if (event_data->value.machine_idletime->idle[ai].group_len > 0)
                    {
                        event_data->value.machine_idletime->idle[ai].group = (char*)calloc((size_t)event_data->value.machine_idletime->idle[ai].group_len + 1, sizeof(char));
                        GetValFromBuf(event_data->value.machine_idletime->idle[ai].group, armd_parser_data, event_data->value.machine_idletime->idle[ai].group_len * sizeof(char));
                    }
                    GetValFromBuf(&event_data->value.machine_idletime->idle[ai].len, armd_parser_data, sizeof(BYTE));
                    if (event_data->value.machine_idletime->idle[ai].len > 0)
                    {
                        event_data->value.machine_idletime->idle[ai].str = (char*)calloc((size_t)event_data->value.machine_idletime->idle[ai].len + 1, sizeof(char));
                        GetValFromBuf(event_data->value.machine_idletime->idle[ai].str, armd_parser_data, event_data->value.machine_idletime->idle[ai].len * sizeof(char));
                    }
                }
                break;
            case EVENT_ALARM_PLC_ERR:
                event_data->value.alarm_plc_error = (PlcError*)calloc(1, sizeof(PlcError));
                GetValFromBuf(&event_data->value.alarm_plc_error->log_len, armd_parser_data, sizeof(BYTE));
                event_data->value.alarm_plc_error->log = (char*)calloc((size_t)event_data->value.alarm_plc_error->log_len + 1, sizeof(char));
                GetValFromBuf(event_data->value.alarm_plc_error->log, armd_parser_data, event_data->value.alarm_plc_error->log_len);
                break;
            case EVENT_MESS_PLC_ERR:
                event_data->value.mess_plc_error = (PlcError*)calloc(1, sizeof(PlcError));
                GetValFromBuf(&event_data->value.mess_plc_error->log_len, armd_parser_data, sizeof(BYTE));
                event_data->value.mess_plc_error->log = (char*)calloc((size_t)event_data->value.mess_plc_error->log_len + 1, sizeof(char));
                GetValFromBuf(event_data->value.mess_plc_error->log, armd_parser_data, event_data->value.mess_plc_error->log_len);

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

    BYTE check = 0;
    for (DWORD ch = index; ch < armd_parser_data->index; ch++)
    {
        check ^= armd_parser_data->buf[ch];
    }
    GetValFromBuf(&armd_data->check, armd_parser_data, sizeof(BYTE));
    if (armd_data->check != check)
    {
        _tprintf(_T("%s. %s."), GetARMDString(I_DATA_CHECK_ERROR), GetARMDString(I_DATA_IS_CORRUPTED));
        return -1;
    }
    return 0;
}

int FreeEventData(ARMDMessageData* armd_data)
{
    if (armd_data == NULL)
        return ERROR_INVALID_FUNCTION_PARAMETER;
    if (armd_data->proc_data == NULL)
        return ERROR_NULL_POINTER;
    for (BYTE proc = 0; proc < armd_data->num_proc; proc++)
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
                {
                    if (event_data->value.system_start_data->file_data)
                    {
                        for (int i = 0; i < event_data->value.system_start_data->character_files_num; i++)
                        {
                            free(event_data->value.system_start_data->file_data[i].logical_name);
                            free(event_data->value.system_start_data->file_data[i].physical_name);
                            free(event_data->value.system_start_data->file_data[i].destination);
                        }
                        free(event_data->value.system_start_data->file_data);
                    }
                    free(event_data->value.system_start_data);
                }
                break;
            case  EVENT_NEW_DATE:
                if (event_data->value.time)
                    free(event_data->value.time);
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
            case EVENT_MES_ERR_PROG:
                if (event_data->value.emergency_error)
                {
                    if (event_data->value.emergency_error->msg != NULL)
                        free(event_data->value.emergency_error->msg);
                    free(event_data->value.emergency_error);
                }
                break;
            case EVENT_PROGRAM_NAME:
                if (event_data->value.progname)
                {
                    if (event_data->value.progname->data)
                    {
                        for (short s_i = 0; s_i < event_data->value.progname->num; s_i++)
                        {
                            if (event_data->value.progname->data[s_i].name)
                                free(event_data->value.progname->data[s_i].name);
                            if (event_data->value.progname->data[s_i].path)
                                free(event_data->value.progname->data[s_i].path);
                        }
                        free(event_data->value.progname->data);
                    }
                    free(event_data->value.progname);
                }
                break;
            case EVENT_BLOCK_NUMB_CTRL_PROG:
                break;
            case EVENT_TOOL_NUMBER:
            case EVENT_CORRECTOR_NUMBER:
                break;
            case EVENT_MACHINE_IDLETIME_CAUSE:
                if (event_data->value.machine_idletime)
                {
                    for (short s_i = 0; s_i < event_data->value.machine_idletime->num; s_i++)
                    {
                        if (event_data->value.machine_idletime->idle[s_i].group_len > 0)
                            free(event_data->value.machine_idletime->idle[s_i].group);
                        if (event_data->value.machine_idletime->idle[s_i].len > 0)
                            free(event_data->value.machine_idletime->idle[s_i].str);
                    }
                    if (event_data->value.machine_idletime->idle)
                        free(event_data->value.machine_idletime->idle);
                    free(event_data->value.machine_idletime);
                }
                break;
            case EVENT_ALARM_PLC_ERR:
                free(event_data->value.alarm_plc_error->log);
                free(event_data->value.alarm_plc_error);
                break;
            case EVENT_MESS_PLC_ERR:
                free(event_data->value.mess_plc_error->log);
                free(event_data->value.mess_plc_error);

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

