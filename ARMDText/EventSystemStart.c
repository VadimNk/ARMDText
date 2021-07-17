#include <malloc.h>
#include "EventSystemStart.h"
#include "ARMDParser.h"
#include "ARMDError.h"
#include "Misc.h"

void FreeCharacterizationFiles(CharacterizationFileData* file_data, BYTE character_files_num)
{
	for (int i = 0; i < character_files_num; i++)
	{
		free((file_data + i)->logical_name);
		free((file_data + i)->physical_name);
		free((file_data + i)->destination);
	}
	free(file_data);
}

void FreeEventSystemStart(SystemStartData* system_start_data)
{
	if (system_start_data->file_data)
		FreeCharacterizationFiles(system_start_data->file_data, system_start_data->character_files_num);
	free(system_start_data);
}

//информация о файлах характеризации
int GetCharacterizationFiles(BYTE* const character_files_num_out, CharacterizationFileData** const file_data_out, ARMDFileReaderData* const armd_file_reader_data)
{
	int i;
	int status = ERROR_OK;
	BYTE character_files_num;
	CharacterizationFileData* file_data = NULL;
	GetValFromBuf(&character_files_num, armd_file_reader_data, sizeof(BYTE));
	if (character_files_num > 0)
	{
		file_data = (CharacterizationFileData*)calloc(character_files_num, sizeof(CharacterizationFileData));
		if (file_data)
		{
			for (i = 0; i < character_files_num; i++)
			{
				status = parseString(&file_data[i].logical_name, armd_file_reader_data);
				if (status < ERROR_OK)
					break;

				status = parseString(&file_data[i].physical_name, armd_file_reader_data);
				if (status < ERROR_OK)
					break;

				status = parseString(&file_data[i].destination, armd_file_reader_data);
				if (status < ERROR_OK)
					break;

			}
			if (status < ERROR_OK)
				FreeCharacterizationFiles(file_data, i + 1);
		}
		else
			status = ERROR_MEMORY_ALLOCATION_ERROR;
	}
	if (status >= ERROR_OK)
	{
		*character_files_num_out = character_files_num;
		*file_data_out = file_data;
	}
	else
	{
		*character_files_num_out = 0;
		*file_data_out = NULL;
	}
	return status;
}

int EventSystemStart(SystemStartData** const system_start_data_out, ARMDFileReaderData* armd_file_reader_data)
{
	int parse_events_system_start_status = ERROR_OK;
	int status = ERROR_OK;
	SystemStartData* system_start_data = (SystemStartData*)calloc(1, sizeof(SystemStartData));
	if (system_start_data)
	{
		GetValFromBuf(&system_start_data->Year, armd_file_reader_data, sizeof(WORD));
		GetValFromBuf(&system_start_data->Month, armd_file_reader_data, sizeof(WORD));
		GetValFromBuf(&system_start_data->Day, armd_file_reader_data, sizeof(WORD));
		GetValFromBuf(&system_start_data->time, armd_file_reader_data, sizeof(DWORD));
		status = GetCharacterizationFiles(&system_start_data->character_files_num, &system_start_data->file_data, armd_file_reader_data);
		if (status < ERROR_OK)
			parse_events_system_start_status = status;
	}
	else
		parse_events_system_start_status = ERROR_MEMORY_ALLOCATION_ERROR;
	if (status < ERROR_OK)
	{
		FreeEventSystemStart(system_start_data);
		system_start_data = NULL;
	}
	*system_start_data_out = system_start_data;
	return parse_events_system_start_status;
}

