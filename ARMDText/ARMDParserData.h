#ifndef _ARMD_PARSER_DATA
#define _ARMD_PARSER_DATA

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#define NO_EVENT_STATE 0x1
//в структуре хранятся данные, которые необходимы в процессе обработки данных
typedef struct _armd_parser_data {
    BYTE flag;
    DWORD index; //объем данных в байтах, который был обработан в текущем буфере 
    DWORD parsed_file_len; //объем данных в байтах, который был обработан во всём файле
    size_t max_buf; //максимальный объем буфера
    BYTE* buf; //указатель на буфер, который читает из файла
}ARMDParserData;

void FreeARMDParseData(ARMDParserData* armd_parser_data);

#endif // !_ARMD_PARSER_DATA
