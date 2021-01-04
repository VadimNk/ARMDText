#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "Keyboard.h"
#include "ARMDDisplayStrings.h"

DWORD WINAPI KeyboardTask(void* argument);

#define MAX_KEYBOARD_SEMAPHORE_COUNT 1

void InitKeyboard(KEYBOARD* keyboard)
{
    if (keyboard)
    {
        keyboard->key_pressed_semaphore = CreateSemaphore(NULL, 0, MAX_KEYBOARD_SEMAPHORE_COUNT, _T("Keyboard semaphore."));
        keyboard->programm_terminate = FALSE;
        keyboard->keyboard_task_terminate = FALSE;
        keyboard->key = 0;
        keyboard->keyboard_task_handle = CreateThread(NULL, 64 * 1024, KeyboardTask, keyboard, CREATE_SUSPENDED, NULL);
        if (keyboard->keyboard_task_handle)
            ResumeThread(keyboard->keyboard_task_handle);
    }
}

INPUT_RECORD PressEscapeKey()
{
    INPUT_RECORD inputRecords;
    inputRecords.EventType = KEY_EVENT;
    inputRecords.Event.KeyEvent.bKeyDown = TRUE;
    inputRecords.Event.KeyEvent.dwControlKeyState = 0;
    inputRecords.Event.KeyEvent.uChar.UnicodeChar = 0x1B;
    inputRecords.Event.KeyEvent.wRepeatCount = 1;
    inputRecords.Event.KeyEvent.wVirtualKeyCode = VK_ESCAPE;
    inputRecords.Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_ESCAPE, MAPVK_VK_TO_VSC);
    return inputRecords;
}

INPUT_RECORD UnPressEscapeKey()
{
    INPUT_RECORD inputRecords;
    inputRecords.EventType = KEY_EVENT;
    inputRecords.Event.KeyEvent.bKeyDown = FALSE;
    inputRecords.Event.KeyEvent.dwControlKeyState = 0;
    inputRecords.Event.KeyEvent.uChar.UnicodeChar = 0x1B;
    inputRecords.Event.KeyEvent.wRepeatCount = 1;
    inputRecords.Event.KeyEvent.wVirtualKeyCode = VK_ESCAPE;
    inputRecords.Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_ESCAPE, MAPVK_VK_TO_VSC);
    return inputRecords;
}

void SendEscapeToInput()
{
#define MAX_INPUT_RECORDS 2
    INPUT_RECORD input_records[MAX_INPUT_RECORDS];
    DWORD nBytesWrote;

    input_records[0] = PressEscapeKey();
    input_records[1] = UnPressEscapeKey();
    if (!WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), input_records, MAX_INPUT_RECORDS, &nBytesWrote))
    {
        if (GetLastError() == ERROR_NO_DATA)
        {
            //	break; // Pipe was closed (normal exit path).
        }
        else
        {
            //	DisplayError("WriteConsoleInput");
        }
    }

}

void ReleaseKeyboard(KEYBOARD* keyboard)
{
    if (keyboard)
    {
        keyboard->keyboard_task_terminate = TRUE;
        do
        {
            SendEscapeToInput();
            Sleep(100);
        } while (WaitForSingleObject(keyboard->keyboard_task_handle, 300) != WAIT_OBJECT_0);
        keyboard->key = 0;
        CloseHandle(keyboard->key_pressed_semaphore);
        keyboard->key_pressed_semaphore = NULL;
        keyboard->keyboard_task_handle = NULL;
        keyboard->key = 0;
    }
}

void KeyPressed(KEYBOARD* keyboard, int key)
{
    if (keyboard)
    {
        keyboard->key = key;
        ReleaseSemaphore(keyboard->key_pressed_semaphore, 1, NULL);
    }
}

int WaitKeyPressed(KEYBOARD* keyboard)
{
    if (keyboard && keyboard->key_pressed_semaphore)
    {
        WaitForSingleObject(keyboard->key_pressed_semaphore, INFINITE);
        return keyboard->key;
    }
    return 0;
}

void TerminateProgramm(KEYBOARD* keyboard)
{
    if (keyboard)
        keyboard->programm_terminate = TRUE;
}

BOOL IsTerminated(KEYBOARD* keyboard)
{
    return keyboard ? keyboard->programm_terminate : FALSE;
}

VOID ErrorExit(TCHAR* lpszMessage)
{
    _ftprintf(stderr, _T("%s.\n"), lpszMessage);

    // Restore input mode on exit.

    ExitProcess(0);
}

DWORD WINAPI KeyboardTask(void* argument)
{
    int key = 0;
    DWORD cNumRead, fdwMode, i;
    INPUT_RECORD irInBuf[128];
    size_t mm = sizeof(INPUT_RECORD);
    int counter = 0;
    HANDLE hStdin;
    DWORD fdwSaveOldMode;
    // Get the standard input handle. 
    if (argument)
    {
        KEYBOARD* keyboard = (KEYBOARD*)argument;
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if (hStdin == INVALID_HANDLE_VALUE)
            ErrorExit(GetStdHandle(I_CANT_GET_STANDARD_CONSOLE_INPUT));

        // Save the current input mode, to be restored on exit. 

        if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
            ErrorExit(GetARMDString(I_CANT_GET_CONSOLE_MODE));

        // Enable the window and mouse input events. 

        fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
        if (!SetConsoleMode(hStdin, fdwMode))
            ErrorExit(GetARMDString(I_CANT_GET_CONSOLE_MODE));

        // Loop to read and handle the next 100 input events. 
        while (!keyboard->keyboard_task_terminate)
        {
            // Wait for the events. 
            WaitForSingleObject(hStdin, INFINITE);
            if (!ReadConsoleInput(
                hStdin,      // input buffer handle 
                irInBuf,     // buffer to read into 
                128,         // size of read buffer 
                &cNumRead)) // number of records read 
                ErrorExit(GetARMDString(I_CANT_READ_CONSOLE_INPUT));

            // Dispatch the events to the appropriate handler. 

            for (i = 0; i < cNumRead && !keyboard->keyboard_task_terminate; i++)
            {
                switch (irInBuf[i].EventType)
                {
                case KEY_EVENT: // keyboard input 
                    if (irInBuf[i].Event.KeyEvent.bKeyDown)
                    {
                        key = irInBuf[i].Event.KeyEvent.wVirtualKeyCode;
                        if (key == VK_CANCEL || key == VK_BACK || key >= VK_TAB)
                        {
                            if (key == VK_ESCAPE)
                                TerminateProgramm(keyboard);
                            else
                                if (key == VK_RETURN)
                                {
                                    int g = 23;
                                    g++;
                                }
                                else
                                    KeyPressed(keyboard, key);
                        }
                    }
                    break;
                }
            }
        }
        // Restore input mode on exit.
        SetConsoleMode(hStdin, fdwSaveOldMode);
    }
    return 0;
}
