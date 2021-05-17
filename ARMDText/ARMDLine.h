#ifndef _ARMD_LINE_H
#define _ARMD_LINE_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct _armd_line_data {
	BYTE len;
	char* str;
}ARMDLineData;

typedef ARMDLineData* ARMDLine;
#endif