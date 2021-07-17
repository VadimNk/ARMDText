#ifndef _PROGRAM_PARAMETERS_H_
#define _PROGRAM_PARAMETERS_H_
#include <tchar.h>
#include <time.h>
typedef struct _program_parameters {
    _TCHAR* specified_armd_file;
    clock_t delay_time_ms;
    int status;
}ProgramParameters;

ProgramParameters ParseProgramParameters(int argc, _TCHAR** argv);
void FreeParseProgramParameters(ProgramParameters* program_parameters);
void SetDefaultProgramParameters(ProgramParameters* program_parameters);

#endif