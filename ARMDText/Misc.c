#include <malloc.h>
#include "Misc.h"

 int parseString(char** const str_out, ARMDFileReaderData* armd_file_reader_data)
{
	int status = NO_ERROR;
	BYTE len;
	GetValFromBuf(&len, armd_file_reader_data, sizeof(BYTE));
	char* str = (char*)malloc(((size_t)len + 1) * sizeof(char));
	if (str)
	{
		if (len > 0)
		{
			GetValFromBuf(str, armd_file_reader_data, len);
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
