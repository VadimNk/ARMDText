#ifndef _ARMD_LINE_H_
#define _ARMD_LINE_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//#include "ARMDFileReader.h"
#include "ARMDFileReaderData.h"
typedef struct _armd_line_data {
	BYTE len;
	char* str;
}ARMDLineData;

typedef ARMDLineData* ARMDLine;

int GetARMDLine(ARMDLine* armd_string_out, ARMDFileReaderData* armd_file_reader_data);
void FreeARMDLine(ARMDLine armd_string);

#endif