/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * combobox.c: This file is part of ...
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

/*
作用:说明组合框的使用。
本示范程序对于简单组合框、下拉式组合框以及下拉式列表框这三种
基本类型的组合框都给出了示范。并且，还介绍了自画组合框的设计
和使用。不仅如此，本示范程序还在对话框之外使用了组合框，为这
种组合框创建子类，使应用程序能很好地控制它的行为。
*/

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "resource.h"

HINSTANCE   hInst;
HWND        hWndMain;
HWND        hWndComboBoxTextString;
HWND        hWndComboBoxFontSize;
WNDPROC     lpfnEditWndProc;

PTRUETYPEFONTINFO   pArrayOfTTFInfo;
HDC                 hdcGlobal;
int                 iFace;
int                 nFaces;
int                 LogPixelSy;

LOGFONT LogFont = {-16, 0, 0, 0, FW_NORMAL, FALSE,FALSE,FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE,
    TEXT("abc")
};

COLORREF crTextColor;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR IpCmdLine,
                     int nCmdShow)
{
    MSG msg;
    if (!InitApplication (hInstance))
        return FALSE;
    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    SendMessage (hWndMain, WM_COMMAND, IDM_FONT, 0);

    while (GetMessage (&msg, NULL, 0,0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcexComboBox;
    wcexComboBox.cbSize        = sizeof(WNDCLASSEX);
    wcexComboBox.style         = 0;
    wcexComboBox.lpfnWndProc   = (WNDPROC)MainWndProc;
    wcexComboBox.cbClsExtra    = 0;
    wcexComboBox.cbWndExtra    = 0;
    wcexComboBox.hInstance     = hInstance;
    wcexComboBox.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    wcexComboBox.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wcexComboBox.hbrBackground = GetStockObject (WHITE_BRUSH);
    wcexComboBox.lpszMenuName  = TEXT("EditBoxMenu");
    wcexComboBox.lpszClassName = TEXT("ComboBoxWClass");
    wcexComboBox.hIconSm       = LoadIcon (hInstance, TEXT("about"));

    return (RegisterClassEx (&wcexComboBox));
}

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;
    hWnd = CreateWindow(TEXT("ComboBoxWClass"),
                        TEXT("组合框示范程序"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);
    if (!hWnd)
        return (FALSE);

    hWndMain = hWnd;
    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);
    return TRUE;
}


LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //static CHAR szBuf[128];
    static TCHAR szFontString[BUF_SIZE];
    HDC hDC;
    HFONT hFont = NULL;
    //static int nCurSize;
    UINT uItem;
    RECT Rect;
    RECT rcTextClip;
    POINT point;
    static HWND hWndComboBox;
    static HWND hWndEditTextString;
    static HWND hWndEditFontSize;
    switch (message)
        {
      case WM_CREATE:
            {
            crTextColor = RGB (0,0,0);
            /*创建两个组合框子窗口。*/
            hWndComboBoxTextString = CreateWindow(TEXT("COMBOBOX"),
                                                   NULL,
                                                   WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWN,
                                                   0,0,150,100,
                                                   hWnd,
                                                   (HMENU) IDM_TEXTSTRING,
                                                   hInst,
                                                   NULL);
            hWndComboBoxFontSize = CreateWindow(TEXT("COMBOBOX"),
                                                NULL,
                                                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWN,
                                                0,0,80,100,
                                                hWnd,
                                                (HMENU) IDM_FONTSIZE,
                                                hInst,
                                                NULL);
            /*预先填充上面两个组合框中的选项。*/
            FillComboBox();

            /*检取各组合框的编辑窗口句柄。*/
            point.x = 3;
            point.y = 3;

            hWndEditTextString = ChildWindowFromPoint (hWndComboBoxTextString, point);
            hWndEditFontSize = ChildWindowFromPoint (hWndComboBoxFontSize, point);

            /* 设置各编辑窗口的子类窗口过程。*/
            //lpfnEditWndProc = (WNDPROC) SetWindowLong (hWndEditTextString, GWL_WNDPROC, (LONG) SubClassProc);
            lpfnEditWndProc = SubclassWindow(hWndEditTextString, SubClassProc);

            //SetWindowLong (hWndEditFontSize, GWL_WNDPROC, (LONG) SubClassProc);
            SubclassWindow(hWndEditFontSize, SubClassProc);

            //枚举可用的True Type字体，建立字体信息数组。*/
            hDC = GetDC(hWnd);
            LogPixelSy = GetDeviceCaps (hDC, LOGPIXELSY);
            pArrayOfTTFInfo = BuildFontList (hDC, &nFaces);
            ReleaseDC(hWnd, hDC);
            lstrcpy (LogFont.lfFaceName, pArrayOfTTFInfo[0].plf->lfFaceName);
            wsprintf(szFontString, TEXT("现在采用的字体是%ls."), LogFont.lfFaceName);
            }
        break;
      case WM_SETFOCUS:
          SetFocus (hWndComboBoxTextString);
          break;
      case WM_SIZE:
          MoveWindow(hWndComboBoxFontSize, 0,0,80,100, TRUE);
          MoveWindow(hWndComboBoxTextString, 80, 0, LOWORD(lParam)-80,100, TRUE);
          SendMessage(hWndMain, WM_COMMAND, IDM_NULL, 0);
          break;
      case WM_COMMAND:
          uItem = LOWORD( wParam);
          switch(uItem)
              {
            case IDM_TEXTSTRING:
              switch (HIWORD(wParam))
                  {
                case CBN_SELCHANGE:
                      {
                      int iSelect;
                      //检取当前选择的要显示的字符串。
                      iSelect = SendMessage (hWndComboBoxTextString, CB_GETCURSEL, 0,0);
                      SendMessage (hWndComboBoxTextString, CB_GETLBTEXT,
                                   (WPARAM) iSelect, (LPARAM) (LPCWSTR) szFontString);
                      }
                  break;
                case CBN_EDITCHANGE:
                  SendMessage (hWndEditTextString, WM_GETTEXT,
                               (WPARAM) BUF_SIZE, (LPARAM) szFontString);
                  break;
                  }
              break;
            case IDM_FONTSIZE:
              switch (HIWORD(wParam))
                  {
                case CBN_SELCHANGE:
                      {
                      int iSelect;
                      TCHAR szFontSize[10];
                      //检取当前选择的字体尺寸。
                      iSelect = SendMessage(hWndComboBoxFontSize, CB_GETCURSEL, 0, 0);
                      SendMessage (hWndComboBoxFontSize, CB_GETLBTEXT, (WPARAM) iSelect, (LPARAM) (LPCWSTR) szFontSize);
                      LogFont.lfHeight = -MulDiv (_wtoi (szFontSize), LogPixelSy, 72);
                      }
                  break;
                  }
              break;
            case IDM_SELECTFONTFACE:
              DialogBox (hInst, TEXT("SelectFontFaceDlgBox"), hWnd, (DLGPROC) SelectFontFaceDlgProc);
              break;
            case IDM_SELECTFONTSTYLE:
              DialogBox (hInst, TEXT("SelectFontStyleDlgBox"), hWnd, (DLGPROC) SelectFontStyleDlgProc);
              break;
            case IDM_SELECTFONTCOLOR:
              DialogBox(hInst, TEXT("SelectFontColorDlgBox"), hWnd, (DLGPROC) SelectFontColorDlgProc);
              break;
            case IDM_SELECTFONTORIENT:
              DialogBox (hInst, TEXT("SelectFontOrientDlgBox"), hWnd, (DLGPROC) SelectFontOrientDlgProc);
              break;
            case IDM_EXIT:
              SendMessage (hWnd, WM_CLOSE, 0, 0L);
              return 0;
            case IDM_ABOUT:
              DialogBox (hInst, TEXT("AboutBox"), hWnd, (DLGPROC)About);
              return 0;
            case IDM_NULL:
              break;
            default:
              return (DefWindowProc(hWnd, message, wParam, lParam));
              }
          if (hFont != NULL)
              DeleteObject (hFont);

          hFont = CreateFontIndirect (&LogFont);
          if(hFont != NULL) {
              HPEN hPen;
              HBRUSH hBrush;

              hDC = GetDC( hWnd);

              GetClientRect (hWnd,&Rect);

              rcTextClip.left   = Rect.left;
              rcTextClip.top    = Rect.top + 23;
              rcTextClip.right  = Rect.right;
              rcTextClip.bottom = Rect.bottom;


              hPen = CreatePen (PS_SOLID, 1, RGB(200,200,200));
              hBrush = CreateSolidBrush (RGB(200,200,200));

              SelectObject (hDC, hPen);
              SelectObject (hDC, hBrush);

              Rectangle (hDC, rcTextClip.left, rcTextClip.top,
                         rcTextClip.right, rcTextClip.bottom);

              SelectObject (hDC, hFont);

              SetTextColor(hDC, crTextColor);
              SetTextAlign(hDC, TA_BASELINE | TA_CENTER );
              SetBkMode(hDC, TRANSPARENT);

              ExtTextOut (hDC,
                          rcTextClip.right/2, rcTextClip.bottom * 2/3,
                          ETO_CLIPPED,
                          &rcTextClip,
                          szFontString, wcslen(szFontString),
                          NULL);

              DeleteObject (hPen);
              DeleteObject (hBrush);

              ReleaseDC(hWnd, hDC);
          }
          break;
      case WM_TAB:
          /*切换组合框输入焦点。*/
          if (GetFocus() == hWndEditTextString)
              SetFocus (hWndComboBoxFontSize);
          else
              SetFocus (hWndComboBoxTextString);
          break;
      case WM_ESC:
          /*检取带有输入焦点的组合框的句柄。*/
          hWndComboBox = (GetFocus() == hWndEditTextString) ?  hWndComboBoxTextString : hWndComboBoxFontSize;

          /* 清除当前选择。*/
          SendMessage (hWndComboBox, CB_SETCURSEL, (WPARAM)(-1), 0);

          /*把输入焦点设置给主窗口。*/
          SetFocus (hWndMain);
          break;

      case WM_ENTER:
          hWndComboBox = (GetFocus() == hWndEditTextString) ?  hWndComboBoxTextString : hWndComboBoxFontSize;
          SetFocus (hWndMain);

          /*如果组合框中没有当前选择，则设定一个当前选择。*/
          if (SendMessage(hWndComboBox, CB_GETCURSEL, 0, 0) == CB_ERR) {
              TCHAR szItemText[BUF_SIZE];
              DWORD dwIndex;

              /*检取组合框的编辑控件中显示的文字；如果为空，则跳过。*/
              if (SendMessage (hWndComboBox, WM_GETTEXT, sizeof (szItemText), (LPARAM) szItemText) == 0)
                  break;

                /*在组合框选项中查找与编辑控件文字匹配的字符串。*/
                dwIndex = SendMessage (hWndComboBox, CB_FINDSTRINGEXACT,(WPARAM)(-1), (LPARAM) szItemText);

                /* 如果没有匹配的组合框选项，则添加该文字字符串，并选择该选项。*/
                  if (dwIndex == CB_ERR)
                      dwIndex = SendMessage(hWndComboBox, CB_INSERTSTRING,
                                            (WPARAM) 0, (LPARAM) szItemText);
                  if (dwIndex != CB_ERR)
                      SendMessage (hWndComboBox, CB_SETCURSEL, dwIndex, 0);

                  if(hWndComboBox == hWndComboBoxFontSize) {
                      LogFont.lfHeight = -MulDiv (_wtoi (szItemText), LogPixelSy, 72);
                      SendMessage (hWnd, WM_COMMAND, IDM_NULL, 0);
                  }
          }
          break;
      case WM_DESTROY:
          DeleteObject (hFont);
          PostQuitMessage (0);
          break;
      default:
          return (DefWindowProc (hWnd, message, wParam, lParam));
        }
    return (0);
}

