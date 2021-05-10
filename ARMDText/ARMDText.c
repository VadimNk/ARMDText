// ARMDText.c
//
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!ДАННАЯ ПРОГРАММА РАЗРАБАТЫВАЛАСЬ ИСКЛЮЧИТЕЛЬНО В ЦЕЛЯХ ОТЛАДКИ АРМД.	!!
!!РАСПРОСТРАНЯЕТСЯ В КАЧЕСТВЕ ОЗНАКОМИТЕЛЬНОГО МАТЕРИАЛА.				!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ "КАК ЕСТЬ", В СВЯЗИ ЧЕМ РАЗРАБОТЧИК НЕ ПРЕДОСТАВЛЯЕТ ГАРАНТИИ ТОГО,	!!
!!ЧТО ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ БУДЕТ СООТВЕТСТВОВАТЬ ТРЕБОВАНИЯМ ПОЛЬЗОВАТЕЛЯ, А ТАКЖЕ ЧТО В ПРОЦЕССЕ РАБОТЫ		!!
!!ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ НЕ БУДУТ ВОЗНИКАТЬ ПРЕРЫВАНИЯ ИЛИ ОШИБКИ.											!!
!!																												!!
!!РАЗРАБОТЧИК НЕ НЕСЕТ ОТВЕТСТВЕННОСТИ ЗА КАКУЮ-ЛИБО ПОТЕРЮ ПРИБЫЛИ ИЛИ ИНОЙ УЩЕРБ, ПРОИЗОШЕДШИЙ В РЕЗУЛЬТАТЕ	!!
!!ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ.																		!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

//https://stackoverflow.com/questions/688760/how-to-create-a-utf-8-string-literal-in-visual-c-2008/2411769#2411769
//https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/
//https://ru.wikipedia.org/wiki/UTF-16

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif // _DEBUG


#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include <tchar.h>
#include <windows.h>
#include <locale.h>
#include <crtdbg.h>  
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <conio.h>
#include <stddef.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>

#include "ARMDText.h"
#include "ARMDParser.h"
#include "ARMDInfReader.h"
#include "ARMDFileReader.h"
#include "ARMDMessageParser.h"
#include "Keyboard.h"
#include "ProgramParameters.h"
#include "ARMDDisplayStrings.h"
#include "ARMDHeaderDisplay.h"

BOOL verbose = TRUE;

BOOL isVerobose()
{
	return verbose;
}

BOOL ReadDelay(clock_t delay)
{
	BOOL waked = FALSE;
	static BOOL first_measure = TRUE;
	static clock_t time_stamp = 0;
	clock_t timer_i;
	clock_t time_elapsed_from_start = clock();
	//считываем файл через определенные промежутки времени
	if (!first_measure)
	{
		timer_i = time_elapsed_from_start - time_stamp;
		if (timer_i > delay)
			time_stamp = time_elapsed_from_start;
		else
		{
			Sleep(delay - timer_i);
			waked = TRUE;
		}
	}
	else
	{
		time_stamp = time_elapsed_from_start;
		first_measure = FALSE;
	}
	return waked;
}

void ExitMain(_TCHAR* current_file_name, ProgramParameters* program_parameters)
{
	FreeParseProgramParameters(program_parameters);
	if (current_file_name)
		free(current_file_name);
}


BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
	INPUT ip;
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		while (1)
		{
			SendEscapeToInput();
			Sleep(10);
		}
		return TRUE;
	}
	return FALSE;
}

