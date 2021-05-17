#include <malloc.h>
#include "EventDate.h"
#include "ARMDError.h"
#include "Misc.h"

void FreeEventDate(WORD* date)
{
	free(date);
}

int EventDate(WORD** const date_out, ARMDParserData* armd_parser_data)
{
	int status = ERROR_OK;
#define YEAR_MONTH_DAY 3
	WORD* date = (WORD*)calloc(YEAR_MONTH_DAY, sizeof(WORD));
	if (date)
		GetValFromBuf(date, armd_parser_data, YEAR_MONTH_DAY * sizeof(WORD));
	else
		status = ERROR_MEMORY_ALLOCATION_ERROR;
	*date_out = date;
	return status;
}