VOID FillComboBox (void)
{
    int idx;
    TCHAR szTextColor[3][20] = {TEXT("床前明月光"), TEXT("远上寒山石径斜"), TEXT("众里寻他千百度")};

    static int aPoints[NUM_POINTS] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
    TCHAR szFontSize[10];

    /*把字体尺寸字符串插入到字体尺寸组合框中。*/
    for (idx = 0; idx < NUM_POINTS; idx++) {
        wsprintf (szFontSize, TEXT("%ld"), aPoints[idx]);
        SendMessage (hWndComboBoxFontSize, CB_ADDSTRING, 0,
                     (LPARAM) (LPCSTR) szFontSize);
    }
    wsprintf(szFontSize, TEXT("%d"), aPoints[4]);
    SendMessage (hWndComboBoxFontSize, WM_SETTEXT, 0,
                 (LPARAM) (LPCWSTR) szFontSize);

    /* 把演示文字字符串插入到演示文字组合框中。*/
    for (idx = 0; idx < 3; idx++) {
        SendMessage (hWndComboBoxTextString, CB_ADDSTRING, 0,
                     (LPARAM) (LPCSTR) szTextColor[idx]);
    }

    SendMessage (hWndComboBoxTextString, WM_SETTEXT, 0,
                 (LPARAM) (LPCSTR) szTextColor[0]);
}

