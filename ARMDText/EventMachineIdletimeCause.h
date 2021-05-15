#ifndef _EVENT_MACHINE_IDLETIME_CAUSE_H
#define _EVENT_MACHINE_IDLETIME_CAUSE_H
#include "ARMDParserData.h"
#include "ARMDMessage.h"
void FreeEventMachineIdletimeCause(MachineIdleTime* const machine_idletime);
int EventMachineIdletimeCause(MachineIdleTime** const machine_idletime_out, ARMDParserData* armd_parser_data);
#endif // !_EVENT_MACHINE_IDLETIME_CAUSE_H