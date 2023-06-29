
/* Copyright 2023 Sean Halnon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**  http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissionsand
**  limitations under the License.
*/
#ifndef APP_WINDOWING_SYSTEM_H
#define APP_WINDOWING_SYSTEM_H

#include "StudyingVulkan.h"

#define MAX_LOADSTRING 100
extern WCHAR     szTitle[MAX_LOADSTRING];              // The title bar text
extern WCHAR     szWindowClass[MAX_LOADSTRING];        // the main window class name

// Some global variables for tracking the press states of keys we care about
namespace KeyStates
{
    extern bool rightArrowKeyPressed;
    extern bool downArrowKeyPressed;
    extern bool leftArrowKeyPressed;
    extern bool upArrowKeyPressed;
    extern bool escapeKeyPressed;
};

ATOM RegisterWindowClass(HINSTANCE hInstance, LPCWSTR window_name);

HWND InitWindowInstance(HINSTANCE hInstance, int nCmdShow, LPWSTR szWindowClass, LPWSTR szTitle, int32_t windowWidth, int32_t windowHeight);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Print the last win32 error
void PrintLastWin32Error(const wchar_t* lpszFunction);

// Ensures WndProc gets called for any window events such as button presses or window being moved
inline void CheckForAndDispatchWindowEventMessages ()
{
    MSG message = {};

    // PeekMessage doesnt block like GetMessage, and if the last arg is 0, than it will simply return the number of messages in the queue
    uint32_t numOutstandingMessages = PeekMessage (&message, NULL, 0, 0, 0); // should add a filter here so it only checks for keyboard events
    while (numOutstandingMessages > 0)
    {
        // Now that we know there ARE messages on the queue, we can call GetMessage() without worrying about it blocking while waiting for something to hit the queue
        GetMessage (&message, NULL, 0, 0);
        
        TranslateMessage (&message);
        DispatchMessage (&message);

        numOutstandingMessages = PeekMessage (&message, NULL, 0, 0, 0);
    }
}

// Brings up the console.
inline void ShowConsole()
{
	AllocConsole();

	FILE* fp = 0;
	freopen_s(&fp, "CONIN$",  "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
}

#endif // APP_WINDOWING_SYSTEM_H