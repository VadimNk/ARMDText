#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "ARMDDisplay.h"
#include "ARMDParser.h"
#include "ARMDDisplayStrings.h"

#define MAX_WORK_MODE_ST 8
TCHAR WORK_MODE_ST[MAX_WORK_MODE_ST][6] = { _T("MDI"), _T("AUTO"), _T("STEP"), _T("MANU"), _T("MANJ"), _T("PROF"),_T("HOME"),_T("RESET") };
TCHAR SYSTEM_STATE_ST[10][5] = { { _T("    " }), { _T("IDLE") },{ _T("RUN ") },{ _T("HOLD") },{ _T("RUNH") },{ _T("RESE") },{ _T("ERRO") },{ _T("WAIT") },{ _T("INP ") },{ _T("RTCE") } };

static void ConvertTime(WORD* hours, WORD* minutes, WORD* seconds, WORD* milliseconds, DWORD time)
{
    //	if (Microseconds!= NULL) (*Microseconds)=(time%10)*100;
    time /= 10;
    if (milliseconds != NULL) (*milliseconds) = time % 1000;
    time /= 1000;
    if (seconds != NULL)	(*seconds) = time % 60;
    time /= 60;
    if (minutes != NULL)(*minutes) = time % 60;
    time /= 60;
    if (hours != NULL)	(*hours) = time % 60;
}

void PrintARMDVariableState(TCHAR* variable, char value)
{
    _tprintf(_T("%s %s| "), variable, value ? GetARMDString(I_ENABLE) : GetARMDString(I_DISABLE));
}

