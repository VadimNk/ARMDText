#include <windows.h>
#include <malloc.h>
#include "EventProgramName.h"
#include "ARMDEventParser.h"

void EventProgramNameFree(ProgName* prog_name)
{
	if (prog_name->data)
	{
		for (short s_i = 0; s_i < prog_name->num; s_i++)
		{
			ProgNameData* program_name_data = prog_name->data + s_i;
			if (program_name_data->name)
				free(program_name_data->name);
			if (program_name_data->path)
				free(program_name_data->path);
		}
		free(prog_name->data);
	}
	free(prog_name);

}

int EventProgramName(ProgName** program_name_out, ARMDParserData* armd_parser_data)
{
	int event_program_name_result = ERROR_COMMON;
	BYTE str_len = 0;
	ProgName* program_name;

	program_name = (ProgName*)calloc(1, sizeof(ProgName));
	if (program_name)
	{
		GetValFromBuf(&program_name->num, armd_parser_data, sizeof(BYTE));
		program_name->data = (ProgNameData*)calloc(program_name->num, sizeof(ProgNameData));
		if (program_name->data)
		{
			for (int i = 0; i < program_name->num; i++)
			{
				ProgNameData* program_name_data = program_name->data + i;
				GetValFromBuf(&program_name_data->layer, armd_parser_data, sizeof(BYTE));
				GetValFromBuf(&str_len, armd_parser_data, sizeof(BYTE));
				program_name_data->name = (char*)malloc(((size_t)str_len + 1) * sizeof(char));
				if (program_name_data->name)
				{
					GetValFromBuf(program_name_data->name, armd_parser_data, str_len);
					*(program_name_data->name + str_len) = '\0';
					GetValFromBuf(&str_len, armd_parser_data, sizeof(BYTE));
					program_name_data->path = (char*)malloc(((size_t)str_len + 1) * sizeof(char));
					if (program_name_data->path)
					{
						GetValFromBuf(program_name_data->path, armd_parser_data, str_len);
						*(program_name_data->path + str_len) = '\0';
						event_program_name_result = ERROR_OK;
					}
					else
						event_program_name_result = ERROR_MEMORY_ALLOCATION_ERROR;
				}
				else
					event_program_name_result = ERROR_MEMORY_ALLOCATION_ERROR;
			}
		}
		else
			event_program_name_result = ERROR_MEMORY_ALLOCATION_ERROR;
	}
	else
		event_program_name_result = ERROR_MEMORY_ALLOCATION_ERROR;
	if (event_program_name_result < ERROR_OK)
	{
		*program_name_out = NULL;
		if (program_name)
			EventProgramNameFree(program_name);
	}
	else
		*program_name_out = program_name;

	return event_program_name_result;
}