/*
 * 函数:SelectFontFaceDlgProc(HWND,UINT,WPARAM, LPARAM)
 * 用途:选择字体字样的对话框过程。这里介绍简单类型的组合框的设计。
 **/
BOOL APIENTRY SelectFontFaceDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int idx;
    //static int nCurrentFont = 0;

    switch (message) {
      case WM_INITDIALOG:
        /*把字体信息数组中的字样名称插入到组合框中。*/
        for (idx = 0; idx < nFaces; idx++) {
            SendMessage (GetDlgItem (hDlg, IDC_FONTFACE), CB_ADDSTRING,
                         0, (LPARAM) (LPCSTR) pArrayOfTTFInfo[idx].plf->lfFaceName);
        }

        /*根据当前使用的字样，选择相应的组合框选项。*/
        SendMessage(GetDlgItem (hDlg, IDC_FONTFACE), CB_SELECTSTRING,
                    (WPARAM)-1, (LPARAM) (LPCSTR) LogFont.lfFaceName);

        SetFocus (GetDlgItem (hDlg, IDC_FONTFACE));
        return TRUE;
      case WM_COMMAND:
          switch (LOWORD(wParam)) {
              case IDC_FONTFACE:
                if(HIWORD(wParam) != CBN_DBLCLK)
                    break;
              case IDOK:
                     SendDlgItemMessage(hDlg, IDC_FONTFACE,
                                        WM_GETTEXT,
                                        sizeof(LogFont.lfFaceName),
                                        (LPARAM)LogFont.lfFaceName);
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

BOOL APIENTRY SelectFontStyleDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hListBox;
    TCHAR szFontFaceStyle[4][10] = {TEXT("粗体"), TEXT("斜体"), TEXT("加下划线"), TEXT("加删除线")};
    int idx;
    static BOOL bBold = FALSE, bItalic =FALSE, bUnderLine = FALSE,
                bStrikeOut = FALSE;
    switch (message) {
      case WM_INITDIALOG:
        for (idx=0; idx < 4; idx++) {
            SendMessage (GetDlgItem (hDlg, IDL_FONTSTYLE), LB_ADDSTRING,
                         0, (LPARAM) (LPCWSTR) szFontFaceStyle[idx]);
        }
        SetFocus (GetDlgItem (hDlg, IDL_FONTSTYLE));
        if(bBold)
            SendMessage (GetDlgItem (hDlg, IDL_FONTSTYLE),
                         LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(0,0));
        if(bItalic)
            SendMessage (GetDlgItem (hDlg, IDL_FONTSTYLE),
                         LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(1,1));
        if(bUnderLine)
            SendMessage (GetDlgItem (hDlg, IDL_FONTSTYLE),
                         LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(2,2));
        if(bStrikeOut)
            SendMessage (GetDlgItem (hDlg, IDL_FONTSTYLE),
                         LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(3,3));
        return TRUE;
      case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case IDOK:
              hListBox = GetDlgItem(hDlg, IDL_FONTSTYLE);
              bBold = (SendMessage (hListBox, LB_GETSEL,
                                  (WPARAM) 0, (LPARAM) 0) >0 ) ? TRUE: FALSE;
              LogFont.lfWeight = bBold ? FW_BOLD : FW_NORMAL;

              bItalic = (SendMessage (hListBox, LB_GETSEL,
                                     (WPARAM)1, (LPARAM) 0) > 0 ) ? TRUE : FALSE;
              LogFont.lfItalic = bItalic;

              bUnderLine = (SendMessage (hListBox, LB_GETSEL,
                                        (WPARAM)2, (LPARAM) 0) > 0 ) ? TRUE : FALSE;
              LogFont.lfUnderline = bUnderLine;

              bStrikeOut = (SendMessage(hListBox, LB_GETSEL,
                                      (WPARAM) 3, (LPARAM) 0) > 0) ? TRUE : FALSE;
              LogFont.lfStrikeOut = bStrikeOut;
              EndDialog (hDlg, TRUE);
              return TRUE;
            case IDCANCEL:
              EndDialog (hDlg, FALSE);
              return TRUE;
            default:
                return FALSE;
          }
    }
    return FALSE;
}

