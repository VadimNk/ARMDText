#ifndef ARMD_EVENT_PARCER_H
#define ARMD_EVENT_PARCER_H

#include "ARMDParserData.h"
#include "ARMDMessage.h"
#include "ARMDError.h"

int ParceEventSystemStart(SystemStartData** const system_start_data_out, ARMDParserData* armd_parser_data);
void FreeEventSystemStart(SystemStartData* system_start_data);
int ParceEventDate(WORD** const date_out, ARMDParserData* armd_parser_data);
void FreeEventDate(WORD* date);

#endif