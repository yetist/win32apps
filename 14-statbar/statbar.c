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

LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        case WM_CREATE:
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
                hWndStatusBar = CreateStatusWindow(WS_CHILD | WS VISIBLE, //窗口风格
                                                   NULL, //状态栏第一分区的初始文字
                                                   hWnd, //父窗口句柄
                                                   ID_STATUSBAR); //控件标识号
                //把状态栏分割成几个部分。
                SendMessage (hWndStatusBar, SB_SETPARTS, (WPARAM)NUM_PARTS, (LPARAM)pParts Width);
                //对状态栏中显示的信息进行初始化。
                //状态栏第一部分显示不带边框的文字。
                wsprintf(szBuf,"一切就绪！");
                SendMessage(hWndStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)(LPSTR)szBuf);
                //状态栏第二部分显示带凸形边框的位图。
                hBmpAlign = hBmpAlignLeft;
                SendMessage (hWndStatusBar, SB_SETTEXT, 1 | SBT_OWNERDRAW | SBT_POPOUT, (LPARAM) (HBITMAP)hBmpAlignLeft);
                //状态栏第三部分显示带凹形边框的文字。
                SendMessage (hWndStatusBar, SB_SETTEXT, 2, (LPARAM) (LPSTR) "字符格式，字体、尺寸、式样效果");
                //枚举多文本编辑控件中的可用字体。 hDC- GetDC(hWndRichEdit)
                pArrayOfTTFInfo = BuildFontList (hDC, &iNumOfFontFaces);
                ReleaseDC(hWndRichEdit, hDC);
                SetFocus (h WndRichEdit);
                CharFormat. cbSize - sizeof( CHARFORMAT);
                CharFormat.dwMask = CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE | CFM_STRIKEOUT | СFМ_FАСЕ;
                //初始化字符，使其不具有任何特殊的式样效果。
                CharFormat,dwEffects=0;
                // 把多文本编辑控件中的初始字符字体设置为字体数组中的第一种字体。
                lstrcpy(CharFormat, szFaceName, pArrayOfTTFInfo[0].plf->lfFaceName);
                SendMessage(hWndRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &CharFormat);
                CheckMenuRadioItem(GetMenu(hWndMain), IDM_ALIGNLEFT, IDM_ALIGNRIGHT, IDM_ALIGNLEFT, MF_BYCOMMAND);
            }
            break;
        case WM_INITMENUPOPUP:
            if((BOOL) HIWORD(lParam))
                break;
            if (SendMessage(hWndMainEdit, EM_CANUNDO, 0, 0)) {
                EnableMenuItem(GetMenu(hWndMain), IDM_EDITUNDO, MF_BYCOMMAND | MF_ENABLED);
            }
            else {
                EnableMenuItem(GetMenu(hWndMain), IDM_EDITUNDO, MF_BYCOMMAND | MF_GRAYED);
            }

            /* 检查是整流器可以进行粘贴操作，并由此设置菜单项的状态 */
            if(SendMessage(hWndRichEdit, EM_CANPASTE, 0, 0)) {
                EnableMenuItem(GetMenu(hWndMain), IDM_EDITPASTE, MF_BYCOMMAND | MF_ENABLED);
            } else {
                EnableMenuItem(GetMenu(hWndMain), IDM_EDITPASTE, MF_BYCOMMAND | MF_GRAYED);
            }
            break;
        case WM_SIZE:
            {
                // 调整多文本编辑手状态栏的位置和尺寸
                RECT rcStatusBar;

                if (bShowStatusBar)
                    GetWindowRect(hWndStatusBar, &rcStatusBar);
                nCyStatusBar = rcStatusBar.bottom - rcStatusBar.top;
                nCyClient = HIWORD(lParam);
                nCxClient = LOWORD(lParam);

                MoveWindow(hWndRichEdit, 0, 0, nCxClient, bShowStatusBar ? nCyClient - nCyStatusBar : nCyClient, TRUE);
                if (bShowStatusBar)
                    MoveWindow(hWndStatusBar, 0, nCyClient - nCyStatusBar, nCxClient, nCyClient, TRUE);
            }
            break;
        case WM_MENUSELECT:
            /*当用户在菜单中选择菜单项时，让状态栏的第一分区显示相关信息。*/
            if (LoadString(hInst, LOWORD(wParam), szBuf, 128)) {
                SendMessage(hWndStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)(LPSTR) szBuf);
            } else {
                SendMessage(hWndStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)(LPSTR) "愿事事顺心，时时如意");
            }
            break;

        case WM_DRAWITEM:
            if((int)wParam == ID_STATUSBAR) {
                /*在状态栏的第二分区上画上相应的位图，来反映段落对齐方式。*/
                LPDRAWITEMSTRUCT lpDis;
                HDC hdcMem;
                BITMAP bitmap;

                // 先保存自画子窗口用来绘制所需要的信息结构
                lpDis = (LPDRAWITEMSTRUCT) lParam;
                // 创建兼容型设备描述表
                hdcMem = CreateCompatibleDC(lpDis->hDC);
                // 把位图选入到该设备描述表中
                SelectObject(hdcMem, hBmpAlign);
                // 检取位图的尺寸信息
                GetObject(hBmpAlignLeft, sizeof(bitmap), &bitmap);
                // 把位图绘制到状态栏的第二部分中
                BitBlt (lpDis->hDC, lpDis->rcItem.left + 5, lpDis->rcItem.top + 1, bitmap.bmWidth, bitmap.bmHeight,
                        hdcMem,
                        0, 0,
                        SRCCOPY);
                // 删除兼容型设备描述表
                DeleteDC(hdcMem);
            }
            break;
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
                case EN_SELCHANGE:
                    /*一旦用户在多文本编辑控件中移动了插入符的位置或改变了文字选择，
              应用程序就会在这里接收和处理通知消息。 */

                    //在状态栏的第一分区中显示当前插入符所在的行与列的数值。
                    UpdateCaretPositionState (hWndRichEdit,hWndStatusBar);
                    //在状态栏的第二分区中显示当前插入符所在段落的对齐方式。
                    UpdateAlignState (hWnd,hWndRichEdit, hWndStatusBar);
                    //在状态栏的第三分区中显示当前插入符所在位置的字符格式。
                    UpdateCharFormatState (hWnd, hWndRichEdit,
                                           hWndStatusBar);
                    break;
                default:
                    return (DefWindowProc (hWnd, message, wParam, lParam));
            }
            break;
        case WM_COMMAND:
            uItem = LOWORD( wParam);
            switch (uItem)
            {
                case IDM_SHOWSTATUSBAR:
                    /*显示或隐藏状态栏，并相应地调整多文本编辑控件的尺寸。*/
                    bShowStatusBar = !bShowStatusBar;
                    CheckMenuItem (GetMenu (hWnd), IDM_SHOWSTATUSBAR,
                                   MF_BYCOMMAND | (bShowStatusBar ? MF_CHECKED : MF_UNCHECKED));
                    ShowWindow (hWndStatusBar, bShowStatusBar ? SW_SHOW : SW_HIDE);
                    MoveWindow (hWndRichEdit, 0,0, nCxClient, bShowStatusBar ? nCyClient - nCyStatusBar : nCyClient, TRUE);
                    if (bShowStatusBar)
                        MoveWindow(hWndStatusBar, 0, nCyClient - nCyStatusBar, nCxClient, nCyClient, TRUE);
                    break;

                case IDM_EDITUNDO:
                    SendMessage( hWndRichEdit, WM_UNDO, 0,0);
                    break;
                case IDM_EDITCUT:
                    SendMessage(hWndRichEdit, WM CUT, 0,0);
                    break;
                case IDM_EDITCOPY:
                    SendMessage( hWndRichEdit, WM COPY,0,0);
                    break;
                case IDM_EDITPASTE:
                    SendMessage( hWndRichEdit, WM PASTE, 0,0);
                    break;
                case IDM_EDITDELETE:
                    SendMessage(hWndRichEdit, WMCLEAR,0,0);
                    break;
                case IDM_EDITSELECTALL:
                    SendMessage( hWndRichEdit, EMSETSEL,0,-1);
                    break;
                case IDM_SELECTFONTFACE:
                    DialogBox (hInst, "SelectFontFaceDlgBox",hWnd,
                               (DLGPROC)SelectFontFaceDlgProc);
                    break;
                case IDM_SELECTFONTSIZE:
                    DialogBox (hInst, "SelectFontSizeDlgBox", hWnd,
                               (DLGPROC)SelectFontSizeDIgProOc);
                    break;
                case IDM_SELECTFONTSIZE:
                    DialogBox (hInst, "SelectFontSizeDlgBox", hWnd,
                               (DLGPROC)SelectFontSizeDIgProc);
                    break;
                case IDM_ALIGNLEFT:
                case IDM_ALIGNRIGHT:
                case IDM_ALIGNCENTER:
                    HandleAlignCommand (hWnd, hWndRichEdit, LOWORD( wParam ));
                    UpdateAlinState (hWnd, hWndRichEdit, hWndStatusBar);
                    break;
                case IDM_BOLD:
                case IDM_ITALIC:
                case IDM_UNDERLINE:
                case IDM_STRIKEOUT:
                    HandleCharEffectCommand (hWndRichEdit, uItem);
                    UpdateCharFormatState (hWnd,hWndRichEdit,
                                           hWndStatusBar);
                    break;
                case IDM_EXIT:
                    SendMessage (hWnd, WM CLOSE,0,0L);
                    return 0;
                case IDM_ABOUT:
                    DialogBox (hInst, "AboutBox", hWnd, (DLGPROC)About);
                    return 0;
                default:
                    return (DefWindowProc (hWnd, message, wParam, lParam));
            }
            break;
        case WM_DESTROY:
            if (hWndRichEdit)
                DestroyWindow (hWndRichEdit);
            if (hWndStatusBar)
                DestroyWindow (hWndStatusBar);
            if (hRTFLib)
            {
                FrееLіbrаrу (hRТFLіb);
                hRTFLib = NULL;
            }
            PostQuitMessage(0);
            break;
        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}