/*
 * 函数: SelectFontColorDlgProc (hDlg, message, wParam, lParam)
 * 用途: 选择字体颜色的对话框过程。
 *       该对话框中有一个自画组合框，本过程为该组合框添加彩色
 *       矩形选项，并处理选项的选择和焦点消息。
 */
BOOL CALLBACK SelectFontColorDlgProc(HWND hDlg, UINT message,
                                     WPARAM wParam, LPARAM lParam)
{
    LPDRAWITEMSTRUCT        lpdis;
    LPMEASUREITEMSTRUCT     lpmis;
    TEXTMETRIC              tm;
    COLORREF                crFontColor;
    RECT                    rect;
    TCHAR                   szItemString[10];
    int                     nItem;
    static HWND             hComboBoxFontColor;
    static int              nItemCurSel = 0;

    switch (message) {
      case WM_INITDIALOG:
        hComboBoxFontColor = GetDlgItem (hDlg, IDC_FONTCOLOR);
        nItem = SendMessage (hComboBoxFontColor, CB_ADDSTRING,
                             0, (LPARAM) TEXT("黑"));
        SendMessage (hComboBoxFontColor, CB_SETITEMDATA, nItem,
                     (LPARAM) RGB(0,0,0));
        nItem = SendMessage (hComboBoxFontColor, CB_ADDSTRING,
                             0, (LPARAM) TEXT("红"));
        SendMessage (hComboBoxFontColor, CB_SETITEMDATA, nItem,
                     (LPARAM) RGB(255,0,0));
        nItem = SendMessage (hComboBoxFontColor, CB_ADDSTRING,
                             0, (LPARAM) TEXT("绿"));
        SendMessage (hComboBoxFontColor, CB_SETITEMDATA, nItem,
                     (LPARAM) RGB(0,255,0));
        nItem = SendMessage (hComboBoxFontColor, CB_ADDSTRING,
                            0, (LPARAM) TEXT("蓝"));
        SendMessage (hComboBoxFontColor, CB_SETITEMDATA, nItem,
                     (LPARAM) RGB(0,0,255));
        nItem = SendMessage (hComboBoxFontColor, CB_ADDSTRING,
                            0, (LPARAM) TEXT("黄"));
        SendMessage (hComboBoxFontColor, CB_SETITEMDATA, nItem,
                     (LPARAM) RGB(255, 255,0));
        nItem = SendMessage (hComboBoxFontColor, CB_ADDSTRING,
                             0, (LPARAM) TEXT("灰"));
        SendMessage (hComboBoxFontColor, CB_SETITEMDATA, nItem,
                     (LPARAM) RGB(150, 150,150));
        SendMessage (hComboBoxFontColor, CB_SETCURSEL,
                     (WPARAM) nItemCurSel, 0);
        return TRUE;
      case WM_DRAWITEM:
        lpdis = (LPDRAWITEMSTRUCT) lParam;
        if (lpdis -> itemID == -1) {
            break;
        }
        switch (lpdis->itemAction) {
          case ODA_DRAWENTIRE:
          case ODA_SELECT:
            crFontColor = (COLORREF) SendMessage (lpdis->hwndItem,
                                                  CB_GETITEMDATA, lpdis -> itemID, (LPARAM) 0);

            rect.left       = (lpdis->rcItem).left + 4;
            rect.top        = (lpdis->rcItem).top + 4;
            rect.right      = (lpdis->rcItem).right - 30;
            rect.bottom     = (lpdis->rcItem).bottom -4;

            FillRect(lpdis->hDC, (LPRECT)&rect, CreateSolidBrush (crFontColor)); 

            SendMessage (lpdis->hwndItem, CB_GETLBTEXT, lpdis->itemID,
                         (LPARAM) szItemString);

            GetTextMetrics (lpdis->hDC, &tm);

            SetTextColor (lpdis->hDC, crFontColor);

            TextOut (lpdis->hDC, rect.right + 4,
                     (lpdis->rcItem.top + lpdis->rcItem.bottom - tm.tmHeight)/2,
                     szItemString, lstrlen (szItemString));

            if (lpdis->itemState & ODS_SELECTED) {
                rect.left   = (lpdis->rcItem).left + 10;
                rect.top    = (lpdis->rcItem).top + 10;
                rect.right  = (lpdis->rcItem).right - 36;
                rect.bottom = (lpdis->rcItem).bottom - 10;

                InvertRect (lpdis->hDC, (LPRECT) &rect);
            }
            break;
          case ODA_FOCUS:
            break;
        }
        return (TRUE);
        break;
      case WM_MEASUREITEM:
        lpmis = (LPMEASUREITEMSTRUCT) lParam;
        lpmis -> itemHeight = 25;
        break;
      case WM_CLOSE:
        EndDialog (hDlg, 0);
        return (TRUE);
        break;
      case WM_COMMAND:
        switch (LOWORD(wParam)) {
          case IDOK:
              nItemCurSel = SendMessage (hComboBoxFontColor, CB_GETCURSEL,0,0);
              crTextColor = (COLORREF) SendMessage (hComboBoxFontColor,
                                                   CB_GETITEMDATA, (WPARAM) nItemCurSel,0);

          case IDCANCEL:
              EndDialog (hDlg, TRUE);
              return (TRUE);
              break;
          default:
              return (FALSE);
              break;
        }
      default:
        return (FALSE);
    }
    return (TRUE);
}

