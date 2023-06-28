#ifndef APP_WINDOWING_SYSTEM_CPP
#define APP_WINDOWING_SYSTEM_CPP

#include "AppWindowingSystem.h"
#include <strsafe.h>
#include <wchar.h >
#include "resource.h"

WCHAR     szTitle[MAX_LOADSTRING];              // |EXTERN| The title bar text
WCHAR     szWindowClass[MAX_LOADSTRING];        // |EXTERN| the main window class name

bool     KeyStates::rightArrowKeyPressed = false; // |EXTERN|  is right arrow in a pressed state
bool     KeyStates::downArrowKeyPressed  = false; // |EXTERN|  is down arrow in a pressed state
bool     KeyStates::leftArrowKeyPressed  = false; // |EXTERN|  is left arrow in a pressed state
bool     KeyStates::upArrowKeyPressed    = false; // |EXTERN|  is up arrow in a pressed state


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    wchar_t msg[32];
    switch (message)
    {
        case WM_COMMAND:
        {
            printf ("WM_COMMAND\n");
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            //case IDM_ABOUT:
            //    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            //    break;
            //case IDM_EXIT:
            //    DestroyWindow(hWnd);
            //    break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

            break;
        }
        //case WM_PAINT:
        //{
        //    PAINTSTRUCT ps;
        //    HDC hdc = BeginPaint(hWnd, &ps);
        //    EndPaint(hWnd, &ps);
        //    return DefWindowProc (hWnd, message, wParam, lParam);
        //    break;
        //}
        case WM_LBUTTONDOWN: // left mouse button is down
        {
            printf ("WM_LBUTTONDOWN : %llu\n", wParam);
            return DefWindowProc (hWnd, message, wParam, lParam);
        }
        case WM_KEYDOWN:
        {
            //@reminder: The VK_LEFT (and other similarly named) key identifiers are not from vulkan. 
            //           The VK_*key* identifiers are from a microsoft header
            KeyStates::rightArrowKeyPressed = ((GetKeyState (VK_RIGHT) & 0x8000 ) != 0) ?   true : false;
            KeyStates::downArrowKeyPressed  = ((GetKeyState ( VK_DOWN) & 0x8000 ) != 0) ?   true : false;
            KeyStates::leftArrowKeyPressed  = ((GetKeyState ( VK_LEFT) & 0x8000 ) != 0) ?   true : false;
            KeyStates::upArrowKeyPressed    = ((GetKeyState (   VK_UP) & 0x8000 ) != 0) ?   true : false;

            return DefWindowProc (hWnd, message, wParam, lParam);
        }
        case WM_KEYUP:
        {
            KeyStates::rightArrowKeyPressed = ((GetKeyState (VK_RIGHT) & 0x8000) != 0) ? true : false;
            KeyStates::downArrowKeyPressed  = ((GetKeyState (VK_DOWN)  & 0x8000) != 0) ? true : false;
            KeyStates::leftArrowKeyPressed  = ((GetKeyState (VK_LEFT)  & 0x8000) != 0) ? true : false;
            KeyStates::upArrowKeyPressed    = ((GetKeyState (VK_UP)    & 0x8000) != 0) ? true : false;

            return DefWindowProc (hWnd, message, wParam, lParam);
        }
        case WM_SYSKEYDOWN:
        {
            printf ("WM_SySKEYDOWN\n");
            return DefWindowProc (hWnd, message, wParam, lParam);
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//
//  FUNCTION: RegisterWindowClass()
//
//  PURPOSE: Registers the window class.
//
ATOM RegisterWindowClass(HINSTANCE hInstance, LPCWSTR window_name)
{
    /*-- Documentation for WNDCLASSEXW::hbrBackground  -   https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexw
      || A handle to the class background brush. This member can be a handle to the brush to be used for painting the background, or it 
      ||  can be a color value. A color value must be one of the following standard system colors (the value 1 must be added to the chosen color).
      ||  If a color value is given, you must convert it to one of the following HBRUSH types:
      ...   [list of HBRUSH types]
      ||  The system automatically deletes class background brushes when the class is unregistered by using UnregisterClass. An application 
      ||     should not delete these brushes.
      ||  When this member is NULL, an application must paint its own background whenever it is requested to paint in its client area.
      ||  To determine whether the background must be painted, an application can either process the WM_ERASEBKGND message,
      ||  or test the fErase member of the PAINTSTRUCT structure filled by the BeginPaint function.
    */
    HBRUSH backgroundBrushHandle = (HBRUSH)(COLOR_WINDOW + 1);
    
    WNDCLASSEXW wcex =
    {
        /*..UINT......cbSize...........*/  sizeof(WNDCLASSEX),
        /*..UINT......style............*/  CS_HREDRAW | CS_VREDRAW,
        /*..WNDPROC...lpfnWndProc......*/  WndProc,
        /*..int.......cbClsExtra.......*/  0,
        /*..int.......cbWndExtra.......*/  0,
        /*..HINSTANCE.hInstance........*/  hInstance,
        /*..HICON.....hIcon............*/  0,   //LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSDESKTOPTEMPLATEPROJECT));
        /*..HCURSOR...hCursor..........*/ LoadCursor(nullptr, IDC_ARROW),
        /*..HBRUSH....hbrBackground....*/  backgroundBrushHandle,
        /*..LPCWSTR...lpszMenuName.....*/  0,   //MAKEINTRESOURCEW(IDC_WINDOWSDESKTOPTEMPLATEPROJECT);
        /*..LPCWSTR...lpszClassName....*/  window_name,
        /*..HICON.....hIconSm..........*/  0   //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    };

    return RegisterClassExW(&wcex);
}

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
HWND InitWindowInstance(HINSTANCE hInstance, int nCmdShow, LPWSTR szWindowClass, LPWSTR szTitle, int32_t windowWidth, int32_t windowHeight)
{

    LoadStringW (hInstance, IDS_APP_TITLE,            szTitle,        MAX_LOADSTRING);
    LoadStringW (hInstance, IDC_STUDYINGVULKANPROJECT, szWindowClass, MAX_LOADSTRING);

    RegisterWindowClass (hInstance, szWindowClass);
    PrintLastWin32Error (L"MyRegisterClass");


    // comments copied from https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindoww
    HWND window_handle = CreateWindowW(/*...[in, optional]..LPCWSTR.....lpClassName....*/ szWindowClass,//   <<this cant be 0. thats the issue. 
                                       /*...[in, optional]..LPCWSTR.....lpWindowName...*/ szTitle,
                                       /*...[in          ]..DWORD.......dwStyle........*/ WS_OVERLAPPEDWINDOW,
                                       /*...[in          ]..int.........x..............*/ CW_USEDEFAULT,  // The initial horizontal position of the window...If this parameter is set to CW_USEDEFAULT, the system selects the default position for the window's upper-left corner and ignores the y parameter. 
                                       /*...[in          ]..int.........y..............*/ CW_USEDEFAULT,
                                       /*...[in          ]..int.........nWidth.........*/ (windowWidth  != 0) ? windowWidth  : CW_USEDEFAULT, // The width, in device units, of the window. For overlapped windows, nWidth is either the window's width, in screen coordinates, or CW_USEDEFAULT. If nWidth is CW_USEDEFAULT, the system selects a default width and height for the window;
                                       /*...[in          ]..int.........nHeight........*/ (windowHeight != 0) ? windowHeight : CW_USEDEFAULT,
                                       /*...[in, optional]..HWND........hWndParent.....*/ nullptr,
                                       /*...[in, optional]..HMENU.......hMenu..........*/ nullptr,
                                       /*...[in, optional]..HINSTANCE...hInstance......*/ hInstance,
                                       /*...[in, optional]..LPVOID......lpPara.........*/ nullptr );

    PrintLastWin32Error(L"InitWindowInstance()");

    if (window_handle != 0)
    {
        // Seems to control what state the window is in, as in maximized, minimized, etc...).
        //       See https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow for list of commands.
        //       looks like here the command gets passed in via wWinMain i guess?
        ShowWindow(window_handle, nCmdShow);

        /* |||| Documentation for: UpdateWindow win32 function
           || The UpdateWindow function updates the client area of the specified window by sending a WM_PAINT message
           ||     to the window if the window's update region is not empty. The function sends a WM_PAINT message 
           ||     directly to the window procedure of the specified window, bypassing the application queue. If the 
           ||     update region is empty, no message is sent.  */
        UpdateWindow(window_handle);
    }
#ifdef DEBUG
    else
    {
        printf("window_handle = 0\n");
        assert (0);
    }
#endif

    return window_handle;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void PrintLastWin32Error(const wchar_t * lpszFunction)
{
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;

    DWORD  dw = GetLastError();

    if (dw != 0)
    {
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

        // Display the error message and exit the process
        LPVOID lpMsgBu      = 0;
        LPVOID lpDisplayBuf = 0;

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                          (lstrlen((LPCTSTR)lpMsgBuf)     +
                                           lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

        StringCchPrintf((LPTSTR)lpDisplayBuf,
                         LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                         TEXT("%s failed with error %d: %s"),
                         lpszFunction, dw, lpMsgBuf);

        printf("Error: %ls\n", lpDisplayBuf);
        std::cout << "Error:" << reinterpret_cast<char*>(lpDisplayBuf) << std::endl;
    }
    else
    {
        std::cout << "No error hit yet" << std::endl;
    }
}

#endif // APP_WINDOWING_SYSTEM_CPP