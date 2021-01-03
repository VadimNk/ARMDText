#include <malloc.h>
#include "ARMDParserData.h"

void FreeARMDParseData(ARMDParserData* armd_parser_data)
{
    if (armd_parser_data)
    {
        armd_parser_data->parsed_file_len = 0;
        armd_parser_data->index = 0;
        if (armd_parser_data->buf)
            free(armd_parser_data->buf);
        armd_parser_data->max_buf = 0;
    }
}
