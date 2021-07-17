#ifndef ARMD_PROCESS_EVENTS_PARSER_H
#define ARMD_PROCESS_EVENTS_PARSER_H
#include <windows.h>
#include "ARMDProcessedData.h"
#include "ARMDHeaderParser.h"

int ParseProcessEvents(ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDFileReaderData* armd_file_reader_data,
	BOOL* no_event_state, BYTE number_of_processes);
int FreeProcessesEvents(ARMDProcessData* armd_process_data);

#endif // !ARMD_PROCESS_EVENTS_PARSER_H

