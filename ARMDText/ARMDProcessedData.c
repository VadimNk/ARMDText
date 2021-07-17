#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include "ARMDParser.h"
#include "ARMDMessageParser.h"
#include "ARMDProcessedData.h"
#include "ARMDDisplayStrings.h"

void FreeProcessedData(ARMDProcessedData* armd_processed_data)
{
    if (armd_processed_data)
    {
        if (armd_processed_data->data)
        {
            for (DWORD d_i = 0; d_i < armd_processed_data->max_items; d_i++)
                free(armd_processed_data->data[d_i]);
            free(armd_processed_data->data);
            armd_processed_data->number_items = 0;
            armd_processed_data->data = NULL;
            armd_processed_data->max_items = 0;
        }
    }
}

int EnsureCapacityOfProcessedData(ARMDProcessedData* armd_processed_data)
{
#define ADDITIONAL_ITEMS 1000
    int ensure_capacity_status = ERROR_OK;
    if (armd_processed_data)
    {
        if (armd_processed_data->number_items >= armd_processed_data->max_items)
        {
            DWORD new_max_items = armd_processed_data->max_items + ADDITIONAL_ITEMS;
            void* tmp_processed_data = realloc(armd_processed_data->data, ((size_t)armd_processed_data->max_items + new_max_items) * sizeof(ARMDMessageData*));
            if (tmp_processed_data)
            {
                armd_processed_data->data = (ARMDMessageData**)tmp_processed_data;
                for (DWORD dw_i = armd_processed_data->max_items; dw_i < new_max_items; dw_i++)
                {
                    *(armd_processed_data->data + dw_i) = (ARMDMessageData*)malloc(sizeof(ARMDMessageData));
                    if (!*(armd_processed_data->data + dw_i))
                    {
                        for (DWORD dw_j = armd_processed_data->max_items; dw_j < dw_i; dw_j++)
                            free(*(armd_processed_data->data + dw_i));
                        ensure_capacity_status = ERROR_MEMORY_ALLOCATION_ERROR;
                        break;
                    }
                }
                if (ensure_capacity_status == ERROR_OK)
                    armd_processed_data->max_items = new_max_items;
            }
            else
            {
                _tprintf(_T("%s!"), GetARMDString(I_MEMORY_ALLOCATION_ERROR));
                ensure_capacity_status = ERROR_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    else
        ensure_capacity_status = ERROR_INVALID_FUNCTION_PARAMETER;
    return ensure_capacity_status;
}


