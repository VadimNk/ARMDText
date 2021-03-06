#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
typedef struct _keyboard {
    HANDLE key_pressed_semaphore;
    BOOL program_terminate;
    BOOL keyboard_task_terminate;
    HANDLE keyboard_task_handle;
    int key;
}KEYBOARD;

void InitKeyboard(KEYBOARD* keyboard);
void ReleaseKeyboard(KEYBOARD* keyboard);
BOOL IsTerminated(KEYBOARD* keyboard);
int WaitKeyPressed(KEYBOARD* keyboard);
INPUT_RECORD PressEscapeKey();
INPUT_RECORD UnPressEscapeKey();
void SendEscapeToInput();

#endif