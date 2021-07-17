#ifndef _ARMD_TEXT_H_
#define _ARMD_TEXT_H_

#define WIN32_LEAN_AND_MEAN
#include "ARMDFileReaderData.h"
#include <windows.h>
BOOL isVerobose();
void FreeARMDParseData(ARMDFileReaderData* armd_file_reader_data);
#endif