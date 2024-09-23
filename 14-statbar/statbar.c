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
//#include "resource.h"

HINSTANCE hInst;
HWND hWndMain;
HWND hWndRichEdit;
HWND hWndStatusBar;

//多文本编辑控件所用动态链接库的句柄
HANDLE hRTFLib;

//位图资源句柄

HBITMAP hBmpAlignLeft, hBmpAlignCenter, hBmpAlignRight;
HBITMAP hBmpAlign;

//字体回调函数中用到的变量
PTRUETYPEFONTINFO pArrayOfTTFInfo;
HDC hdcGlobal;
int iFace;
int iNumOfFontFaces; // 可用字体的数目

//定义状态栏上的分区数
#define NUM_PARTS 3
#define NUM_POINTS 16

//定义状态栏各分区的宽度

int pPartsWidth[NUM_PARTS] = {1200,226,5001};
int APIENTRY WinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow)
{
    MSG msg;
    InitCommonControls();
    hRTFLib= LoadLibrary (L"RICHED32.DLL");
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
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("about"));

    return RegisterClassEx(&wcex);
}


BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;
    hWnd = CreateWindow(L"Statusbar WClass",
                         L"状态栏示范程序",
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
    HDC hDC;
    static BOOL bShowStatusBar = TRUE;
    static int nCyStatusBar;
    static int nCyClient, nCxClient;
    CHARFORMAT CharFormat;
    UINT uItem;

    switch (message) {
        case WM_CREATE:
            {
            hDC = GetDC(hWnd);
                //装入位图资源。
                hBmpAlignLeft  = LoadBitmap (hInst, L"AlignLeft");
                hBmpAlignCenter = LoadBitmap (hInst, L"AlignCenter");
                hBmpAlignRight = LoadBitmap (hInst, L"AlignRight");
                //创建多文本编辑控件。
                hWndRichEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
                                              L"RICHEDIT",
                                              L"",
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
                hWndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, //窗口风格
                                                   NULL, //状态栏第一分区的初始文字
                                                   hWnd, //父窗口句柄
                                                   ID_STATUSBAR); //控件标识号
                //把状态栏分割成几个部分。
                SendMessage (hWndStatusBar, SB_SETPARTS, (WPARAM)NUM_PARTS, (LPARAM)pPartsWidth);
                //对状态栏中显示的信息进行初始化。
                //状态栏第一部分显示不带边框的文字。
                wsprintf(szBuf, L"一切就绪！");
                SendMessage(hWndStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)(LPSTR)szBuf);
                //状态栏第二部分显示带凸形边框的位图。
                hBmpAlign = hBmpAlignLeft;
                SendMessage (hWndStatusBar, SB_SETTEXT, 1 | SBT_OWNERDRAW | SBT_POPOUT, (LPARAM) (HBITMAP)hBmpAlignLeft);
                //状态栏第三部分显示带凹形边框的文字。
                SendMessage (hWndStatusBar, SB_SETTEXT, 2, (LPARAM) (LPSTR) "字符格式，字体、尺寸、式样效果");
                //枚举多文本编辑控件中的可用字体。 hDC- GetDC(hWndRichEdit)
                pArrayOfTTFInfo = BuildFontList (hDC, &iNumOfFontFaces);
                ReleaseDC(hWndRichEdit, hDC);
                SetFocus (hWndRichEdit);
                CharFormat.cbSize = sizeof( CHARFORMAT);
                CharFormat.dwMask = CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_FACE;
                //初始化字符，使其不具有任何特殊的式样效果。
                CharFormat.dwEffects=0;
                // 把多文本编辑控件中的初始字符字体设置为字体数组中的第一种字体。
                lstrcpy(CharFormat.szFaceName, pArrayOfTTFInfo[0].plf->lfFaceName);
                SendMessage(hWndRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &CharFormat);
                CheckMenuRadioItem(GetMenu(hWndMain), IDM_ALIGNLEFT, IDM_ALIGNRIGHT, IDM_ALIGNLEFT, MF_BYCOMMAND);
            }
            break;
        case WM_INITMENUPOPUP:
            if((BOOL) HIWORD(lParam))
                break;
            if (SendMessage(hWndRichEdit, EM_CANUNDO, 0, 0)) {
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
                    SendMessage(hWndRichEdit, WM_CUT, 0,0);
                    break;
                case IDM_EDITCOPY:
                    SendMessage( hWndRichEdit, WM_COPY,0,0);
                    break;
                case IDM_EDITPASTE:
                    SendMessage( hWndRichEdit, WM_PASTE, 0,0);
                    break;
                case IDM_EDITDELETE:
                    SendMessage(hWndRichEdit, WM_CLEAR,0,0);
                    break;
                case IDM_EDITSELECTALL:
                    SendMessage( hWndRichEdit, EM_SETSEL,0,-1);
                    break;
                case IDM_SELECTFONTFACE:
                    DialogBox (hInst, L"SelectFontFaceDlgBox",hWnd,
                               (DLGPROC)SelectFontFaceDlgProc);
                    break;
                case IDM_SELECTFONTSIZE:
                    DialogBox (hInst, L"SelectFontSizeDlgBox", hWnd,
                               (DLGPROC)SelectFontSizeDlgProc);
                    break;
                case IDM_ALIGNLEFT:
                case IDM_ALIGNRIGHT:
                case IDM_ALIGNCENTER:
                    HandleAlignCommand (hWnd, hWndRichEdit, LOWORD( wParam ));
                    UpdateAlignState (hWnd, hWndRichEdit, hWndStatusBar);
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
                    SendMessage (hWnd, WM_CLOSE,0,0L);
                    return 0;
                case IDM_ABOUT:
                    DialogBox (hInst, L"AboutBox", hWnd, (DLGPROC)About);
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
                FreeLibrary(hRTFLib);
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
    TCHAR szPosInfo[256];
    int iRow, iCol;
    CHARRANGE CharRange;
    //找出插入符所在的位置(索引值).(如果有已选择的文字，那么就采用该文字选择的始点位置)。

    SendMessage (hWndRichEdit, EM_EXGETSEL, 0, (LPARAM) &CharRange);
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
    wsprintf (szPosInfo, L"行:%d,列:%d\t\t总索引位置:%d", iRow + 1, iCol +1,
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
    ParaFormat.cbSize = sizeof (ParaFormat);
    ParaFormat.dwMask = PFM_ALIGNMENT;
    SendMessage (hWndRichEdit, EM_GETPARAFORMAT, 0, (LPARAM)&ParaFormat);
    switch(ParaFormat. wAlignment)
    {
    case PFA_LEFT:
            uAlignCommand = IDM_ALIGNLEFT;
            hBmpAlign = hBmpAlignLeft;
            break;
    case PFA_CENTER:
            uAlignCommand = IDM_ALIGNCENTER;
            hBmpAlign= hBmpAlignCenter;
            break;
    case PFA_RIGHT:
            uAlignCommand = IDM_ALIGNRIGHT;
            hBmpAlign = hBmpAlignRight;
            break;
    }
    //在状态栏中显示反映对齐方式的位图。
    SendMessage (hWndStatusBar, SB_SETTEXT,
                 1 | SBT_OWNERDRAW| SBT_POPOUT,
                 (LPARAM)hBmpAlign);

    //改设菜单项的状态。
    hMenu=GetMenu (hWnd );
    CheckMenuRadioItem (hMenu, IDM_ALIGNLEFT, IDM_ALIGNRIGHT,
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
    switch (iAlign)
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
    SendMessage (hWndRichEdit, EM_SETMODIFY, (WPARAM)TRUE, 0L);
}

/*
函数:UpdateCharFormatState (HwND,HwND, HWND)
用途:根据多文本编辑控件中插入符当前位置所在字符的格式(字体、
字符尺寸、式样效果)，改变相应的状态栏和菜单项的状态。
*/

void UpdateCharFormatState (HWND hWnd, HWND hWndRichEdit, HWND hWndStatusBar)
{
    BOOL bBold, bItalic, bUnderLine, bStrikeOut;
    CHARFORMAT CharFormat;
    TCHAR szCharFormat[255];
    HMENU hMenu;

    // 检取多文本控件的当前字符格式。
    CharFormat.cbSize = sizeof(CHARFORMAT);
    CharFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE
      | CFM_STRIKEOUT | CFM_FACE | CFM_SIZE;
    SendMessage (hWndRichEdit, EM_GETCHARFORMAT, TRUE, (LPARAM)&CharFormat);

    //判断各字符式样效果是否被采用。
    bBold       = (CharFormat.dwEffects & CFE_BOLD )? TRUE: FALSE;
    bItalic     = (CharFormat.dwEffects & CFE_ITALIC)? TRUE:FALSE;
    bUnderLine  = (CharFormat.dwEffects & CFE_UNDERLINE)? TRUE: FALSE;
    bStrikeOut  = (CharFormat.dwEffects & CFE_STRIKEOUT)? TRUE: FALSE;

    //改设相关菜单项的状态。
    hMenu = GetMenu (hWnd);
    CheckMenuItem (hMenu, IDM_BOLD, MF_BYCOMMAND| bBold ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem (hMenu, IDM_ITALIC, MF_BYCOMMAND | bItalic ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem (hMenu, IDM_UNDERLINE, MF_BYCOMMAND | bUnderLine ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem (hMenu, IDM_STRIKEOUT, MF_BYCOMMAND | bStrikeOut ? MF_CHECKED : MF_UNCHECKED);

    //在状态栏上显示当前的字符格式信息。
    wsprintf (szCharFormat, L"[%s %1]",
              CharFormat.szFaceName, //字体名称
              CharFormat.yHeight); //字符高度
    if (bBold)
        lstrcat(szCharFormat, L"粗体");
    if (bItalic)
        lstrcat(szCharFormat, L"斜体");
    if (bUnderLine)
        lstrcat(szCharFormat, L"加下划线");
    if (bStrikeOut)
        lstrcat(szCharFormat, L"加删除线");
    SendMessage(hWndStatusBar, SB_SETTEXT,2, (LPARAM)(LPSTR)szCharFormat);
}


/****
* 函数:HandleCharEffectCommand (HWND,HWND,int)
* 用途:处理字符式样效果命令。
*/
void HandleCharEffectCommand(HWND hWndRichEdit,
                             UINT uCommand)
{
    CHARFORMAT CharFormat;
    //多文本编辑控件句柄/字符式样效果命令
    CharFormat.cbSize = sizeof (CHARFORMAT);
    CharFormat.dwMask = CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE | CFM_STRIKEOUT;
    //检取多文本编辑控件的原有的字符格式(是否斜体、粗体、加下划线、加删除线)。
    SendMessage (hWndRichEdit, EM_GETCHARFORMAT, TRUE,(LPARAM) &CharFormat);
    switch (uCommand)
        {
      case IDM_BOLD:
        CharFormat.dwMask = CFM_BOLD;
          //切换字符的粗体效果。
          CharFormat.dwEffects ^= CFE_BOLD;
          break;
      case IDM_ITALIC:
          CharFormat.dwMask = CFM_ITALIC;
          //切换字符的斜体效果。
          CharFormat.dwEffects ^= CFE_ITALIC;
          break;
      case IDM_UNDERLINE:
          CharFormat.dwMask = CFM_UNDERLINE;
          // 切换字符的加下划线效果。
          CharFormat.dwEffects ^= CFE_UNDERLINE;
          break;
      case IDM_STRIKEOUT:
          CharFormat.dwMask = CFM_STRIKEOUT;
          //切换字符的加删除线效果。
          CharFormat.dwEffects ^= CFE_STRIKEOUT;
          break;
        }
    SendMessage (hWndRichEdit, EM_SETCHARFORMAT, SCF_SELECTION,
                 (LPARAM)&CharFormat);
}

BOOL APIENTRY SelectFontFaceDlgProc(HWND hDlg, UINT message, WPARAM wParam,LPARAM lParam)
{
    int index;
    CHARFORMAT CharFormat;
    switch (message)
        {
      case WM_INITDIALOG:

        //把字体名称插入到列表框中。 
        for (index=0; index<iNumOfFontFaces; index++) 
            SendMessage (GetDlgItem(hDlg, IDC_FONTFACE), CB_ADDSTRING, 0,
                         (LPARAM) (LPCSTR) pArrayOfTTFInfo[index].plf->lfFaceName); 
        //检取多文本编辑控件当前插入符所在位置的字体。
        CharFormat.cbSize = sizeof(CHARFORMAT);
        CharFormat.dwMask = CFM_FACE;
        SendMessage(hWndRichEdit, EM_GETCHARFORMAT, TRUE, (LPARAM)&CharFormat);

        // 初始化列表框中的选择项。 
        SendMessage (GetDlgItem (hDlg, IDC_FONTFACE), WM_SETTEXT, 
                     0,(LPARAM) (LPCSTR) CharFormat.szFaceName);
        SetFocus (GetDlgItem (hDlg, IDC_FONTFACE)); 
        return (TRUE); 
      case WM_COMMAND:
        switch (LOWORD(wParam))
            {
          case IDC_FONTFACE:
            if( HIWORD(wParam) != CBN_DBLCLK)
                break;
          case IDOK:
            //检取用户在列表框中选择的新字体。
            SendDlgItemMessage(hDlg, IDC_FONTFACE, WM_GETTEXT, sizeof (CharFormat.szFaceName),
                               (LPARAM)CharFormat.szFaceName);
            //设置新字体。
            CharFormat.cbSize=sizeof( CHARFORMAT );
            CharFormat.dwMask= CFM_FACE;
            SendMessage (hWndRichEdit, EM_SETCHARFORMAT, 
                         SCF_SELECTION, (LPARAM)&CharFormat); 
            EndDialog (hDlg, TRUE);
            return (TRUE); 
          case IDCANCEL:
            EndDialog (hDlg, FALSE);
            return (TRUE);
          default: 
            return (FALSE);
            }
        }
    return (FALSE);
}

BOOL APIENTRY SelectFontSizeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM IParam) 
{
    //被支持的字体尺寸数组
    static int aPoints[NUM_POINTS] = {8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,72}; 
    TCHAR szFontSize[10];
    CHARFORMAT CharFormat;
    int index;
    switch (message) 
      {
      case WM_INITDIALOG: 
        //把字体尺寸点数字符串填充到组合框中。 
        for (index=0; index <NUM_POINTS; index ++) 
            wsprintf (szFontSize, L"%d", aPoints[index]); 
        SendMessage (GetDlgItem (hDlg, IDC_FONTSIZE), CB_ADDSTRING, 
                     0, (LPARAM) (LPCSTR) szFontSize); 

        //检取多文本控件的当前字符尺寸。 
        CharFormat.cbSize = sizeof( CHARFORMAT); 
        CharFormat.dwMask = CFM_SIZE;
        SendMessage (hWndRichEdit, EM_GETCHARFORMAT, TRUE, (LPARAM)&CharFormat); 

        //初始选择选项。 
        wsprintf (szFontSize, L"%d", CharFormat.yHeight/20);
        SendDlgItemMessage(hDlg, IDC_FONTSIZE,
                           CB_SELECTSTRING,
                           (WPARAM)-1, (LPARAM) (LPCSTR) szFontSize);
        SetFocus (GetDlgItem (hDlg, IDC_FONTSIZE));
        return (TRUE);
      case WM_COMMAND: 
        switch( LOWORD(wParam)) 
            {
          case IDC_FONTSIZE: 
            if ( HIWORD(wParam)!= CBN_DBLCLK)
                break;
          case IDOK:
            //检取用户在列表框中选择的字符尺寸点数。 
            SendDlgItemMessage (hDlg, IDC_FONTSIZE, 
                                WM_GETTEXT, 
                                sizeof (szFontSize), 
                                (LPARAM)(LPCSTR) szFontSize); 
            CharFormat.cbSize = sizeof (CHARFORMAT);
            CharFormat.dwMask = CFM_SIZE;
            //把字符尺寸点数乘以20，转换成字符高度数值。 
            CharFormat.yHeight = _wtoi (szFontSize)* 20;
            //设置新的字符尺寸。 
            SendMessage (hWndRichEdit, EM_SETCHARFORMAT, SCF_SELECTION,
                         (LPARAM)&CharFormat);
            EndDialog (hDlg, TRUE);
            return (TRUE);
          case IDCANCEL:
            EndDialog (hDlg, FALSE);
            return (TRUE);
          default: 
            return (FALSE);
            }
      }
    return (FALSE);
}

BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM IParam) 
{
    switch (message) 
        {
      case WM_INITDIALOG:
        return (TRUE);
      case WM_COMMAND:
        if (LOWORD(wParam)==IDOK || LOWORD(wParam)==IDCANCEL) 
            {
            EndDialog (hDlg, TRUE); 
            return (TRUE); 
            }
        break; 
        }
    return (FALSE); 
}

PTRUETYPEFONTINFO BuildFontList (HDC hDCFont, //设备描述表句柄
                                 LPINT retnFaces) //返回字体数目的变量的指针
{
    iNumOfFontFaces = 0; //全局字体计数变量初始化为o 
    hdcGlobal = hDCFont; //保存好字体数组对应的设备描述表save， 
                         //以备回调函数中使用。 
                         //首先算出系统中可用True Type字体的数目。 
    EnumFontFamilies (hdcGlobal, NULL, (FONTENUMPROC)MyEnumFontCountProc, 
                      (LPARAM)&iNumOfFontFaces); 
    // 分配字体信息结构的数组。 
    pArrayOfTTFInfo = (PTRUETYPEFONTINFO)LocalAlloc (LPTR, 
                                                     sizeof (TRUETYPEFONTINFO)*(iNumOfFontFaces +1)); 
    /*再次枚举所有可用的True Type字体。对于每个字体，都填写好各自的逻辑字体* 
     *信息结构(LOGFONT)和物理字体信息结构(TEXTMETRIC)。 
     */ 
    iFace = 0; 
    EnumFontFamilies(hdcGlobal, NULL,(FONTENUMPROC) MyEnumFontFacesProc, 
                     (LPARAM)NULL);


    *retnFaces = iNumOfFontFaces;
    return pArrayOfTTFInfo;
}

int APIENTRY MyEnumFontFacesProc( 
                                 LPLOGFONT lpLogFont, 
                                 LPTEXTMETRIC lpTEXTMETRICs, 
                                 DWORD fFontType, 
                                 LPVOID IpData) 
{
    if (fFontType &TRUETYPE_FONTTYPE) /*确定该字体属于True Type型字体。*/
        {
        pArrayOfTTFInfo[iFace].plf = (LPLOGFONT)LocalAlloc (LPTR, 
                                                          sizeof (LOGFONT)); //*nFonts);
        pArrayOfTTFInfo[iFace].ptm = (LPTEXTMETRIC)LocalAlloc (LPTR,
                                                               sizeof (TEXTMETRIC)); //* nFonts);
        if ((pArrayOfTTFInfo[iFace].plf ==NULL) || (pArrayOfTTFInfo[iFace].ptm ==NULL)) 
            {
            MessageBox (NULL, L"内存分配失败", NULL, MB_OK);
            return FALSE;
            }
        *(pArrayOfTTFInfo[iFace].plf) = *lpLogFont;
        *(pArrayOfTTFInfo[iFace].ptm) = *lpTEXTMETRICs;
        iFace++;
        }
    return TRUE;
}

int APIENTRY MyEnumFontCountProc(LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC IPTEXTMETRICs,
                                 DWORD fFontType,
                                 LPINT lpData) 
{
    if (fFontType & TRUETYPE_FONTTYPE)
        (*lpData)++; 
    return TRUE;
}