/**
* 函数:UpdateCaretPositionState(HWND,HWND)
* 用途:在状态栏上显示多文本编辑控件中插入符的位置。
*/

void UpdateCaretPositionState (HWND hWndRichEdit, //多文本编辑控件句柄
                               HWND hWndStatusBar) //状态栏控件句柄
{
    CHAR szPosInfo[256];
    int iRow, iCol;
    CHARRANGE CharRange;
    //找出插入符所在的位置(索引值).(如果有已选择的文字，那么就采用该文字选择的始点位置)。

    SendMessage (hWndRichEdit, EM EXGETSEL, 0, (LPARAM) &CharRange);
    //根据索引值计算插入符所在行与列的具体数值。
    iRow=(int)SendMessage (hWndRichEdit,
                           EM_EXLINEFROMCHAR,
                           0,
                           (LPARAM) (CharRange.cpMin));
    iCol=(int)(CharRange.cpMin - (int) SendMessage(hWndRichEdit,
                                                   EM_LINEINDEX,
                                                   iRow,
                                                   0));
    //对信息字符串进行格式化。
    wsprintf (szPosInfo,"行:%d,列:%d\t\t总索引位置:%d", iRow + 1, iCol +1,
              CharRange.cpMin);
    //把信息文字显示在状态栏的第一个部分中。
    SendMessage (hWndStatusBar, SB_SETTEXT, 0| SBT_NOBORDERS, (LPARAM)szPosInfo);
}

