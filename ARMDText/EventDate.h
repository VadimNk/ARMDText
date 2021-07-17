#ifndef _EVENT_DATE_H
#define _EVENT_DATE_H
#include "ARMDFileReaderData.h"
int EventDate(WORD** const date_out, ARMDFileReaderData* armd_file_reader_data);
void FreeEventDate(WORD* date);
#endif // !_EVENT_DATE_H
