/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * wimpwin.c: This file is part of ...
 *
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
 * */

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <string.h>
#include "simpwin.h"

HINSTANCE hInst;        // 当前实例句柄
HWND hWndMain;          // 主窗口句柄

LPCTSTR hello = TEXT("窗口外的世界很精彩，窗口内的天地也很奇妙");
LPCTSTR lpClassName = TEXT("SimpwinWClass");
LPCTSTR lpWindowName = TEXT("我的窗口");

/*
 * 调用初始化函数，处理消息循环
 */
int APIENTRY WinMain(HINSTANCE hInstance,       // 当前实例句柄
                     HINSTANCE hPrevInstance,   // 前一个实例句柄
                     LPSTR lpCmdLine,           // 命令行字符串
                     int nCmdShow)              // 窗口显示方式
{
    MSG msg;

    // 初始化窗口数据，并注册窗口类
    if (!InitApplication(hInstance))
        return FALSE;

    // 创建和显示窗口，对应用程序进行必要的初始化。
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;


    // 进入消息循环；从应用程序消息队列中检取消息。
    // 当检取的消息是一条 WM_QUIT 消息时，就退出消息循环
    while (GetMessage(&msg, NULL, 0, 0)) {
        // 把虚拟键消息翻译为字符消息
        TranslateMessage(&msg);
        // 把消息分配到相应的窗口过程中去
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

/*
 *初始化窗口数据，并注册窗口类
 */
BOOL InitApplication(HINSTANCE hInstance) //当前实例句柄
{
    WNDCLASS wcSimpwin;

    wcSimpwin.style = 0;
    wcSimpwin.lpfnWndProc = (WNDPROC) MainWndProc;
    wcSimpwin.cbClsExtra = 0;
    wcSimpwin.cbWndExtra = 0;
    wcSimpwin.hInstance = hInstance;
    wcSimpwin.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcSimpwin.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcSimpwin.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcSimpwin.lpszMenuName = NULL;
    wcSimpwin.lpszClassName = lpClassName;

    return RegisterClass(&wcSimpwin);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    hWndMain = CreateWindow(lpClassName,
                            lpWindowName,
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);

    /* 如果创建窗口失败，则返回FALSE */
    if (!hWndMain)
        return FALSE;

    /* 让窗口显示出来，并更新其客户区，最后返回TRUE */
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    return TRUE;
}


LRESULT CALLBACK MainWndProc(HWND hWnd,         // 窗口句柄
                             UINT Msg,          // 消息类型
                             WPARAM wParam,     // 消息附带信息
                             LPARAM lParam)     // 消息附带信息
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (Msg) {
      case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 20, 10, hello, lstrlen(hello));
        // All painting occurs here, between BeginPaint and EndPaint.
        //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
        EndPaint(hWnd, &ps);
        break;
      case WM_DESTROY:        // 消息：本窗口正将被销毁
        PostQuitMessage(0); // 请求退出窗口和应用程序
        break;
      default:
        // 调用默认窗口过程对未处理的消息进行必要的处理
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}
