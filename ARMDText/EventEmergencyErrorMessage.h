#ifndef _EVENT_EMERGENCY_ERROR_MESSAGE_H
#define _EVENT_EMERGENCY_ERROR_MESSAGE_H
#include "ARMDFileReaderData.h"
#include "ARMDMessage.h"

int EventEmergencyErrorMessage(EmergencyErrorMessage** const emergency_error_message_out, ARMDFileReaderData* armd_file_reader_data);
void FreeEmergencyErrorMessage(EmergencyErrorMessage* emergency_error_message);
#endif // !_EVENT_EMERGENCY_ERROR_MESSAGE_H
