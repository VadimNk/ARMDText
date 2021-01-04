#ifndef _ARMD_FILE_READER_H_
#define _ARMD_FILE_READER_H_
#include <tchar.h>
#include <windows.h>
#include "ARMDParserData.h"
int ReadARMDFile(DWORD current_file_name_max_characters, _TCHAR* current_file_name, ARMDParserData* armd_parser_data);
void ResetFileReader(ARMDParserData* armd_parser_data, ARMDProcessedData* armd_processed_data);
#endif// !_ARMD_FILE_READER_H_