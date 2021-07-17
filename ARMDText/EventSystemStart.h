#ifndef _EVENT_SYSTEM_START
#define _EVENT_SYSTEM_START
#include "ARMDFileReaderData.h"
#include "ARMDMessage.h"

int EventSystemStart(SystemStartData** const system_start_data_out, ARMDFileReaderData* armd_file_reader_data);
void FreeEventSystemStart(SystemStartData* system_start_data);
#endif // !_EVENT_SYSTEM_START

