#ifndef _ARMD_HEADER_DISPLAY_H_
#define _ARMD_HEADER_DISPLAY_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "ARMDHeaderParser.h"

void DisplayHeader(HANDLE console_output, ARMDHeaderInfo* const armd_header_info);

#endif