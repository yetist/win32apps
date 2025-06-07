/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * editbox.c: This file is part of ...
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
#include <string.h>
#include "editbox.h"

HINSTANCE  hInst;
HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    /* 显示对话框 , 让用户输入口令 。
       最多可输入三次，如果口令还是无效，则退出应用程序。
       */
    if (!DialogBox (hInstance, TEXT("CheckPasswordDlgBox"),
                     NULL, (DLGPROC) PasswordDlgProc))
        return FALSE;

    if (!InitApplication (hInstance))
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
    wcex.lpszMenuName  = TEXT("EditBoxMenu");
    wcex.lpszClassName = TEXT("EditBoxWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("Small"));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    hWnd = CreateWindow(TEXT("EditBoxWClass"),
                        TEXT("编辑框示范程序"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return FALSE;

    hWndMain = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    UINT uItem;
    static HWND hWndEdit;

    switch (message) {
      case WM_CREATE:
        // 创建一个编辑控件
        hWndEdit = CreateWindow (TEXT("EDIT"),                          // 窗口类名称
                                 NULL,                                  // 无标题文字
                                 WS_CHILD | WS_VISIBLE | WS_BORDER |
                                 WS_VSCROLL | WS_HSCROLL |              // 带有水平和垂直滚动条
                                 ES_AUTOHSCROLL | ES_AUTOVSCROLL |      // 水平和垂直方向上都能自动滚动
                                 ES_LEFT |                              // 文字左对齐排列
                                 ES_MULTILINE,                          // 可容纳和编辑多行文字
                                 0, 0, 0,0,                             // 位置和尺寸待定
                                 hWnd,                                  // 父窗口句柄
                                 (HMENU) ID_EDITCONTROL,                // 控件标识号
                                 hInst,
                                 (LPVOID) NULL);
        break;
      case WM_SETFOCUS:
        SetFocus(hWndEdit);
        break;
      case WM_SIZE:
        MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
      case WM_INITMENUPOPUP:
        /* 判断当前编辑控件是否可以进行撒销操作，以此
         * 来设置【撤销】菜单项的状态。*/
        if (SendMessage(hWndEdit, EM_CANUNDO, 0, 0))
            EnableMenuItem (GetMenu(hWndMain) , IDM_EDITUNDO,
                            MF_BYCOMMAND | MF_ENABLED );
        else
            EnableMenuItem (GetMenu (hWndMain), IDM_EDITUNDO,
                            MF_BYCOMMAND | MF_GRAYED);
        break ;

      case WM_COMMAND:
        uItem = LOWORD( wParam );
        switch (uItem) {
          case IDM_EDITUNDO: /* 撒销 */
            SendMessage( hWndEdit, WM_UNDO,0, 0 );
            break;
          case IDM_EDITCUT: /* 剪切 */
            SendMessage( hWndEdit, WM_CUT, 0, 0 );
            break;
          case IDM_EDITCOPY: /* 复制 */
            SendMessage( hWndEdit, WM_COPY, 0, 0);
            break;
          case IDM_EDITPASTE: /* 粘贴 */
            SendMessage( hWndEdit, WM_PASTE,0, 0);
            break ;
          case IDM_EDITDELETE: /* 删除 */
            SendMessage( hWndEdit, WM_CLEAR, 0, 0);
            break;
          case IDM_EDITSELECTALL: /* 全部选定 */
            SendMessage(hWndEdit, EM_SETSEL, 0, -1);
            break;
          case IDM_SETWINPOSITION: /* 设置窗口位置 */
            DialogBox(hInst, TEXT("SetWindowPositionDlgBox"), hWnd,
                      (DLGPROC) SetWinPositionDlgProc);
            break;
          case IDM_SETWINCAPTION:
            DialogBox(hInst, TEXT("SetWindowCaptionDlgBox"), hWnd,
                      (DLGPROC) SetWinCaptionDlgProc);
            break;
          case IDM_EXIT:
            SendMessage(hWnd, WM_CLOSE, 0, 0L);
            return 0;
          case IDM_ABOUT:
            DialogBox(hInst, TEXT("AboutBox"), hWnd, (DLGPROC) About);
            return 0;
          default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
        }
        break;
      case WM_DESTROY:
        PostQuitMessage(0);
        break;
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL APIENTRY SetWinPositionDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    static int nWinWidth, nWinHeight;
    BOOL bErr;

    switch (message) {
      case WM_INITDIALOG:
        GetWindowRect (hWndMain , &rect);
        nWinWidth = rect.right - rect.left;
        nWinHeight = rect.bottom - rect.top;

        // 设置编辑控件中的初始数值
        SetDlgItemInt (hDlg, IDC_EDITWINLEFT, rect.left, TRUE);
        SetDlgItemInt (hDlg, IDC_EDITWINTOP,  rect.top, TRUE);

        return TRUE;
      case WM_COMMAND:
        switch(LOWORD(wParam)) {
          case IDOK:
            rect.left = GetDlgItemInt(hDlg, IDC_EDITWINLEFT, &bErr, TRUE);
            rect.right = GetDlgItemInt(hDlg, IDC_EDITWINTOP, &bErr, TRUE);

            MoveWindow(hWndMain, rect.left, rect.top, nWinWidth, nWinHeight,
                       TRUE);
            EndDialog (hDlg, TRUE);
            return TRUE;
          case IDCANCEL:
              EndDialog (hDlg, FALSE);
              return TRUE;
        }
        break;
    }
    return FALSE;
}

BOOL APIENTRY SetWinCaptionDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                   LPARAM lParam)
{
    TCHAR szWinCaption[80];

    switch(message) {
      case WM_INITDIALOG:
        GetWindowText(hWndMain, (LPTSTR) szWinCaption, 80);
        SendMessage(GetDlgItem(hDlg, IDC_EDITWINCAPTION), WM_SETTEXT, 0,
                    (LPARAM) szWinCaption);
        return TRUE;
      case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case IDOK:
              SendDlgItemMessage(hDlg, IDC_EDITWINCAPTION, WM_GETTEXT, (WPARAM)
                                 880, (LPARAM) szWinCaption);
              SetWindowText(hWndMain, szWinCaption);
              EndDialog(hDlg, TRUE);
              return TRUE;
            case IDCANCEL:
              EndDialog(hDlg, FALSE);
              return TRUE;
              break;
          }
    }
    return FALSE;
}

