#include <tchar.h>
#include "ARMDDisplayStrings.h"
#include "StringsRu.h"
#include "StringsEn.h"


TCHAR** armd_strings;

void InitStrings(int language_index)
{
    if (language_index == RUSSIAN_LANGUAGE)
        armd_strings = GetCommonStringsRu();
    else
        armd_strings = GetCommonStringsEn();
}

TCHAR* GetARMDString(int number)
{
    return armd_strings[number];
}

TCHAR* ByteToWide(DWORD max_tmp, TCHAR* tmp, char* str)
{
#ifdef _UNICODE
    MultiByteToWideChar(866, MB_PRECOMPOSED, str, -1, tmp, max_tmp);
    return tmp;
#else
    return str;
#endif
}
