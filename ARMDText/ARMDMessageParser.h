#ifndef _ARMD_BODY_PARSER_
#define _ARMD_BODY_PARSER_
#include <windows.h>
//#include "ARMDParser.h"
#include "ARMDProcessData.h"
#include "ARMDParserData.h"

//длина события "нет события" в байтах
static const DWORD NO_EVENT_EVENT_LEN = sizeof(((ARMDMessageData*)0)->time) + sizeof(((ARMDMessageData*)0)->events_len) +
sizeof(((ARMDMessageData*)0)->num_proc) + sizeof(((ARMDProcessData*)0)->proc) + sizeof(((ARMDProcessData*)0)->num_event) + sizeof(short) + sizeof(BYTE);

int ParseARMDMessage(ARMDHeaderInfo* armd_header_info, ARMDProcessedData* armd_processed, ARMDParserData* armd_parser_data, BOOL* no_event_state);
int FreeEventData(ARMDMessageData* armd_data);
#endif // !_ARMD_BODY_PARSER_