// Функция вывода на экран считанных структур
int ShowVal(HANDLE console_output, ARMDMessageData* armd_data)
{
#define MAX_TMP 512
    TCHAR tmp[MAX_TMP];
    WORD hours, minutes, seconds, milliseconds;
    SetConsoleTextAttribute(console_output, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    ConvertTime(&hours, &minutes, &seconds, &milliseconds, armd_data->time);
    SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
    _tprintf(_T("%.2u:%.2u:%.2u.%.4u "), hours, minutes, seconds, milliseconds);

    for (short i_proc = 0; i_proc < armd_data->num_proc; i_proc++)
    {
        switch (armd_data->proc_data[i_proc].proc)
        {
        case 0:	SetConsoleTextAttribute(console_output, FOREGROUND_GREEN | FOREGROUND_INTENSITY);									break;
        case 1:	SetConsoleTextAttribute(console_output, FOREGROUND_RED | FOREGROUND_INTENSITY);										break;
        case 2:	SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);					break;
        case 3:	SetConsoleTextAttribute(console_output, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);					break;
        case 4:	SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);					break;
        case 5:	SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
        }
        if (armd_data->proc_data[i_proc].proc != 0)
            _tprintf(_T("%s:%u "), GetARMDString(I_PROCESS), armd_data->proc_data[i_proc].proc);
        for (short event = 0; event < armd_data->proc_data[i_proc].num_event; event++)
        {
            ARMDEventData* event_data = &armd_data->proc_data[i_proc].event_data[event];
            switch (event_data->event)
            {
            case EVENT_NO_EVENT:
                _tprintf(GetARMDString(I_NO_EVENT));
                break;
            case EVENT_SYSTEM_START:
                _tprintf(_T("%s: %u-%u-%u "),
                    GetARMDString(I_SYSTEM_START),
                    event_data->value.system_start_data->Year,
                    event_data->value.system_start_data->Month,
                    event_data->value.system_start_data->Day);

                ConvertTime(&hours, &minutes, &seconds, &milliseconds, armd_data->time);
                _tprintf(_T("%.2u:%.2u:%.2u.%.4u \n"), hours, minutes, seconds, milliseconds);

                _tprintf(_T("%s: "), GetARMDString(I_CHARACTERIZATION_FILES));
                for (int i = 0; i < event_data->value.system_start_data->character_files_num; i++)
                {
                    _tprintf(_T("%s "), ByteToWide(MAX_TMP, tmp, event_data->value.system_start_data->file_data[i].logical_name));
                    _tprintf(_T("%s"), ByteToWide(MAX_TMP, tmp, event_data->value.system_start_data->file_data[i].physical_name));
                    _tprintf(_T("%s, "), ByteToWide(MAX_TMP, tmp, event_data->value.system_start_data->file_data[i].destination));
                }
                break;
            case  EVENT_NEW_DATE:
                _tprintf(_T("%s: %u-%u-%u| "), GetARMDString(I_NEW_DATE), event_data->value.time[0],
                    event_data->value.time[1], event_data->value.time[2]);
                break;
            case EVENT_WORK_MODE:
            {
                char mode = event_data->value.Char - 1;
                if (mode >= 0 && mode < MAX_WORK_MODE_ST)
                    _tprintf(_T("%s:%s| "), GetARMDString(I_WORK_MODE), WORK_MODE_ST[mode]);
                else
                    if (mode == -1)
                        _tprintf(_T("%s: %s "), GetARMDString(I_WORK_MODE), GetARMDString(I_NOT_INITIALIZED));
                    else
                        _tprintf(_T("%s: %s "), GetARMDString(I_WORK_MODE), GetARMDString(I_INITIALIZED));
            }
            break;
            case EVENT_FEED:
                _tprintf(_T("%s:%f| "), GetARMDString(I_FEED), event_data->value.Float);
                break;
            case EVENT_SPINDLE_SPEED:
                _tprintf(_T("%s:%f| "), GetARMDString(I_SPINDLE_SPEED), event_data->value.Float);
                break;
            case EVENT_SYSTEM_STATE:
                _tprintf(_T("%s:%s| "), GetARMDString(I_SYSTEM_STATE), SYSTEM_STATE_ST[event_data->value.Char]);
                break;
            case EVENT_EMERGENCY_ERROR_MESSAGE:
                if (event_data->value.emergency_error)
                {
                    _tprintf(_T("%s: %d "), GetARMDString(I_PROGRAM_ERROR_MESSAGE), event_data->value.emergency_error->error_code);
                    if (event_data->value.emergency_error->msg_len != 0)
                        _tprintf(_T(" %s: %s| "), GetARMDString(I_MESSAGE), ByteToWide(MAX_TMP, tmp, event_data->value.emergency_error->msg));
                }
                break;
            case EVENT_PROGRAM_NAME:
                for (int i = 0; i < event_data->value.prog_name->num; i++)
                {
                    switch (event_data->value.prog_name->data[i].layer)
                    {
                    case ROUTINE: _tprintf(_T("%s "), GetARMDString(I_ROUTINE)); break;
                    case SUBROUTINE1: _tprintf(_T("%s 1 "), GetARMDString(I_SUBROUTINE)); break;
                    case SUBROUTINE2: _tprintf(_T("%s 2 "), GetARMDString(I_SUBROUTINE)); break;
                    }
                    _tprintf(_T("%s:%s| "), GetARMDString(I_NAME), ByteToWide(MAX_TMP, tmp, event_data->value.prog_name->data[i].name));
                    _tprintf(_T("%s:%s| "), GetARMDString(I_PATH), ByteToWide(MAX_TMP, tmp, event_data->value.prog_name->data[i].path));
                }
                break;
            case EVENT_CONTROL_PANEL_SWITCH_JOG:
                _tprintf(_T("%s:%f| "), GetARMDString(I_JOG_SWITCH), event_data->value.Float);
                break;
            case EVENT_CONTROL_PANEL_SWITCH_FEED:
                _tprintf(_T("%s:%f| "), GetARMDString(I_FEED_SWITCH), event_data->value.Float);
                break;
            case EVENT_CONTROL_PANEL_SWITCH_SPINDLE:
                _tprintf(_T("%s:%f| "), GetARMDString(I_SPINDLE_SWITCH), event_data->value.Float);
                break;
            case EVENT_BLOCK_NUMB_CTRL_PROG:
                _tprintf(_T("%s:%d| "), GetARMDString(I_BLOCK_NUMBER), event_data->value.Long);
                break;
            case EVENT_TOOL_NUMBER:
                _tprintf(_T("%s:%u| "), GetARMDString(I_TOOL_NUMBER), event_data->value.Word);
                break;
            case EVENT_CORRECTOR_NUMBER:
                _tprintf(_T("%s:%u| "), GetARMDString(I_CORRECTOR_NUMBER), event_data->value.Word);
                break;
            case EVENT_UAS:
                PrintARMDVariableState(GetARMDString(I_UAS), event_data->value.Char);
                break;
            case EVENT_UVR:
                PrintARMDVariableState(GetARMDString(I_UVR), event_data->value.Char);
                break;
            case EVENT_URL:
                PrintARMDVariableState(GetARMDString(I_URL), event_data->value.Char);
                break;
            case EVENT_COMU:
                PrintARMDVariableState(GetARMDString(I_COMU), event_data->value.Char);
                break;
            case EVENT_CEFA:
                PrintARMDVariableState(GetARMDString(I_CEFA), event_data->value.Char);
                break;
            case EVENT_MUSP:
                PrintARMDVariableState(GetARMDString(I_MUSP), event_data->value.Char);
                break;
            case EVENT_REAZ:
                PrintARMDVariableState(GetARMDString(I_REAZ), event_data->value.Char);
                break;
            case EVENT_MACHINE_IDLETIME_CAUSE:
                for (int ai = 0; ai < event_data->value.machine_idletime->num; ai++)
                {
                    if (event_data->value.machine_idletime->idle[ai].action == MACHINE_IDLETIME_SET)
                        _tprintf(_T("%s %s: "), GetARMDString(I_MACHINE_IDLE_TIME), GetARMDString(I_SET));
                    else
                        _tprintf(_T("%s %s: "), GetARMDString(I_MACHINE_IDLE_TIME), GetARMDString(I_RESET));
                    if (event_data->value.machine_idletime->idle[ai].group_len > 0)
                        _tprintf(_T("%s| "), ByteToWide(MAX_TMP, tmp, event_data->value.machine_idletime->idle[ai].group));
                    if (event_data->value.machine_idletime->idle[ai].len > 0)
                        _tprintf(_T("%s| "), ByteToWide(MAX_TMP, tmp, event_data->value.machine_idletime->idle[ai].str));
                }
                break;
            case EVENT_ALARM_PLC_ERR:
                _tprintf(_T("%s:%s| "), GetARMDString(I_PLC_ALARM), ByteToWide(MAX_TMP, tmp, event_data->value.alarm_plc_error->str));
                break;
            case EVENT_MESS_PLC_ERR:
                _tprintf(_T("%s:%s| "), GetARMDString(I_PLC_MESSAGE), ByteToWide(MAX_TMP, tmp, event_data->value.mess_plc_error->str));
                break;
            case EVENT_PROCESS_COMMAND_LINE:
                _tprintf(_T("%s:%s| "), GetARMDString(I_COMMNAD_FROM_PROCESS), ByteToWide(MAX_TMP, tmp, event_data->value.command_line->str));
                break;
            case EVENT_PROCESS_BLOCK_LINE:
                _tprintf(_T("%s:%s| "), GetARMDString(I_BLOCK_FROM_PROCESS), ByteToWide(MAX_TMP, tmp, event_data->value.command_line->str));
                break;
            case EVENT_COMMAND_LINE:
                _tprintf(_T("%s:%s| "), GetARMDString(I_COMMAND_LINE), ByteToWide(MAX_TMP, tmp, event_data->value.command_line->str));
                break;
            case EVENT_PART_FINISHED:
                if (event_data->value.Char == 1)
                    _tprintf(_T("%s |"), GetARMDString(I_PART_FINISHED));
                break;
            case EVENT_G_FUNCTIONS:
#define NO_G 0xFF
                _tprintf(_T("%s: "), GetARMDString(I_G_FUNCTIONS));
                for (int i = 0; i < event_data->value.g_functions->num; i++)
                    if (event_data->value.g_functions->g[i] != NO_G)
                        _tprintf(_T("%u "), event_data->value.g_functions->g[i]);
                    else
                        _tprintf(_T("  "));
                _tprintf(_T("| "));
                break;
            case EVENT_RISP:
                PrintARMDVariableState(GetARMDString(I_RISP), event_data->value.Char);
                break;
            case EVENT_CONP:
                PrintARMDVariableState(GetARMDString(I_CONP), event_data->value.Char);
                break;
            case EVENT_SPEPN_REQ:
                PrintARMDVariableState(GetARMDString(I_SPEPNREQ), event_data->value.Char);
                break;
            case EVENT_A_SPEPN:
                PrintARMDVariableState(GetARMDString(I_ASPEPN), event_data->value.Char);
                break;
            case EVENT_WNCMT:
                _tprintf(_T("%s:%s| "), GetARMDString(I_WNCMT), ByteToWide(MAX_TMP, tmp, event_data->value.subroutine_info->str));
                break;
            case EVENT_WNPRT:
                _tprintf(_T("%s:%s| "), GetARMDString(I_WPRT), ByteToWide(MAX_TMP, tmp, event_data->value.subroutine_info->str));
                break;
            case EVENT_WPROG:
                _tprintf(_T("%s:%s| "), GetARMDString(I_WPROG), ByteToWide(MAX_TMP, tmp, event_data->value.subroutine_info->str));
                break;
            case EVENT_WIZKD:
                _tprintf(_T("%s:%s| "), GetARMDString(I_WIZKD), ByteToWide(MAX_TMP, tmp, event_data->value.subroutine_info->str));
                break;
            case EVENT_TIME_SYNCH:
                if (event_data->value.Char == 1)
                    _tprintf(_T("%s| "), GetARMDString(I_TIME_SYNCH));
                break;
            case EVENT_SPINDLE_POWER:
                _tprintf(_T("%s:%f| "), GetARMDString(I_SPINDLE_POWER), event_data->value.Float);
                break;
            case EVENT_ARMD_SERVICE:
                _tprintf(_T("%s:%d| "), GetARMDString(I_SERVICE_MSG_CODE), event_data->value.Char);
                break;
            }
        }
    }
    _tprintf(_T("\n"));
    return 0;
}

