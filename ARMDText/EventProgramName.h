#ifndef EVENT_PROGRAM_NAME
#define EVENT_PROGRAM_NAME
#include "ARMDEventParser.h"

void EventProgramNameFree(ProgName* prog_name);
int EventProgramName(ProgName** prog_name, ARMDParserData* armd_parser_data);

#endif // !EVENT_PROGRAM_NAME

