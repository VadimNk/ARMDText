#include "ARMDString.h"
#include "ARMDError.h"
#include "ARMDParserData.h"

void FreeARMDString(ARMDString armd_string)
{
	if (armd_string->str)
		free(armd_string->str);
	free(armd_string);
}

int GetARMDString(ARMDString* armd_string_out, ARMDParserData* armd_parser_data)
{
	int status = ERROR_COMMON;
	ARMDString armd_string;
	armd_string = (ARMDString)calloc(1, sizeof(ARMDStringData));
	if (armd_string)
	{
		GetValFromBuf(&armd_string->len, armd_parser_data, sizeof(BYTE));
		armd_string->str = (char*)calloc((size_t)armd_string->len + 1, sizeof(char));
		if (armd_string->str)
		{
			GetValFromBuf(armd_string->str, armd_parser_data, armd_string->len);
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
		FreeARMDString(armd_string);
	}
	return status;
}