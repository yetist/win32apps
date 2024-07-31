/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * scrllbar.c: This file is part of ...
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
#include "scrllbar.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    if (!InitApplication(hInstance))
        return FALSE;

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    SendMessage(hWndMain, WM_COMMAND, IDM_DRAW, 0L);

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
    wcex.lpszMenuName  = TEXT("ScrollBarMenu");
    wcex.lpszClassName = TEXT("ScrollBarWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("SmallIcon"));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndMain = CreateWindow(TEXT("ScrollBarWClass"),
                            TEXT("滚动条控件示范程序"),
                            WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);

    if (!hWndMain)
        return FALSE;

    crRGB[0] = 0;
    crRGB[1] = 0;
    crRGB[2] = 0;

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
    HBRUSH  hBrush;
    static HWND hWndScrollBar;
    static SCROLLINFO stScrollInfo;
    static int nRectWidth;

    switch (message) {
        case WM_CREATE:
          hWndScrollBar = CreateWindowEx (0L,
                                          TEXT("SCROLLBAR"),
                                          (LPWSTR) NULL,
                                          WS_CHILD | SBS_VERT,
                                          10,
                                          30 - GetSystemMetrics (SM_CYVSCROLL),
                                          19,
                                          100 + 2*GetSystemMetrics(SM_CYVSCROLL),
                                          hWnd,
                                          (HMENU) NULL,
                                          hInst,
                                          (LPVOID) NULL);
          stScrollInfo.cbSize   = sizeof(SCROLLINFO);
          stScrollInfo.fMask    = SIF_POS | SIF_RANGE | SIF_PAGE;
          stScrollInfo.nMin     = 1;
          stScrollInfo.nMax     = 100;
          stScrollInfo.nPos     = 1;
          stScrollInfo.nPage    = 10;

          // 设置滚动条控件的各方面特性参数
          SetScrollInfo (hWndScrollBar, SB_CTL, &stScrollInfo, FALSE);
          ShowWindow(hWndScrollBar, TRUE);
          nRectWidth = 100;
          break;
        case WM_VSCROLL:
          if ((HWND) lParam != hWndScrollBar)
              break;
          switch (LOWORD(wParam)) {
            case SB_LINEUP:
              nRectWidth += 1;
              break;
            case SB_LINEDOWN:
              nRectWidth -= 1;
              break;
            case SB_PAGEUP:
              nRectWidth += 10;
              break;
            case SB_PAGEDOWN:
              nRectWidth -= 10;
              break;
            case SB_THUMBPOSITION:
              nRectWidth = 101 - HIWORD(wParam);
              break;
            default:
              // 调用默认窗口过程对未处理的消息进行必要的处理
              return DefWindowProc(hWnd, message, wParam, lParam);
          }

          if (nRectWidth < 1)
              nRectWidth = 1;
          else if (nRectWidth > 100)
              nRectWidth = 100;

          stScrollInfo.fMask = SIF_POS;
          stScrollInfo.nPos = 101 - nRectWidth;
          SetScrollInfo(hWndScrollBar, SB_CTL, &stScrollInfo, TRUE);

          SendMessage(hWndMain, WM_COMMAND, IDM_DRAW, 0L);
          break;
        case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case IDM_RGB:
              DialogBox(hInst, TEXT("RGBDlg"), hWnd, (DLGPROC) RGBDlgProc);
              break;
            case IDM_DRAW:
              hDC = GetDC(hWnd);
              hBrush = CreateSolidBrush(RGB(255, 255, 255));
              SelectObject(hDC, hBrush);
              Rectangle(hDC, 30, 30, 130, 130);

              hBrush = CreateSolidBrush(RGB(crRGB[0], crRGB[1], crRGB[2]));
              SelectObject(hDC, hBrush);
              Rectangle(hDC, 30, 130 - nRectWidth, 30 + nRectWidth, 130);
              ReleaseDC(hWnd, hDC);
              break;
            case IDM_EXIT:
              PostQuitMessage(0);
              break;
            case IDM_ABOUT:
              DialogBox(hInst, TEXT("AboutBox"), hWnd, (DLGPROC) About);
              break;
            default:
              return DefWindowProc(hWnd, message, wParam, lParam);
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

BOOL APIENTRY RGBDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int CurrentVal[3], PrevVal[3];
    static HWND hwScroll[3];
    TCHAR szString[10];
    int i;

    switch(message) {
      case WM_INITDIALOG:
        for(i = 0; i < 3; i ++) {
            CurrentVal[i] = crRGB[i];
            PrevVal[i] = crRGB[i];

            hwScroll[i] = GetDlgItem(hDlg, IDC_RED + i);
            SetScrollRange(hwScroll[i], SB_CTL, 0, 255, FALSE);
            SetScrollPos(hwScroll[i], SB_CTL, CurrentVal[i], TRUE);
            SetDlgItemText(hDlg, IDC_VALRED + i, (LPCWSTR) _itow(CurrentVal[i], szString,
                                                      10));
        }
        SetFocus(hwScroll[0]);
        break;

        /* 处理滚动条通知消息 */
      case WM_HSCROLL:
        /* 首先确定本条消息来自于哪个滚动条 */
        i = 0;
        while(hwScroll[i] != (HWND) lParam)
            i++;

        switch(LOWORD(wParam)) {
          case SB_LINEDOWN:
            CurrentVal[i]++;
            if (CurrentVal[i] > 255)
                CurrentVal[i] = 255;
            break;
          case SB_LINEUP:
            CurrentVal[i]--;
            if (CurrentVal[i] < 0)
                CurrentVal[i] = 0;
            break;
          case SB_PAGEDOWN:
            CurrentVal[i] += (255 - CurrentVal[i])/2;
            break;
          case SB_PAGEUP:
            CurrentVal[i] -= (255 - CurrentVal[i])/2;
            break;
          case SB_THUMBTRACK:
            CurrentVal[i] = HIWORD(wParam);
            break;
          case SB_TOP:
            CurrentVal[i] = 0;
            break;
          case SB_BOTTOM:
            CurrentVal[i] = 255;
            break;
        }

        /* 设定滚动条的位置及其对应的文字 */
        SetScrollPos(hwScroll[i], SB_CTL, CurrentVal[i], TRUE);
        SetDlgItemText(hDlg, IDC_VALRED + i, (LPCWSTR) _itow(CurrentVal[i], szString, 10));
        crRGB[i] = CurrentVal[i];
        SendMessage(hWndMain, WM_COMMAND, IDM_DRAW, 0L);
        break;
      case WM_COMMAND:
        switch(wParam) {
          case IDOK:
            for (i = 0; i < 3; i++)
                crRGB[i] = CurrentVal[i];
            EndDialog(hDlg, TRUE);
            return TRUE;
          case IDCANCEL:
            for (i = 0; i < 3; i++)
                crRGB[i] = PrevVal[i];
            SendMessage(hWndMain, WM_COMMAND, IDM_DRAW, 0L);
            EndDialog(hDlg, FALSE);
            return TRUE;
        }
        break;
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
