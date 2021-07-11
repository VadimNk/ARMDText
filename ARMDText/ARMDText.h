#ifndef _ARMD_TEXT_H_
#define _ARMD_TEXT_H_

#define WIN32_LEAN_AND_MEAN
#include "ARMDParserData.h"
#include <windows.h>
BOOL isVerobose();
void FreeARMDParseData(ARMDParserData* armd_parser_data);
#endif