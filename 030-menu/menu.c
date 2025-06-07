/*
 * menu.c: This file is part of ...
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
#include "menu.h"

HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance,       // 当前实例句柄
                     HINSTANCE hPrevInstance,   // 前一个实例句柄
                     LPSTR lpCmdLine,           // 命令行字符串
                     int nCmdShow // 窗口显示方式
                    )
{
    MSG msg;
    HACCEL hAccel;

    // 初始化窗口数据，并注册窗口类
    if (!InitApplication(hInstance))
        return FALSE;

    // 创建和显示窗口，对应用程序进行必要的初始化。
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    // 装入快捷键资源
    hAccel = LoadAccelerators(hInstance, TEXT("MenuAppAccel"));

    // 进入消息循环；从应用程序消息队列中检取消息。
    // 当检取的消息是一条 WM_QUIT 消息时，就退出消息循环
    while (GetMessage(&msg, NULL, 0, 0)) {
        // 依据快捷键表，把键盘消息转译为菜单命令消息
        if (!TranslateAccelerator(hWndMain, hAccel, &msg))
            {
            // 把虚拟键消息翻译为字符消息
            TranslateMessage(&msg);
            // 把消息分配到相应的窗口过程中去
            DispatchMessage(&msg);
            }
    }

    return msg.wParam;
}

/*
 *初始化窗口数据，并注册窗口类
 */
