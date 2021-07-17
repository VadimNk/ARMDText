#ifndef _ARMD_BODY_PARSER_
#define _ARMD_BODY_PARSER_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "ARMDProcessedData.h"
#include "ARMDFileReaderData.h"

//длина события "нет события" в байтах
static const DWORD NO_EVENT_EVENT_LEN = sizeof(((ARMDMessageData*)0)->time) + sizeof(((ARMDMessageData*)0)->events_len) +
sizeof(((ARMDMessageData*)0)->num_proc) + sizeof(((ARMDProcessData*)0)->proc) + sizeof(((ARMDProcessData*)0)->num_event) + sizeof(short) + sizeof(BYTE);

BYTE CheckMessageData(const BYTE* const buffer, const DWORD start_index, const DWORD finish_index);
int ParseARMDMessage(ARMDMessageData* armd_data, ARMDHeaderInfo* armd_header_info, ARMDFileReaderData* armd_file_reader_data, BOOL* no_event_state);
int FreeARMDMessage(ARMDMessageData* armd_data);
#endif // !_ARMD_BODY_PARSER_
