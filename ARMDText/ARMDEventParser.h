#ifndef ARMD_EVENT_PARCER_H
#define ARMD_EVENT_PARCER_H

#include "ARMDParserData.h"
#include "ARMDMessage.h"
#include "ARMDError.h"

int ParceEventDate(WORD** const date_out, ARMDParserData* armd_parser_data);
void FreeEventDate(WORD* date);
int ParseEmergencyErrorMessage(EmergencyErrorMessage** const emergency_error_message_out, ARMDParserData* armd_parser_data);
void FreeEmergencyErrorMessage(EmergencyErrorMessage* emergency_error_message);

#endif