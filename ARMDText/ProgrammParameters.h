#ifndef _PROGRAMM_PARAMETERS_H_
#define _PROGRAMM_PARAMETERS_H_
#include <tchar.h>
#include <time.h>
typedef struct _program_parameters {
    _TCHAR* specified_armd_file;
    clock_t delay_time_ms;
    int status;
}ProgrammParameters;

ProgrammParameters ParseProgramParameters(int argc, _TCHAR** argv);
void FreeParseProgramParameters(ProgrammParameters* programm_parameters);
void SetDefaultProgramParameters(ProgrammParameters* program_parameters);

#endif