#include <malloc.h>
#include "EventEmergencyErrorMessage.h"
#include "ARMDError.h"
#include "Misc.h"

void FreeEmergencyErrorMessage(EmergencyErrorMessage* emergency_error_message)
{
	if (emergency_error_message->msg)
		free(emergency_error_message->msg);
	free(emergency_error_message);
}

int EventEmergencyErrorMessage(EmergencyErrorMessage** const emergency_error_message_out, ARMDFileReaderData* armd_file_reader_data)
{
	int status = ERROR_OK;
	EmergencyErrorMessage* emergency_error_message = (EmergencyErrorMessage*)calloc(1, sizeof(EmergencyErrorMessage));
	if (emergency_error_message)
	{
		GetValFromBuf(&emergency_error_message->error_code, armd_file_reader_data, sizeof(char));
		GetValFromBuf(&emergency_error_message->msg_len, armd_file_reader_data, sizeof(emergency_error_message->msg_len));
		if (emergency_error_message->msg_len > 0)
		{
			emergency_error_message->msg = (char*)calloc((size_t)emergency_error_message->msg_len + 1, sizeof(char));
			if (emergency_error_message)
				GetValFromBuf(emergency_error_message->msg, armd_file_reader_data, emergency_error_message->msg_len);
			else
				status = ERROR_MEMORY_ALLOCATION_ERROR;
		}
		else
			emergency_error_message->msg = NULL;
	}
	else
		status = ERROR_MEMORY_ALLOCATION_ERROR;
	if (status < ERROR_OK)
	{
		if (emergency_error_message)
			FreeEmergencyErrorMessage(emergency_error_message);
	}
	else
		*emergency_error_message_out = emergency_error_message;
	return status;
}

