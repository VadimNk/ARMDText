#ifndef _ARMD_MESSAGE_H
#define _ARMD_MESSAGE_H
#include <windows.h>
//----начало структуры данных событий АРМД-------------------//
typedef struct _g_functions {
    BYTE num;
    BYTE* g;
}GFunctions;

//------ProgName----------------------//
typedef struct _prog_name_data {
    BYTE layer;
    char* name;
    char* path;
}ProgNameData;

typedef struct _prog_name {
    BYTE num;
    ProgNameData* data;
}ProgName;
//------------------------------------//

typedef struct _emerg_data {
    char error_code;
    char msg_len;
    char* msg;
}EmergencyErrorMessage;

//------MachineIdleTime--------------//
typedef struct _idle {
    char action;
    BYTE len;
    BYTE group_len;
    char* group;
    char* str;
}Idle;

typedef struct _machine_idletime {
    BYTE num;
    Idle* idle;
}MachineIdleTime;
//--------------------------------------//

typedef struct _alarm_plc_error {
    BYTE log_len;
    char* log;
}PlcError;

//-----SystemStartData------------------//
typedef struct _characterization_file_data {
    char* logical_name;
    char* physical_name;
    char* destination;
}CharacterizationFileData;

typedef struct _system_start_data {
    WORD Year;
    WORD Month;
    WORD Day;
    DWORD time;
    BYTE character_files_num;
    CharacterizationFileData* file_data;
}SystemStartData;
//--------------------------------------//

typedef struct _command_line {
    BYTE len;
    char* str;
}CommandLine;

typedef struct _subroutine_info {
    BYTE len;
    char* str;
}SubroutineInfo;

typedef struct _armd_event_data {
    short event;					//событие
    union
    {
        char	Char;
        short	Short;
        int		Int;
        float	Float;
        long	Long;
        WORD	Word;
        char* str;
        void* Void;
        WORD* time;
        GFunctions* g_functions;
        ProgName* prog_name;
        EmergencyErrorMessage* emergency_error;
        MachineIdleTime* machine_idletime;
        PlcError* alarm_plc_error;
        PlcError* mess_plc_error;
        SystemStartData* system_start_data;
        CommandLine* command_line;
        SubroutineInfo* subroutine_info;
    }value;							//значение события
}ARMDEventData;

//----конец структуры данных событий АРМД-------------------//

typedef struct _armd_process_data {
    BYTE proc;						//номер процесса, в котором произошли события
    short num_event;				//кол-во событий в сообщении
    ARMDEventData* event_data;
}ARMDProcessData;

//----начало структура данных сообщений АРМД-------------------//
typedef struct _armd_message_data {
    DWORD time;						//время "появления" событий
    WORD events_len;				//длина сообщения
    BYTE num_proc;					//общее количество процессов
    ARMDProcessData* proc_data;
    BYTE check;						//контрольная сумма
}ARMDMessageData;
//----конец структура данных сообщений АРМД-------------------//
#endif // !_ARMD_MESSAGE_H
