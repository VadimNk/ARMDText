#ifndef _ARMD_PROCESS_DATA_H_
#define _ARMD_PROCESS_DATA_H_
#include <windows.h>
#include "ARMDMessage.h"
// в структуре хранится уже обработанные данные

typedef struct _armd_processed_data {
    DWORD max_items;
    DWORD number_items;
    ARMDMessageData** data;
}ARMDProcessedData;

int EnsureCapacityOfProcessedData(ARMDProcessedData* armd_processed_data);
void FreeProcessedData(ARMDProcessedData* armd_processed_data);

#endif // !_ARMD_PROCESS_DATA_H_
