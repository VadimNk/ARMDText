#include <windows.h>
#include <stdio.h>
#include "ProgrammParameters.h"
#include "ARMDError.h"
#include "ARMDDisplayStrings.h"

#define SECONDS_TO_MILLISECONDS(x) x * 1000

void SetDefaultProgramParameters(ProgrammParameters* program_parameters)
{
	if (program_parameters)
	{
		program_parameters->specified_armd_file = NULL;
		program_parameters->delay_time_ms = SECONDS_TO_MILLISECONDS(1);
		program_parameters->status = ERROR_OK;
	}
}

void FreeParseProgramParameters(ProgrammParameters* programm_parameters)
{
	if (programm_parameters)
	{
		if (programm_parameters->specified_armd_file)
			free(programm_parameters->specified_armd_file);
	}
}

int ToLower(_TCHAR * tmp_str, DWORD max_str, _TCHAR* argv)
{
	_tcscpy_s(tmp_str, max_str, argv);
	errno_t err = _tcslwr_s(tmp_str, max_str);
	if (err == errno)
		return 0;
	else
		return -1;
}
//-file 201902323.mon -delay 100 -lanuage russian
ProgrammParameters ParseProgramParameters(int argc, _TCHAR * *argv)
{
	_TCHAR tmp_str[MAX_PATH];

	ProgrammParameters programm_parameters;
	SetDefaultProgramParameters(&programm_parameters);
	for (int i = 1; i < argc; i++)
	{
		if( ToLower(tmp_str, MAX_PATH, *(argv + i)) == 0)
		{
			if (_tcscmp(tmp_str, _T("-file")) == 0)
			{
				if (i + 1 < argc)
					i++;
				if (ToLower(tmp_str, MAX_PATH, *(argv + i)) == 0)
				{
					_TCHAR* p = _tcsstr(tmp_str, _T(".mon"));
					if (p)
					{//файл задан в командной строке пользователем
						size_t str_len = _tcslen(tmp_str);
						if (str_len > 0)
						{
							programm_parameters.specified_armd_file = (_TCHAR*)malloc((str_len + 1) * sizeof(_TCHAR));
							if (programm_parameters.specified_armd_file)
								_tcscpy_s(programm_parameters.specified_armd_file, str_len + 1, tmp_str);
							else
								programm_parameters.status = ERROR_MEMORY_ALLOCATION_ERROR;
						}
					}
				}
			}
			else
			{
				if (_tcscmp(tmp_str, _T("-delay")) == 0)
				{
					if (i + 1 < argc)
						i++;
					if (ToLower(tmp_str, MAX_PATH, *(argv + i)) == 0)
					{
						long value = _ttol(*(argv + i));
						if (value == 0)
							value = 1;
						programm_parameters.delay_time_ms = SECONDS_TO_MILLISECONDS(value);
					}
				}
				else
				{
					if (_tcscmp(tmp_str, _T("-language")) == 0)
					{
						if (i + 1 < argc)
							i++;
						if (ToLower(tmp_str, MAX_PATH, *(argv + i)) == 0)
						{
							if (_tcscmp(tmp_str, _T("russian")) == 0)
								InitStrings(RUSSIAN_LANGUAGE);
						}
					}
				}
			}
		}
	}
	return programm_parameters;
}
