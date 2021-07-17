#include <malloc.h>
#include "EventMachineIdletimeCause.h"
#include "ARMDError.h"
#include "Misc.h"
void FreeEventMachineIdletimeCause(MachineIdleTime* const machine_idletime)
{
	if (machine_idletime->idle)
	{
		for (int i = 0; i < machine_idletime->num; i++)
		{
			Idle* idle = machine_idletime->idle + i;
			if (idle->group)
				free(idle->group);
			if (idle->str)
				free(idle->str);
		}
		free(machine_idletime->idle);
		free(machine_idletime);
	}
}
int EventMachineIdletimeCause(MachineIdleTime** const machine_idletime_out, ARMDFileReaderData* armd_file_reader_data)
{
	int status = ERROR_OK;
	MachineIdleTime *machine_idletime = (MachineIdleTime*)calloc(1, sizeof(MachineIdleTime));
	if (machine_idletime)
	{
		GetValFromBuf(&machine_idletime->num, armd_file_reader_data, sizeof(BYTE));
		if (machine_idletime->num)
		{
			machine_idletime->idle = (Idle*)calloc(machine_idletime->num, sizeof(Idle));
			if (machine_idletime->idle)
			{
				for (int i = 0; i < machine_idletime->num; i++)
				{
					Idle* idle = machine_idletime->idle + i;
					GetValFromBuf(&idle->action, armd_file_reader_data, sizeof(char));
					GetValFromBuf(&idle->group_len, armd_file_reader_data, sizeof(BYTE));
					if (idle->group_len > 0)
					{
						idle->group = (char*)calloc((size_t)idle->group_len + 1, sizeof(char));
						if (idle->group)
							GetValFromBuf(idle->group, armd_file_reader_data, idle->group_len * sizeof(char));
						else
							status = ERROR_MEMORY_ALLOCATION_ERROR;
					}
					GetValFromBuf(&idle->len, armd_file_reader_data, sizeof(BYTE));
					if (idle->len > 0)
					{
						idle->str = (char*)calloc((size_t)idle->len + 1, sizeof(char));
						if (idle->str)
							GetValFromBuf(idle->str, armd_file_reader_data, idle->len * sizeof(char));
						else
							status = ERROR_MEMORY_ALLOCATION_ERROR;
					}
				}
			}
			else
				status = ERROR_MEMORY_ALLOCATION_ERROR;
		}
	}
	else
		status = ERROR_MEMORY_ALLOCATION_ERROR;
	if (status >= NO_ERROR)
		*machine_idletime_out = machine_idletime;
	else
	{
		FreeEventMachineIdletimeCause(machine_idletime);
		*machine_idletime_out = NULL;
	}
	return status;
}