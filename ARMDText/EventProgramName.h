#ifndef _EVENT_PROGRAM_NAME
#define _EVENT_PROGRAM_NAME
#include "ARMDFileReaderData.h"
#include "ARMDMessage.h"

void EventProgramNameFree(ProgName* prog_name);
int EventProgramName(ProgName** prog_name, ARMDFileReaderData* armd_file_reader_data);

#endif // !EVENT_PROGRAM_NAME

