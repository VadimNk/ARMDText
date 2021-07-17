#ifndef ARMD_EVENT_PARSER_H
#define ARMD_EVENT_PARSER_H
#include "ARMDMessage.h"
#include "ARMDHeaderParser.h"
#include "ARMDFileReaderData.h"

int FreeEventValue(ARMDEventId event_id, ARMDEventValue* value);
int FreeEventData(ARMDProcessData* process_data);
int ParseEventNumber(ARMDEventId* armd_event_out, ARMDProcessData* current_proc_data, ARMDHeaderInfo* armd_header_info, ARMDFileReaderData* armd_file_reader_data);
int ParseEventData(ARMDEventData* event_data, ARMDEventId armd_event, ARMDFileReaderData* armd_file_reader_data, BOOL* no_event_state);
#endif // ARMD_EVENT_PARSER_H