#include "Misc.h"

 int ParceString(char** const str_out, ARMDParserData* armd_parser_data)
{
	int status = NO_ERROR;
	BYTE len;
	GetValFromBuf(&len, armd_parser_data, sizeof(BYTE));
	char* str = (char*)malloc(((size_t)len + 1) * sizeof(char));
	if (str)
	{
		if (len > 0)
		{
			GetValFromBuf(str, armd_parser_data, len);
			*(str + len) = '\0';
		}
		else
			*str = '\0';
	}
	else 
		status = ERROR_MEMORY_ALLOCATION_ERROR;
	*str_out = str;
	return status;
}
