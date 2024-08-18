/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * statbar.c: This file is part of ...
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

//用途:说明状态栏的使用。
#include <windows.h>        // 嵌入包含基本Windows 函数的头文件
#include <commctrl.h>       // 嵌入公共控制件头文件
#include <richedit.h>       // 嵌入多文本控制件头文件
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "statbar.h"
#include "resource.h"

HINSTANCE hInst;
HWND hWndMain;
HWND hWndRichEdit;
HWND hWndStatusBar;

//多文本编辑控件所用动态链接库的句柄
HANDLE hRTFLib;

//位图资源句柄

HBITMAP hBmpAlignLeft, hBmpAlignCenter, hBmpAlignRight,
HBITMAP hBmpAlign;

//字体回调函数中用到的变量
PTRUETYPEFONTINFO pArrayOfTTFInfo;
HDC hdcGlobal;
int iFace;
int iNumOfFontFaces; // 可用字体的数目

//定义状态栏上的分区数
#define NUM_PARTS 3

//定义状态栏各分区的宽度

int pPartsWidth[NUM PARTS] = {1200,226,5001};
int APIENTRYWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow)
{
    MSG msg;
    InitCommonControls();
    hRTFLib= LoadLibrary ("RICHED32. DLL");
    if (!InitApplication (hInstance))
        return (FALSE);

    if (! InitInstance (hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage (&msg, NULL, 0,0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return (msg. wParam);
}

BOOL InitApplication (HINSTANCE hInstance)
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
    wcex.lpszMenuName  = TEXT("StatusBarMenu");
    wcex.lpszClassName = TEXT("StatusbarWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("Small"));

    return RegisterClassEx(&wcex);
}


BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;
    hWnd = Create Window(L"Statusbar WClass",
                        L"状态栏示范程序"，
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return FALSE;

    hWndMain = hWnd;
    ShowWindow (hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return (TRUE);
}

LRESULT APIENTRY Main WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM IParam)
{
    static TCHAR szBuf[128];
    static TCHAR szFontString[1024];
    HDC hDC;
    static int nCurSize;
    static BOOL bShowStatusBar = TRUE;
    static int nCyStatusBar;
    static int nCyClient, nCxClient;
    CHARFORMAT CharFormat;
    UINT uItem;

    switch (message) {
      case WM CREATE:
      {
      //装入位图资源。
      hBmpAlignLeft  = LoadBitmap (hInst, "AlignLeft");
      hBmpAlignCenter = LoadBitmap (hInst,"AlignCenter");
      hBmpAlignRight = LoadBitmap (hInst,"AlignRight");
      //创建多文本编辑控件。
      hWndRichEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
                                    "RICHEDIT",
                                    "",
                                    WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_SAVESEL |
                                    WS_HSCROLL | WS_VSCROLL,
                                    0,0,0,0,
                                    hWnd,
                                    (HMENU)ID_RICHEDIT,
                                    hInst,
                                    NULL);
      //设置多文本编辑控件的事件屏蔽码，使得多文本编辑控件一且
      //改变了当前选择，就会向其父窗口发送通知消息，以此来跟踪插入符的位置变化。
      /* SendMessage (h WndRichEdit, EM SETEVENTMASK,O, ENM SELCHANGE ); */

      //创建状态栏控件窗口。
      hWndStatusBar = CreateStatusWindow( WS_CHILD | WS VISIBLE, //窗口风格
                                         NULL, //状态栏第一分区的初始文字
                                         hWnd, //父窗口句柄
                                         ID_STATUSBAR); //控件标识号
      //把状态栏分割成几个部分。
      SendMessage (hWndStatusBar, SB_SETPARTS, (WPARAM)NUM_PARTS, (LPARAM)pParts Width);
      //对状态栏中显示的信息进行初始化。
      //状态栏第一部分显示不带边框的文字。
      wsprintf(szBuf,"一切就绪！");
      SendMessage(hWndStatusBar, SB_SETTEXT,OI SBT NOBORDERS, (LPARAM)(LPSTR)szBuf);
      //状态栏第二部分显示带凸形边框的位图。
      hBmpAlign- hBmpAlignLeft,
        SendMessage (hWndStatusBar, SB SETTEXT. 1 | SBT OWNERDRAW| SBT POPOUT, (LPARAM) (HBITMAP)hBmpAlignLeft),
        //状态栏第三部分显示带凹形边框的文字。
        SendMessage (hWndStatusBar, SB_SETTEXT, 2, (LPARAM) (LPSTR) "字符格式，字体、尺寸、式样效果"); 
      //枚举多文本编辑控件中的可用字体。 hDC- GetDC(hWndRichEdit) 
      pArrayOITTFInfo = BuildFontList (hDC, &iNumOfFontFaces), ReleaseDC(hWndRichEdit, hDC), 
                      SetFocus (h WndRichEdit), 
                      CharFormat. cbSize - sizeof( CHARFORMAT), CharFormat. dwMask-CFM ITALIC I CFM BOLD | CFM UNDERLINE | CFM STRIKEOUT I СЕМ ГАСЕ; 
      //初始化字符，使其不具有任何特殊的式样效果。 CharFormat,dwEffects=0, 
      日把多文本编辑控件中的初始字符字体设置为字体数组中的第一种字体。