BOOL InitApplication(HINSTANCE hInstance) //当前实例句柄
{
    WNDCLASSEX wcexMenuApp;

    wcexMenuApp.cbSize        = sizeof(WNDCLASSEX);
    wcexMenuApp.style         = 0;
    wcexMenuApp.lpfnWndProc   = (WNDPROC) MainWndProc;
    wcexMenuApp.cbClsExtra    = 0;
    wcexMenuApp.cbWndExtra    = 0;
    wcexMenuApp.hInstance     = hInstance;
    wcexMenuApp.hIcon         = LoadIcon(hInstance, TEXT("MenuAppIcon"));
    wcexMenuApp.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcexMenuApp.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcexMenuApp.lpszMenuName  = TEXT("MenuAppMenu");
    wcexMenuApp.lpszClassName = TEXT("MenuAppWClass");
    wcexMenuApp.hIconSm       = LoadIcon(hInstance, TEXT("SmallIcon"));

    return RegisterClassEx(&wcexMenuApp);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    hWnd = CreateWindow(TEXT("MenuAppWClass"),
                        TEXT("菜单示范程序"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    /* 如果创建窗口失败，则返回FALSE */
    if (!hWnd)
        return FALSE;

    hWndMain = hWnd;
    /* 让窗口显示出来，并更新其客户区，最后返回TRUE */
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd,         // 窗口句柄
                             UINT Message,          // 消息类型
                             WPARAM wParam,     // 消息附带信息
                             LPARAM lParam)     // 消息附带信息
{
    HDC hdc;
    PAINTSTRUCT ps;
    UINT idCommand;
    static BOOL bFontStyle[4];

    switch (Message) {
      case WM_CREATE:
        InitAlignMenu (hWnd);

        // 对菜单项的状态进行初始化
        CheckMenuRadioItem(GetMenu(hWnd),
                           IDM_ALIGNLEFT,
                           IDM_ALIGNRIGHT,
                           IDM_ALIGNLEFT,
                           MF_BYCOMMAND);
        CheckMenuRadioItem(GetMenu(hWnd),
                           IDM_SMALLFONTSIZE,
                           IDM_LARGEFONTSIZE,
                           IDM_SMALLFONTSIZE,
                           MF_BYCOMMAND);
        EnableMenuItem(GetMenu(hWnd),
                       IDM_EDITPASTE,
                       MF_BYCOMMAND | MF_UNCHECKED);

        bFontStyle[0] = bFontStyle[1] = bFontStyle[2] = bFontStyle[3] = FALSE;

        EnableMenuItem(GetMenu(hWnd),
                       IDM_BOLD,
                       MF_BYCOMMAND | MF_UNCHECKED);
        EnableMenuItem(GetMenu(hWnd),
                       IDM_ITALIC,
                       MF_BYCOMMAND | MF_UNCHECKED);
        EnableMenuItem(GetMenu(hWnd),
                       IDM_UNDERLINE,
                       MF_BYCOMMAND | MF_UNCHECKED);
        EnableMenuItem(GetMenu(hWnd),
                       IDM_STRIKEOUT,
                       MF_BYCOMMAND | MF_UNCHECKED);
        break;
      case WM_PAINT:
            {
            LPCTSTR szText = TEXT("请选择菜单项，或使用菜单快捷键");
            hdc = BeginPaint(hWnd, &ps);
            TextOut(hdc, 20, 20, szText, lstrlen(szText));
            EndPaint(hWnd, &ps);
            }
        break;
      case WM_SIZE:
        break;
      case WM_COMMAND:
        idCommand = LOWORD(wParam);

        switch(idCommand)
            {
          case IDM_BOLD:
          case IDM_ITALIC:
          case IDM_UNDERLINE:
          case IDM_STRIKEOUT:
            bFontStyle[idCommand - IDM_BOLD] = !bFontStyle[idCommand  - IDM_BOLD];
            CheckMenuItem(GetMenu(hWnd),
                          idCommand,
                          MF_BYCOMMAND | bFontStyle[idCommand - IDM_BOLD]? MF_CHECKED: MF_UNCHECKED);
            break;
          case IDM_SHOWTOOLBAR:
          case IDM_SHOWSTATUSBAR:
            if(GetMenuState(GetMenu(hWnd),
                            idCommand,
                            MF_BYCOMMAND) & MF_CHECKED)
                CheckMenuItem(GetMenu(hWnd), idCommand,
                              MF_BYCOMMAND | MF_UNCHECKED);
            else
                CheckMenuItem(GetMenu(hWnd), idCommand,
                              MF_BYCOMMAND | MF_CHECKED);
            break;
          case IDM_ALIGNLEFT:
          case IDM_ALIGNCENTER:
          case IDM_ALIGNRIGHT:
            CheckMenuRadioItem(GetMenu(hWnd),
                               IDM_ALIGNLEFT, IDM_ALIGNRIGHT,
                               idCommand, MF_BYCOMMAND);
            break;
          case IDM_SMALLFONTSIZE:
          case IDM_NORMALFONTSIZE:
          case IDM_LARGEFONTSIZE:
            CheckMenuRadioItem(GetMenu(hWnd),
                               IDM_SMALLFONTSIZE,
                               IDM_LARGEFONTSIZE,
                               idCommand, MF_BYCOMMAND);
            break;
          case IDM_EDITUNDO:
            EnableMenuItem(GetMenu(hWnd),
                           IDM_EDITUNDO,
                           MF_BYCOMMAND | MF_GRAYED);
            break;
          case IDM_EDITCOPY:
          case IDM_EDITCUT:
            EnableMenuItem(GetMenu(hWnd),
                           IDM_EDITPASTE,
                           MF_BYCOMMAND | MF_ENABLED);
            break;
          case IDM_EDITPASTE:
            EnableMenuItem(GetMenu(hWnd),
                           IDM_EDITUNDO,
                           MF_BYCOMMAND | MF_ENABLED);
            break;
          case IDM_EDITDELETE:
            EnableMenuItem(GetMenu(hWnd),
                           IDM_EDITPASTE,
                           MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(GetMenu(hWnd),
                           IDM_EDITUNDO,
                           MF_BYCOMMAND | MF_ENABLED);
            break;
          case IDM_EDITSELECTALL:
          case IDM_SELECTFONTFACE:
            MessageBox(NULL,
                       TEXT("在本示范程序中，该命令的功能暂缺。"),
                       TEXT("命令"),
                       MB_OK);
            break;
          case IDM_ABOUT:
            MessageBox(hWnd,
                       TEXT("这是菜单示范程序！\n\n作者"),
                       TEXT("关于"),
                       MB_OK | MB_ICONINFORMATION);
            break;
          case IDM_EXIT:
            PostQuitMessage(0); // 请求退出窗口和应用程序
            break;
          default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
            }
        break;
      case WM_DESTROY:        // 消息：本窗口正将被销毁
        PostQuitMessage(0); // 请求退出窗口和应用程序
        break;
      default:
        // 调用默认窗口过程对未处理的消息进行必要的处理
        return DefWindowProc(hWnd, Message, wParam, lParam);
    }

    return 0;
}


/* 在主菜单中插入格式下拉式菜单，并追加菜单项 */
VOID InitAlignMenu(HWND hWnd)
{
    HMENU hMenuMain;
    HMENU hMenuAlign;

    hMenuMain = GetMenu(hWnd);
    hMenuAlign = CreatePopupMenu ();

    InsertMenu (hMenuMain, (UINT)3, MF_BYPOSITION | MF_STRING | MF_POPUP,
                (UINT_PTR) hMenuAlign,
                (LPCTSTR) TEXT("格式(&M)"));

    AppendMenu (hMenuAlign, MF_STRING, IDM_ALIGNLEFT, (LPCTSTR) TEXT("左对齐"));
    AppendMenu (hMenuAlign, MF_STRING, IDM_ALIGNCENTER, (LPCTSTR) TEXT("居中对齐"));
    AppendMenu (hMenuAlign, MF_STRING, IDM_ALIGNRIGHT, (LPCTSTR) TEXT("右对齐"));

    return;
}
