/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * mypixel.c: This file is part of ...
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
#include "mypixel.h"

HINSTANCE hInst;
HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    if (!InitApplication(hInstance))
        return FALSE;

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = 0;
    wcex.lpfnWndProc   = (WNDPROC) MainWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = TEXT("MyPixelWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("Small"));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndMain = CreateWindow(TEXT("MyPixelWClass"),
                            TEXT("我的窗口"),
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);

    if (!hWndMain)
        return FALSE;

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    return TRUE;
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    HDC hDC;

    switch (message) {
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
        hDC = GetDC(hWnd);
        SetPixel (hDC, LOWORD(lParam), HIWORD(lParam),
                  (message == WM_LBUTTONDOWN) ? RGB(0, 0, 255) : RGB(255, 0, 0));
        ReleaseDC(hWnd, hDC);
        break;
      case WM_MOUSEMOVE:
        if(wParam & MK_LBUTTON || wParam & MK_RBUTTON)
            {
            hDC = GetDC(hWnd);
            SetPixel (hDC, LOWORD(lParam), HIWORD(lParam),
                      (wParam & MK_LBUTTON) ? RGB(0, 0, 0) : RGB(0, 255, 0));
            ReleaseDC(hWnd, hDC);
            }
        break;
      case WM_DESTROY:
        PostQuitMessage(0);
        break;
      default:
        // 调用默认窗口过程对未处理的消息进行必要的处理
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