BOOL APIENTRY SelectFontOrientDlgProc(HWND hDlg, UINT message,
                                      WPARAM wParam, LPARAM lParam)
{
    static TCHAR szFontOrientation[20] = TEXT("0.0");
    int idx;
    int nSelItem;
    static int nCurFontOrientation = 0;

    switch (message) {
      case WM_INITDIALOG:
        for (idx= 0; idx < 360; idx += 5) {
            wsprintf(szFontOrientation, TEXT("倾斜%d度"), (idx<-180) ? idx : (idx-360));

            SendMessage (GetDlgItem (hDlg, IDC_FONTORIENT),
                         CB_ADDSTRING, 0, (LPARAM) (LPCSTR) szFontOrientation);
        }
        SetFocus (GetDlgItem (hDlg, IDC_FONTORIENT));

        wsprintf (szFontOrientation, TEXT("倾斜%d度"),
                 (nCurFontOrientation <= 180) ? nCurFontOrientation :
                 (nCurFontOrientation - 360));

        SendMessage (GetDlgItem (hDlg, IDC_FONTORIENT),
                     CB_SELECTSTRING, (WPARAM) -1,
                     (LPARAM) (LPCSTR)szFontOrientation);
        return (TRUE);

      case WM_COMMAND:
        switch( LOWORD(wParam)) {
          case IDOK:
          case IDL_FONTFACE:
            if(LOWORD(wParam) == IDOK || HIWORD(wParam) == CBN_DBLCLK) {
                nSelItem  =  SendMessage (GetDlgItem (hDlg, IDC_FONTORIENT),
                                          CB_GETCURSEL, 0,0);

                nCurFontOrientation = nSelItem * 5;
                LogFont.lfEscapement = LogFont.lfOrientation - nCurFontOrientation * 10;
                EndDialog (hDlg, TRUE);
                return (TRUE);
            }
            break;
          case IDCANCEL:
            EndDialog (hDlg, FALSE);
            return (TRUE);
          default:
            return (FALSE);
        }
    }
    return (FALSE);
}

BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
      case WM_INITDIALOG:
          return (TRUE);
      case WM_COMMAND:
          if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
              EndDialog (hDlg,TRUE); 
              return (TRUE); 
          }
          break;
    }
    return (FALSE);
}


PTRUETYPEFONTINFO BuildFontList(HDC hDCFont, //设备描述表句柄
                                LPINT retnFaces) //返回字体数目的变量的指针
{
    nFaces = 0; //全局字体计数变量初始化为
    hdcGlobal = hDCFont; //保存好字体数组对应的设备描述表save，以备回调函数中使用。

    //首先算出系统中可用True Type字体的数目。
    EnumFontFamilies (hdcGlobal, NULL, (FONTENUMPROC)MyEnumFontCountProc,
                      (LPARAM)&nFaces);

    //分配字体信息结构的数组。
    pArrayOfTTFInfo = (PTRUETYPEFONTINFO)LocalAlloc (LPTR, sizeof (TRUETYPEFONTINFO) * (nFaces +1));

    /* 再次枚举所有可用的TrueT ype字体。对于每个字体，都填写好各自的逻辑字体 *
     * 信息结构(LOGFONT)和物理字体信息结构(TEXTMETRIC)。                     */
    iFace = 0;

    EnumFontFamilies (hdcGlobal, NULL, (FONTENUMPROC)MyEnumFontFacesProc,
                      (LPARAM)NULL);
    *retnFaces = nFaces;
    return pArrayOfTTFInfo;
}


