/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * Copyright (c) 2024 yetist <yetist@x395>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **/

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg) {
      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

      case WM_PAINT:
            {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hWnd, &ps);
            }
        return 0;
    }

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
    HWND hWnd;
    MSG msg = {0};
    WNDCLASS wndclass = {0};

    LPCTSTR lpClassName  = TEXT("HelloApp");
    LPCTSTR lpWindowName = TEXT("Learn to Program Windows中文标题");

    // Register the window class.
    wndclass.lpfnWndProc   = WindowProc;
    wndclass.hInstance     = hInstance;
    wndclass.lpszClassName = lpClassName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), lpClassName, MB_ICONERROR);
        return 0;
    }

    // Create the window.
    hWnd = CreateWindowEx(0,                              // Optional window styles.
                          lpClassName,                    // Window class
                          lpWindowName,                   // Window text
                          WS_OVERLAPPEDWINDOW,            // Window style
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Size and position
                          NULL,       // Parent window
                          NULL,       // Menu
                          hInstance,  // Instance handle
                          NULL        // Additional application data
                         );

    if (hWnd == NULL){
        return 0;
    }

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    // Run the message loop.
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