/*
函数:UpdateAlignState(HWND,HWND, HWND) 
用途:根据多文本编辑控件中插入符当前位置所在段落的对齐方式，来
改变相应的状态栏和菜单项的状态。 
*/
void UpdateAlignState (HWND hWnd, //主窗口句柄
                       HWND hWndRichEdit, //多文本编辑控件句柄
                       HWND hWndStatusBar) //状态栏控件句柄
{
    UINT uAlignCommand;
    PARAFORMAT ParaFormat;
    HMENU hMenu;
    //检取当前插入符所在段落的排列对齐格式。
    ParaFormat.cbSize - sizeof (ParaFormat);
    ParaFormat.dwMask = PFM_ALIGNMENT;
    SendMessage (hWndRichEdit, EM_GETPARAFORMAT, 0, (LPARAM)&ParaFormat);
    switch(ParaFormat. wAlignment)
    {
    case PFA_LEFT:
            uAlignCommand = IDM_ALIGNLEFT;
            hBmpAlign = hBmpAlignLeft;
            break;
    case PFA_CENTER:
            uAlignCommand- IDM_ALIGNCENTER;
            hBmpAlign= hBmpAlignCenter;
            break;
    case PFA_RIGHT:
            uAlignCommand = IDM ALIGNRIGHT;
            hBmpAlign = hBmpAlignRight;
            break;
    }
    //在状态栏中显示反映对齐方式的位图。
    SendMessage (hWndStatusBar, SB_SETTEXT,
                 1 | SBT_OWNERDRAW| SBT_POPOUT,
                 (LPARAM)hBmpAlign);

    //改设菜单项的状态。
    hMenu=GetMenu (hWnd );
    CheckMenuRadioltem (hMenu, IDM_ALIGNLEFT, IDM_ALIGNRIGHT,
                        uAlignCommand, MF_BYCOMMAND);
}

/*******
函数:HandleAlignCommand(HWND,HWND, int)
用途:处理段落排列对齐命令。
*/
VOID HandleAlignCommand(HWND hWnd, //主窗口句柄
                        HWND hWndRichEdit, //多文本窗口句柄
                        int iAlign) //对齐命令
{
    PARAFORMAT ParaFormat;
    //先填写段落格式结构PARAFORMAT中的字节尺寸和屏蔽码成员的值。
    ParaFormat.cbSize = sizeof (PARAFORMAT);
    ParaFormat.dwMask = PFM_ALIGNMENT;
    switch (Align)
    {
        case IDM_ALIGNLEFT:
            ParaFormat.wAlignment = PFA_LEFT;
            break;
        case IDM_ALIGNRIGHT:
            ParaFormat.wAlignment = PFA_RIGHT;
            break;
        case IDM_ALIGNCENTER:
            ParaFormat.wAlignment = PFA_CENTER;
            break;
    }
    //设置新的段落格式(对齐方式)。
    SendMessage (hWndRichEdit, EM_SETPARAFORMAT, 0, (LPARAM)&ParaFormat);
    //设置多文本编辑控件的修改标志。
    SendMessage (hWndRichEdit, EM_SETMODIFY, (WPARAM)TRUE, 0L),
}