BOOL APIENTRY PasswordDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM
                              lParam)
{
    static TCHAR szPassword[20];
    int idx;
    int nPassword;
    BOOL bMatch;
    static int nInvalidPassword = 0;

    switch(message) {
      case WM_INITDIALOG:
        SendDlgItemMessage(hDlg, IDE_PASSWORD, EM_SETPASSWORDCHAR, (WPARAM) '?',
                           (LPARAM) 0);
        SendDlgItemMessage(hDlg, IDE_PASSWORD, EM_SETLIMITTEXT, (WPARAM) 5,
                           (LPARAM) 0);
        SendDlgItemMessage(hDlg, IDE_PASSWORD, DM_SETDEFID, (WPARAM) IDCANCEL,
                           (LPARAM) 0);

        SetFocus(GetDlgItem(hDlg, IDE_PASSWORD));
        return TRUE;
      case WM_COMMAND:
        switch(LOWORD(wParam)) {
          case IDE_PASSWORD:
            switch (HIWORD(wParam)) {
              case EN_CHANGE:
                nPassword = (WORD) SendDlgItemMessage(hDlg, IDE_PASSWORD,
                                                      EM_LINELENGTH, (WPARAM)
                                                      0,(WPARAM) 0);
                if (nPassword == 5) {
                    EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
                    SendMessage(hDlg, DM_SETDEFID, (WPARAM) IDOK, (LPARAM) 0);
                } else {
                    EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
                    SendMessage(hDlg, DM_SETDEFID, (WPARAM) IDOK, (LPARAM) 0);
                }
                break;
              case EN_MAXTEXT:
                MessageBeep(MB_ICONHAND);
                break;
            }
            break;
          case IDOK:
            *((LPWORD) szPassword) = 5;
            SendDlgItemMessage(hDlg, IDE_PASSWORD, EM_GETLINE, (WPARAM) 0,
                               (LPARAM) szPassword);
            bMatch = TRUE;
            for(idx = 0; idx < 5; idx++) {
                if(szPassword[idx] != '0')
                    bMatch = FALSE;
            }
            if(bMatch) {
                EndDialog(hDlg, TRUE);
                return TRUE;
            } else {
                if(nInvalidPassword < 2) {
                    MessageBox(hDlg, TEXT("请再次输入口令！"), TEXT("口令非法"), MB_OK |
                               MB_ICONEXCLAMATION);
                    SendMessage(GetDlgItem(hDlg, IDE_PASSWORD), EM_SETSEL, 0,
                                -1);
                    SendMessage(GetDlgItem(hDlg, IDE_PASSWORD), WM_CLEAR, 0,
                                0);
                    SetFocus(GetDlgItem(hDlg, IDE_PASSWORD));

                    nInvalidPassword++;
                    return FALSE;
                } else {
                    EndDialog(hDlg, FALSE);
                }
            }
          case IDCANCEL:
            EndDialog(hDlg, FALSE);
            return TRUE;
          default:
            return FALSE;
        }
    }
    return FALSE;
}

BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
      case WM_INITDIALOG:
        return TRUE;
      case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;
    }

    return FALSE;
}
