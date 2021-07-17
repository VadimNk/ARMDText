#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "ARMDHeaderDisplay.h"
#include "ARMDHeaderParser.h"
#include "ARMDDisplayStrings.h"
#include "ARMDText.h"

void DisplayHeader(HANDLE console_output, ARMDHeaderInfo* const armd_header_info)
{
#define MAX_TMP 512
    TCHAR tmp[MAX_TMP];
    if (armd_header_info == NULL)
        return;

    SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    if (isVerobose())
        _tprintf(_T("%s: %s\n"), GetARMDString(I_FILE_IDENTIFIER), ByteToWide(MAX_TMP, tmp, armd_header_info->identifier));

    if (isVerobose())
        _tprintf(_T("%s: %s\n"), GetARMDString(I_VERSION_OF_ARMD_DATA_STRUCTURES), ByteToWide(MAX_TMP, tmp, armd_header_info->ver));
    _tprintf(_T("%s: %u %s\n"), GetARMDString(I_FILE_SIZE), armd_header_info->file_size, GetARMDString(I_BYTES));

    if (armd_header_info->prev_file_name[0] != '\0')
        _tprintf(_T("%s: %s\n"), GetARMDString(I_PREVIOUS_FILE), ByteToWide(MAX_TMP, tmp, armd_header_info->prev_file_name));
    else
        _tprintf(_T("%s: %s.\n"), GetARMDString(I_PREVIOUS_FILE), GetARMDString(I_NO_PREVIOUS_FILE));

    if (armd_header_info->next_file_name[0] != '\0')
        _tprintf(_T("%s: %s\n"), GetARMDString(I_NEXT_FILE), ByteToWide(MAX_TMP, tmp, armd_header_info->next_file_name));
    else
        _tprintf(_T("%s: %s.\n"), GetARMDString(I_NEXT_FILE), GetARMDString(I_NO_NEXT_FILE));

    _tprintf(_T("%s.\n"), armd_header_info->withdraw ? GetARMDString(I_FILE_CHAIN_HAS_BEEN_TORN) : GetARMDString(I_FILE_CHAIN_IS_INTACT));
    if (isVerobose())
        _tprintf(_T("%s: %u \n"), GetARMDString(I_LENGTH_OF_SOFTWARE_VERSION_STRING), armd_header_info->software_version_len);

    if (armd_header_info->software_version)
    {
        _tprintf(_T("%s: %s\n"), GetARMDString(I_SOFTWARE_VERSION), ByteToWide(MAX_TMP, tmp, armd_header_info->software_version));
        _tprintf(_T("%s: %s\n"), GetARMDString(I_CNC_MACHINE_NAME), ByteToWide(MAX_TMP, tmp, armd_header_info->machine_name));
        _tprintf(_T("%s: %d (%s)\n"), GetARMDString(I_CNC_TIMER_TICK), armd_header_info->system_tick, GetARMDString(I_MS));
        _tprintf(_T("%s: %d (%s.)\n"), GetARMDString(I_ARMD_DATA_WRITTING_INTERVAL),
            armd_header_info->record_time * armd_header_info->system_tick / 1000, GetARMDString(I_SEC));
        _tprintf(_T("%s: %.4u.%.2u.%.2u\n"), GetARMDString(I_FILE_CREATION_DATE),
            armd_header_info->SysTime.wYear, armd_header_info->SysTime.wMonth, armd_header_info->SysTime.wDay);
        _tprintf(_T("%s.\n"), armd_header_info->local ?
            GetARMDString(I_FILE_HAS_BEEN_CREATED_ON_CNC_LOCAL_DRIVE) : GetARMDString(I_FILE_HAS_BEEN_CREATED_ON_TARGET_DEVICE));
        _tprintf(_T("\n"));

        _tprintf(_T("%s: %u\n"), GetARMDString(I_NUMBER_OF_ALL_PROCESSES), armd_header_info->num_proc);
        if (armd_header_info->proc_info)
        {
            for (short i = 0; i < armd_header_info->num_proc; i++)
            {
                SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                _tprintf(_T("%s: %d "), GetARMDString(I_PROCESS_NUMBER), i);
                _tprintf(_T("%s: %d\n"), GetARMDString(I_NUMBER_OF_ALL_EVENTS), armd_header_info->proc_info[i].num_events);
                if (armd_header_info->proc_info[i].event_info)
                {
                    for (short j = 0; j < armd_header_info->proc_info[i].num_events; j++)
                    {
                        SetConsoleTextAttribute(console_output, j % 2 ? FOREGROUND_GREEN | FOREGROUND_INTENSITY : FOREGROUND_RED | FOREGROUND_INTENSITY);

                        _tprintf(_T("%s: %3d  "), GetARMDString(I_INDEX), j);
                        _tprintf(_T("%s: %3d  "), GetARMDString(I_EVENT), armd_header_info->proc_info[i].event_info[j].event_id);
                        _tprintf(_T("%35s  "), GetARMDString(armd_header_info->proc_info[i].event_info[j].event_id + I_RESET));
                        _tprintf(_T("%s: %3d  "), GetARMDString(I_ACCOUNT_TYPE), armd_header_info->proc_info[i].event_info[j].account_type);
                        _tprintf(_T("%s: %10.2f  "), GetARMDString(I_ACCOUNT_PARAMETER), armd_header_info->proc_info[i].event_info[j].account_param);
                        _tprintf(_T("%s: %u\n"), GetARMDString(I_UNITS), armd_header_info->proc_info[i].event_info[j].units);
                    }
                }
                else
                {
                    _tprintf(_T("%s! %s.\n"), GetARMDString(I_ERROR), GetARMDString(I_NO_DATA_ABOUT_EVENTS));
                    break;
                }
            }
        }
        else
            _tprintf(_T("%s! %s.\n"), GetARMDString(I_ERROR), GetARMDString(I_NO_DATA_ABOUT_PROCESSES));
    }
    else
        _tprintf(_T("%s! %s.\n"), GetARMDString(I_ERROR), GetARMDString(I_NO_SOFTWARE_VERSION));
}