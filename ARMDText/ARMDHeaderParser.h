#ifndef _ARMD_HEADER_PARSER_H_
#define _ARMD_HEADER_PARSER_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "ARMD.h"
#include "ARMDFileReaderData.h"
//#include "ARMDFileReader.h"

#define IDENTIFIER_LEN			4			//длина заголовка
#define VERSION_LEN				4			//длина поля, в котором находится значение "версия структур файлов мониторинга"
#define HEADER_RESERVED_BYTES	12

//----параметры событий------------------------------------------------------------------------------------------------------------------------//
#define SYSTEM_EVENT_VAR	1			//системное	событие
#define TIME_EVENT_VAR		2			//тип учета:контроль событий "по интервалу времени" (через n-ое кол-во миллисекунд) из moncfg.ini
#define TICKCPU_EVENT_VAR	3			//тип учета:точное значение, контроль событий "каждый тик" (1 тик = n миллисекунд) из moncfg.ini
#define MEANVALUE_EVENT_VAR	4			//тип учета:среднее значение, "общая сумма событий за интервал времени" из moncfg.ini
#define APPROX_EVENT_VAR	5			//тип учета:приближенное значение, из moncfg.ini
//---------------------------------------------------------------------------------------------------------------------------------------------//

typedef struct _sys_armd_info {
    short			event_id;			//событие
    int				account_type;	//тип(вид) учета: как отслеживать события (находятся параметры событий, такие как TIMER_EVENT_VAR TICKCPU_EVENT_VAR)
    float			account_param;	//параметр типа учета (к примеру через сколько миллисекунд необходимо выполнять контроль для TIMER_EVENT_VAR)
                                    //к примеру если в файле конфигурации установлен мониторинг среднего значения подачи за 1с=1000 миллисекунд
                                    //(FEED,MEANVALUE,1000), то переменная event = 5(EVENT_FEED), 
                                    //account_type = 4(MEANVALUE_EVENT_VAR), account_param =1000 
    BYTE			units;			//единицы измерения
}SysARMDInfo;
//пояснение к APPROX_EVENT_VAR
//Параметром типа учета для приближенного значения является значение отклонения мониторируемой переменной
// от предыдущего ее значения, записанного в файле истории. 
//Далее, если значение мониторируемой переменной через некоторый промежуток времени не изменяется, то записывается ее точное значение в данный момент времени. 
//К примеру, в файле конфигурации заявлено FEED,APPROXIMATION, 10
//Тогда, если значение мониторируемой переменной подачи колеблется около 100(предыдущее записанное значение =100), и не превышает +-10,то значение в файл не записывается. 
//Как только ее значение становится 111 или 89, в файл записывается кратное значение. 
//Т.е будет взята целая часть от деления (111+10/2)/10 и умножена на 10, далее отклонение на +-10 будет учитываться от этого нового числа, 
//при этом, если после изменения это число остается неизменным какое-то время, то в файл записывается его точное значение. 
//такой тип мониторирования переменной предназначен для дискретизации постоянно изменяющихся переменных.

//У типа учета TICKCPU_EVENT_VAR нет параметра

typedef struct _proc_info {
    BYTE proc;					//процесс УЧПУ
    short num_events;			//кол-во событий в процессе УЧПУ
    SysARMDInfo* event_info;
}ProcInfo;

typedef struct _armd_header_info {
    char identifier[IDENTIFIER_LEN + 1];		//идентификатор файла
    char ver[VERSION_LEN + 1];					//версия структур, используемых для записи файла
    DWORD file_size;					//длина файла мониторинга
    char prev_file_name[MAX_ARMD_FILE_NAME + 1];	//имя предыдущего файла
    char next_file_name[MAX_ARMD_FILE_NAME + 1];	//имя следующего файла
    BYTE withdraw;
    BYTE software_version_len;					//длина строки 
    char* software_version;						//версия программного обеспечения установленного на УЧПУ
    char machine_name[MAX_ARMD_FILE_NAME + 1];	//название станка
    long record_time;							//период записи буффера в файл (или переиод возникновния события "нет события")
    SYSTEMTIME SysTime;							//структура даты и времени
    BYTE local;
    char reserved[12];
    BYTE num_proc;								//количество всех процессов подвергающихся мониторингу
    short system_tick;							//тик системы
    ProcInfo* proc_info;						//параметры АРМД для каждого из процессов, заданных в файле конфигурации АРМД
}ARMDHeaderInfo;

int LoadHeader(ARMDHeaderInfo** const armd_header_info_p, ARMDFileReaderData* armd_file_reader_data);
void FreeHeader(ARMDHeaderInfo** armd_header_info);

#endif// !_ARMD_HEADER_PARSER_H_
