/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * window.c: This file is part of ...
 *
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
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
 * */

#include <windows.h>
#include <desktop.h>
#include "windesk.h"

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM
                          lParam);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    hInst = hInstance;
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    static TCHAR szAppName[] = TEXT ("HelloWin");

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground= (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName= szAppName;
    if (!RegisterClass (&wndclass)) {
        MessageBox ( NULL, TEXT ("This program requires Windows NT!"),
                    szAppName, MB_ICONERROR);
        return 0;
    }
    hwnd = CreateWindow( szAppName,                 // window class name
                        TEXT ("The Hello Program"), // window caption
                        WS_OVERLAPPEDWINDOW,        // window style
                        CW_USEDEFAULT,              // initial x position
                        CW_USEDEFAULT,              // initial y position
                        CW_USEDEFAULT,              // initial x size
                        CW_USEDEFAULT,              // initial y size
                        NULL,                       // parent window handle
                        NULL,                       // window menu handle
                        hInstance,                  // program instance handle
                        NULL);                      // creation parameters
    ShowWindow (hwnd, iCmdShow);
    UpdateWindow (hwnd);
    while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    switch (message) {
      case WM_CREATE:
        show_desktop();
        return 0;
      case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        GetClientRect (hwnd, &rect);
        DrawText (hdc, TEXT("Hello, Windows!"), -1, &rect,
                  DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint (hwnd, &ps);
        return 0;
      case WM_DESTROY:
        PostQuitMessage (0);
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}