void ViewBlock(HANDLE console_output, KEYBOARD* keyboard, DWORD store_index, ARMDHeaderInfo* armd_header_info, ARMDProcessedData* armd_processed_data)
{
	if (!store_index)
		DisplayHeader(console_output, armd_header_info);
	for (DWORD i_number = store_index; i_number < armd_processed_data->number_items && !IsTerminated(keyboard); i_number++)
	{
		SetConsoleTextAttribute(console_output, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		if (!armd_processed_data->number_items)
			_tprintf(_T("--------------------------%s--------------------------------\n"), GetARMDString(I_INITIAL_VALUES));
		else
			if (armd_processed_data->number_items == 1)
				_tprintf(_T("------------------------------------------------------------------------\n"));
		ShowVal(console_output, *(armd_processed_data->data + i_number));
	}
}

void ARMDParseBlock(HANDLE console_output, KEYBOARD* keyboard, ARMDHeaderInfo** armd_header_info, ARMDProcessedData* armd_processed_data, ARMDParserData* armd_parser_data)
{
	int result = ERROR_OK;
	BOOL no_event_state = FALSE;
	int parse_armd_buffer_result = ERROR_COMMON;
	armd_parser_data->index = 0;
	if (!*armd_header_info) //заголовок ещё не считывался?
	{//считываем заголовок
		LoadHeader(armd_header_info, armd_parser_data);
		_tprintf(_T("%s.\n"), GetARMDString(I_HEADER_LOADER));
	}
	while (armd_parser_data->index < armd_parser_data->max_buf && !IsTerminated(keyboard))
	{
		parse_armd_buffer_result = GetARMDMessage(console_output, armd_header_info, armd_processed_data, armd_parser_data, &no_event_state);
		if (parse_armd_buffer_result < 0)
		{
			result = parse_armd_buffer_result;
			break;
		}
	}
	if (parse_armd_buffer_result >= 0)
	{
		armd_parser_data->parsed_file_len += armd_parser_data->index;//прибавляем к прочитанному объему данных из файла длину буфера
		//т.к. событие "нет события" затирается следующим событием причем это может быть повторное событие "нет события" 
		//с новым временем появления, которое возникает в системе, если не происходило никаких изменений в значениях мониторируемых переменных, 
		//то необходимо вернуться на позицию в файле до события "нет события". Период возникновения события "нет события"
		//(запись на диск накопленного буффера) устанавливается инструкцией RecordTime в файле MonCfg.ini.

		//if (no_event_state > 0)//Было событие "нет события" последним в буфере?
		//{//переместить "указатель" в файле в позицию до события "нет события"
//            armd_parser_data->parsed_file_len -= NO_EVENT_EVENT_LEN;
  //      }
	}
}

int SetNextFileSpecifiedFileMode(ARMDHeaderInfo* armd_header_info, ARMDParserData* armd_parser_data)
{
	if (armd_parser_data->parsed_file_len >= armd_header_info->file_size)
	{//дочитали файл до конца на момент последней записи УЧПУ
		_tprintf(_T("%s:%u, %s:%u"), GetARMDString(I_INDEX), armd_parser_data->index,
			GetARMDString(I_FILE_SIZE_HAVE_READ_FROM_HEADER), armd_header_info->file_size);
		FreeHeader(&armd_header_info);
		return -1;
	}
	return 0;
}

int NextFileMode(_TCHAR* current_file_name, _TCHAR* cnc_last_entry, ARMDParserData* armd_parser_data, ARMDHeaderInfo* armd_header_info,
	ARMDProcessedData* armd_processed_data)
{
	//файл не изменился и последней записью УЧПУ было событие отличное от события "нет события"
	//факт того, что длина файла не изменяется не может служить признаком того, что УЧПУ перестало посылать данные,
	//т.к. событие "нет события" обновляется не изменяя длину файла
	if (_tcsicmp(current_file_name, cnc_last_entry) != 0 &&
		armd_parser_data->parsed_file_len >= armd_header_info->file_size) //изменился файл, с которым в настоящий моммент работает программа УЧПУ?
	{//устанавливаем начальные значения для считывания файла
		_tprintf(_T("%s:%u, %s:%u"), GetARMDString(I_INDEX), armd_parser_data->index, GetARMDString(I_FILE_SIZE_HAVE_READ_FROM_HEADER), armd_header_info->file_size);
		_tcscpy_s(current_file_name, MAX_PATH, cnc_last_entry);
		ResetFileReader(armd_parser_data, armd_processed_data);
		FreeHeader(&armd_header_info);
	}
	return 0;
}

int View(HANDLE console_output, ProgramParameters* program_parameters, KEYBOARD* keyboard, _TCHAR* current_file_name)
{
	ARMDParserData armd_parser_data;
	ARMDProcessedData armd_processed_data;
	_TCHAR cnc_last_entry[MAX_ARMD_FILE_NAME + 1];
	int main_result = ERROR_OK;;
	ARMDHeaderInfo* armd_header_info = NULL;
	memset(&armd_processed_data, 0, sizeof(ARMDProcessedData));
	memset(&armd_parser_data, 0, sizeof(ARMDParserData));
	while (main_result >= ERROR_OK && !IsTerminated(keyboard))
	{
		BOOL waked = ReadDelay(program_parameters->delay_time_ms);
		if (waked)
			continue;
		if (program_parameters->specified_armd_file)
		{
			if (armd_header_info)
			{
				if (SetNextFileSpecifiedFileMode(armd_header_info, &armd_parser_data) < 0)
				{
					int key = WaitKeyPressed(keyboard);
					break;
				}
			}
			else
				_tcscpy_s(current_file_name, MAX_PATH, program_parameters->specified_armd_file);
		}
		else
		{
			BOOL read_inf_file_result = ReadInfFile(MAX_ARMD_FILE_NAME + 1, cnc_last_entry);
			if (!read_inf_file_result)
				continue;
			if (armd_header_info)
			{
				if (NextFileMode(current_file_name, cnc_last_entry, &armd_parser_data, armd_header_info, &armd_processed_data) < 0)
					break;
			}
			else
				_tcscpy_s(current_file_name, MAX_PATH, cnc_last_entry);
		}
		DWORD store_index = armd_processed_data.number_items;
		int read_armd_file_status = ReadARMDFile(MAX_PATH, current_file_name, &armd_parser_data);
		if (read_armd_file_status == ERROR_OK)
		{
			ARMDParseBlock(console_output, keyboard, &armd_header_info, &armd_processed_data, &armd_parser_data);
			ViewBlock(console_output, keyboard, store_index, armd_header_info, &armd_processed_data);
		}
		for (DWORD i_number = store_index; i_number < armd_processed_data.number_items; i_number++)
			FreeEventData(*(armd_processed_data.data + i_number));

	}
	if (armd_header_info)
		FreeHeader(&armd_header_info);
	FreeProcessedData(&armd_processed_data);
	FreeARMDParseData(&armd_parser_data);
	return main_result;
}

void SetDefaultLanguage()
{
	LANGID id = GetUserDefaultUILanguage();
	if ((id & 0x00FF) == 0x19)
		InitStrings(RUSSIAN_LANGUAGE);
	else
		InitStrings(ENGLISH_LANGUAGE);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int main_result = ERROR_OK;
	KEYBOARD keyboard;
	ProgramParameters program_parameters;
	HANDLE console_output;
	_TCHAR* current_file_name;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
#ifdef _UNICODE
	int set_mode_stdout_result = _setmode(_fileno(stdout), _O_U8TEXT);
	int set_mode_stdin_result = _setmode(_fileno(stdin), _O_U8TEXT);
	int set_mode_stderr_result = _setmode(_fileno(stderr), _O_U8TEXT);

	UINT oldcp = GetConsoleOutputCP();
	BOOL set_console_output_result = SetConsoleOutputCP(CP_UTF8);
	UINT ocp = GetConsoleCP();
	BOOL set_console = SetConsoleCP(CP_UTF8);
#else
	setlocale(LC_ALL, "1251");//translate source files saved as utf-8 to windows 1251 encoding

	int set_mode_stdout_result = _setmode(_fileno(stdout), _O_TEXT);
	int set_mode_stdin_result = _setmode(_fileno(stdin), _O_TEXT);
	int set_mode_stderr_result = _setmode(_fileno(stderr), _O_TEXT);

	UINT oldcp = GetConsoleOutputCP();
	BOOL set_console_output_result = SetConsoleOutputCP(1251);
	UINT ocp = GetConsoleCP();
	BOOL set_console = SetConsoleCP(1251);
#endif
	SetDefaultLanguage();
	if (set_mode_stdout_result >= 0 && set_mode_stdin_result >= 0 && set_mode_stderr_result >= 0 && set_console_output_result && set_console)
	{
		SetDefaultProgramParameters(&program_parameters);

		console_output = GetStdHandle(STD_OUTPUT_HANDLE);// Получаем хэндл консоли //с помощью него будем менять цвет строк
		if (console_output != INVALID_HANDLE_VALUE && console_output)
		{
			InitKeyboard(&keyboard);

			current_file_name = (_TCHAR*)malloc(MAX_PATH * sizeof(_TCHAR));
			if (current_file_name)
			{
				program_parameters = ParseProgramParameters(argc, argv);
				if (program_parameters.status >= ERROR_OK)
				{
					View(console_output, &program_parameters, &keyboard, current_file_name);
				}
			}
			SetConsoleTextAttribute(console_output, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
			ExitMain(current_file_name, &program_parameters);
			ReleaseKeyboard(&keyboard);
		}
		else
			_tprintf(_T("%s."), GetARMDString(I_CANT_GET_CONSOLE_HANDLE));
	}
	else
		_tprintf(_T("%s."), GetARMDString(I_CANT_SWITCH_CONSOLE_OUTPUT_OR_INPUT));

	SetConsoleOutputCP(oldcp);
	SetConsoleCP(ocp);

	if (set_mode_stdout_result >= 0)
		_setmode(_fileno(stdout), set_mode_stdout_result);
	if (set_mode_stdin_result >= 0)
		_setmode(_fileno(stdin), set_mode_stdin_result);
	if (set_mode_stderr_result >= 0)
		_setmode(_fileno(stderr), set_mode_stderr_result);
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	return main_result;
}