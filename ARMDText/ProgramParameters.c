#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include "ProgramParameters.h"
#include "ARMDError.h"
#include "ARMDDisplayStrings.h"

#define SECONDS_TO_MILLISECONDS(x) x * 1000

void SetDefaultProgramParameters(ProgramParameters* program_parameters)
{
	if (program_parameters)
	{
		program_parameters->specified_armd_file = NULL;
		program_parameters->delay_time_ms = SECONDS_TO_MILLISECONDS(1);
		program_parameters->status = ERROR_OK;
	}
}

void FreeParseProgramParameters(ProgramParameters* program_parameters)
{
	if (program_parameters)
	{
		if (program_parameters->specified_armd_file)
			free(program_parameters->specified_armd_file);
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
// cmd example: /file 201902323.mon /delay 100 /language russian
ProgramParameters ParseProgramParameters(int argc, _TCHAR * *argv)
{
	_TCHAR tmp_str[MAX_PATH];

	ProgramParameters program_parameters;
	SetDefaultProgramParameters(&program_parameters);
	for (int i = 1; i < argc; i++)
	{
		if( ToLower(tmp_str, MAX_PATH, *(argv + i)) == 0)
		{
			if (_tcscmp(tmp_str, _T("/file")) == 0)
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
							program_parameters.specified_armd_file = (_TCHAR*)malloc((str_len + 1) * sizeof(_TCHAR));
							if (program_parameters.specified_armd_file)
								_tcscpy_s(program_parameters.specified_armd_file, str_len + 1, tmp_str);
							else
								program_parameters.status = ERROR_MEMORY_ALLOCATION_ERROR;
						}
					}
				}
			}
			else
			{
				if (_tcscmp(tmp_str, _T("/delay")) == 0)
				{
					if (i + 1 < argc)
						i++;
					if (ToLower(tmp_str, MAX_PATH, *(argv + i)) == 0)
					{
						long value = _ttol(*(argv + i));
						if (value == 0)
							value = 1;
						program_parameters.delay_time_ms = SECONDS_TO_MILLISECONDS(value);
					}
				}
				else
				{
					if (_tcscmp(tmp_str, _T("/language")) == 0)
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
	return program_parameters;
}
