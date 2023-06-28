#ifndef APP_WINDOWING_SYSTEM_H
#define APP_WINDOWING_SYSTEM_H

#include "StudyingVulkan.h"

#define MAX_LOADSTRING 100
extern WCHAR     szTitle[MAX_LOADSTRING];              // The title bar text
extern WCHAR     szWindowClass[MAX_LOADSTRING];        // the main window class name

namespace KeyStates
{
    extern bool     rightArrowKeyPressed;
    extern bool     downArrowKeyPressed;
    extern bool     leftArrowKeyPressed;
    extern bool     upArrowKeyPressed;
};

//  FUNCTION: RegisterWindowClass()
//
//  PURPOSE: Registers the window class.
//
ATOM RegisterWindowClass(HINSTANCE hInstance, LPCWSTR window_name);

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitWindowInstance(HINSTANCE hInstance, int nCmdShow, LPWSTR szWindowClass, LPWSTR szTitle, int32_t windowWidth, int32_t windowHeight);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - command: process the application menu
//  WM_PAINT    - command: Paint the main window
//  WM_DESTROY  - command: post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

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