int APIENTRY MyEnumFontFacesProc(
                                 LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPVOID lpData)
{
    if (fFontType & TRUETYPE_FONTTYPE) {      /*确定该字体属于True Type型字体。*/
        pArrayOfTTFInfo[iFace].plf = (LPLOGFONT)LocalAlloc (LPTR,
                                                            sizeof (LOGFONT)); //* nFonts);
        pArrayOfTTFInfo[iFace].ptm = (LPTEXTMETRIC)LocalAlloc(LPTR,
                                                              sizeof (TEXTMETRIC)); // * nFonts);
        if ((pArrayOfTTFInfo[iFace].plf ==  NULL) || (pArrayOfTTFInfo[iFace].ptm == NULL)) {
            MessageBox(NULL, TEXT("内存分配失败"), NULL, MB_OK);
            return FALSE;
        }

        *(pArrayOfTTFInfo[iFace].plf) = *lpLogFont;
        *(pArrayOfTTFInfo[iFace].ptm) = *lpTEXTMETRICs;

        iFace ++;
    }
    return TRUE;
}


int APIENTRY MyEnumFontCountProc(
                                 LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPINT lpData)
{
    if (fFontType & TRUETYPE_FONTTYPE)
        (* lpData)++;
    return TRUE;
}


/*
 * 函数: SubClassProc (HWND, UINT, WPARAM, LPARAM)
 * 用途: 处理编辑控件中的Tab和Escape按键消息，并把其他所有
         消息交给该类原有的窗口过程处理。
*******/

LRESULT CALLBACK SubClassProc(HWND hWnd,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam)
{
    switch (message) {
      case WM_KEYDOWN:
          switch (wParam) {
            case VK_TAB:
              SendMessage (hWndMain, WM_TAB, 0, 0);
              return 0;
            case VK_ESCAPE:
              SendMessage (hWndMain, WM_ESC, 0, 0);
              return 0;
            case VK_RETURN:
              SendMessage (hWndMain, WM_ENTER, 0, 0);
              break;
          }
          break;
      case WM_KEYUP:
      case WM_CHAR:
          switch (wParam) {
            case VK_TAB:
            case VK_ESCAPE:
            case VK_RETURN:
              break;
          }
    }

    /*调用原有的窗口过程，对消息进行默认处理。*/
    return CallWindowProc (lpfnEditWndProc, hWnd, message, wParam, lParam);
}
