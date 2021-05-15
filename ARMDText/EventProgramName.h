#ifndef _EVENT_PROGRAM_NAME
#define _EVENT_PROGRAM_NAME
#include "ARMDParserData.h"
#include "ARMDMessage.h"

void EventProgramNameFree(ProgName* prog_name);
int EventProgramName(ProgName** prog_name, ARMDParserData* armd_parser_data);

#endif // !EVENT_PROGRAM_NAME

