//текст
#ifndef _ARMD_FILE_READER_H_
#define _ARMD_FILE_READER_H_

#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <windows.h>
#include "ARMDFileReaderData.h"
#include "ARMDProcessedData.h"

int ReadARMDFile(DWORD current_file_name_max_characters, _TCHAR* current_file_name, ARMDFileReaderData* armd_file_reader_data);
void ResetFileReader(ARMDFileReaderData* armd_file_reader_data, ARMDProcessedData* armd_processed_data);
#endif// !_ARMD_FILE_READER_H_