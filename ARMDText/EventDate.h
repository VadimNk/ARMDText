#ifndef _EVENT_DATE_H
#define _EVENT_DATE_H
#include "ARMDParserData.h"
int EventDate(WORD** const date_out, ARMDParserData* armd_parser_data);
void FreeEventDate(WORD* date);
#endif // !_EVENT_DATE_H
