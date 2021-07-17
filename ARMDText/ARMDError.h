#ifndef _ARMD_ERROR_H_
#define _ARMD_ERROR_H_
typedef enum _armd_parse_errors {
    ERROR_NOTHING_TO_PROCESS = 1,
    ERROR_OK = 0,
    ERROR_COMMON = -1,
    ERROR_INVALID_FUNCTION_PARAMETER = -2,
    ERROR_NULL_POINTER = -3,
    ERROR_OUT_OF_RANGE = -4,
    ERROR_MEMORY_ALLOCATION_ERROR = -5,
    ERROR_WRONG_CHECK = -6,
    ERROR_READING_FILE = -7,
    ERROR_OPENING_FILE = -8,
    ERROR_MOVING_TO_POSITION_FILE = -9
}ARMDParserErrors;
#endif // !_ARMD_ERROR_H_
