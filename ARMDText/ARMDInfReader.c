#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include "ARMD.h"
#include "ARMDDisplayStrings.h"

BOOL ReadInfFile(DWORD cnc_last_entry_max_characters, _TCHAR* cnc_last_entry)
{
    BOOL read_inf_file_result = FALSE;
    HANDLE current_inf_handle;
    _TCHAR information_file_name[MAX_PATH];
    char s_cnc_last_entry[MAX_ARMD_FILE_NAME + 1];
    DWORD last_entry_bytes_have_read;
    const TCHAR* const armd_information_file = _T("current.inf");
    if (cnc_last_entry)
    {
        _tcscpy_s(cnc_last_entry, cnc_last_entry_max_characters, armd_information_file);
        //Открываем файл, в котором хранится имя файла истории. В файл истории с этим именем программа ЧПУ произвела последнюю запись
        _tcscpy_s(information_file_name, MAX_PATH, armd_information_file);
        current_inf_handle = CreateFile(information_file_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (current_inf_handle != INVALID_HANDLE_VALUE)
        {
            SetFilePointer(current_inf_handle, 0, NULL, FILE_BEGIN);
            BOOL read_file_result = ReadFile(current_inf_handle, s_cnc_last_entry, MAX_ARMD_FILE_NAME, &last_entry_bytes_have_read, NULL);
            CloseHandle(current_inf_handle);
            if (read_file_result && MAX_ARMD_FILE_NAME == last_entry_bytes_have_read)
            {
                s_cnc_last_entry[MAX_ARMD_FILE_NAME] = '\0';
#ifdef UNICODE
                int mbwc_result = MultiByteToWideChar(CP_ACP, 0, s_cnc_last_entry, MAX_ARMD_FILE_NAME + 1, cnc_last_entry, cnc_last_entry_max_characters);
                if (mbwc_result == MAX_ARMD_FILE_NAME + 1)
                    read_inf_file_result = TRUE;
                else
                    _tprintf(_T("%s.\n"), GetARMDString(I_CANT_CONVERT_MULTI_BYTE_TO_WIDE_CHARACTER));
#else
                strcpy_s(cnc_last_entry, cnc_last_entry_max_characters, s_cnc_last_entry);
                read_inf_file_result = TRUE;
#endif
            }
            else
                _tprintf(_T("%s %s\n"), GetARMDString(I_INACCESSIBLE_OR_WRONG_DATA_IN_FILE), information_file_name);
        }
        else
            _tprintf(_T("%s: %s! %s...\n"), GetARMDString(I_CANT_OPEN), armd_information_file, GetARMDString(I_RETRY));
    }
    return read_inf_file_result;
}

