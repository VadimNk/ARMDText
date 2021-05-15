#ifndef _EVENT_SYSTEM_START
#define _EVENT_SYSTEM_START
#include "ARMDParserData.h"
#include "ARMDMessage.h"

int EventSystemStart(SystemStartData** const system_start_data_out, ARMDParserData* armd_parser_data);
void FreeEventSystemStart(SystemStartData* system_start_data);
#endif // !_EVENT_SYSTEM_START

