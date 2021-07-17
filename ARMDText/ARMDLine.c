// NOTE: Check misc.h 
#include <malloc.h>
#include "ARMDLine.h"
#include "ARMDError.h"
#include "Misc.h"

void FreeARMDLine(ARMDLine armd_string)
{
	if (armd_string->str)
		free(armd_string->str);
	free(armd_string);
}

int GetARMDLine(ARMDLine* armd_string_out, ARMDFileReaderData* armd_file_reader_data)
{
	int status = ERROR_COMMON;
	ARMDLine armd_string;
	armd_string = (ARMDLine)calloc(1, sizeof(ARMDLineData));
	if (armd_string)
	{
		GetValFromBuf(&armd_string->len, armd_file_reader_data, sizeof(BYTE));
		armd_string->str = (char*)calloc((size_t)armd_string->len + 1, sizeof(char));
		if (armd_string->str)
		{
			GetValFromBuf(armd_string->str, armd_file_reader_data, armd_string->len);
			*(armd_string->str + armd_string->len) = '\0';
			status = ERROR_OK;
		}
		else
			status = ERROR_MEMORY_ALLOCATION_ERROR;
	}
	else
		status = ERROR_MEMORY_ALLOCATION_ERROR;
	if (status >= ERROR_OK)
		*armd_string_out = armd_string;
	else
	{
		*armd_string_out = NULL;
		FreeARMDLine(armd_string);
	}
	return status;
}