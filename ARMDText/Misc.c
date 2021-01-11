#include "Misc.h"

char* ParceString(ARMDParserData* armd_parser_data)
{
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
	return str;
}
