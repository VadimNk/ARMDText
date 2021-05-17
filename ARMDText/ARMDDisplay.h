#ifndef _ARMD_DISPLAY_H_
#define _ARMD_DISPLAY_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "ARMDParser.h"

int ShowVal(HANDLE console_output, ARMDMessageData* data);
#endif // !_ARMD_DISPLAY_H_

