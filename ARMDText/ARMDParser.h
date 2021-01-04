#ifndef _ARMD_PARSER_H
#define _ARMD_PARSER_H
#include <windows.h>
#include "ARMDParserData.h"
#include "ARMDHeaderParser.h"
#include "ARMDMessageParser.h"
#include "ARMDProcessData.h"
#include "ARMDDisplay.h"
#include "ARMDError.h"

//----соотношение номеров событий и мнемонических названий--------------------------------------//
#define EVENT_NO_EVENT						1
#define EVENT_SYSTEM_START					2
#define EVENT_NEW_DATE						3
#define EVENT_WORK_MODE						4
#define EVENT_FEED							5
#define EVENT_SPINDLE_SPEED					6
#define EVENT_SYSTEM_STATE					7	//EVENT_SYSTEM_STATE == EVENT_PROGRAM_STATE
#define EVENT_MES_ERR_PROG					8
#define EVENT_PROGRAM_NAME					9
#define EVENT_CONTROL_PANEL_SWITCH_JOG		10
#define EVENT_CONTROL_PANEL_SWITCH_FEED		11
#define EVENT_CONTROL_PANEL_SWITCH_SPINDLE	12
#define EVENT_BLOCK_NUMB_CTRL_PROG			13
#define EVENT_TOOL_NUMBER					14
#define EVENT_CORRECTOR_NUMBER				15
#define EVENT_UAS							16
#define EVENT_UVR							17
#define EVENT_URL							18
#define EVENT_COMU							19
#define EVENT_CEFA							20
#define EVENT_MUSP							21
#define EVENT_REAZ							22
#define EVENT_MACHINE_IDLETIME_CAUSE		23
#define EVENT_ALARM_PLC_ERR					24
#define EVENT_MESS_PLC_ERR					25
#define EVENT_PROCESS_COMMAND_LINE			26
#define EVENT_PROCESS_BLOCK_LINE			27
#define EVENT_COMMAND_LINE					28
#define EVENT_PART_FINISHED					29
#define EVENT_G_FUNCTIONS					30
#define EVENT_RISP							31
#define EVENT_CONP							32
#define EVENT_SPEPN_REQ						33
#define EVENT_A_SPEPN						34
#define EVENT_WNCMT							35
#define EVENT_WNPRT							36
#define EVENT_WPROG							37
#define EVENT_WIZKD							38
#define EVENT_TIME_SYNCH					39
#define EVENT_SPINDLE_POWER					40
#define EVENT_ARMD_SERVICE					41

//---время простоя-----------------------------------------------------------------------------------------------------------------------------//
#define MACHINE_IDLETIME_SET 1
#define MACHINE_IDLETIME_RESET 2
//---------------------------------------------------------------------------------------------------------------------------------------------//

//---имя программы\подпрограммы----------------------------------------------------------------------------------------------------------------//
#define ROUTINE 0						//имя программы
#define SUBROUTINE1 1					//имя подпрограммы 1 уровня вложенности
#define SUBROUTINE2	2					//имя подпрограммы 2 уровня вложенности
//---------------------------------------------------------------------------------------------------------------------------------------------//

void GetValFromBuf(void* value, ARMDParserData* armd_parser_data, const DWORD value_size);
int GetARMDMessage(HANDLE console_output, ARMDHeaderInfo** armd_header_info, ARMDProcessedData* armd_processed_data,
    ARMDParserData* armd_parser_data, BOOL* no_event_state);

#endif // !_ARMD_PARSER_H