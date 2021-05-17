#ifndef _ARMD_INF_READER_H_
#define _ARMD_INF_READER_H_

#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <windows.h>
BOOL ReadInfFile(DWORD cnc_last_entry_max_characters, _TCHAR* cnc_last_entry);
#endif// !_ARMD_INF_READER_H_