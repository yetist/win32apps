/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * bitmap.c: This file is part of ...
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
#include "bitmap.h"

HINSTANCE hInst;
HWND hWndMain;
HBITMAP hSun, hMoon;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg;
  HACCEL hAccel;

  if (!InitApplication(hInstance))
    return FALSE;

  if (!InitInstance(hInstance, nCmdShow))
    return FALSE;

  // 装入快捷键资源
  hAccel = LoadAccelerators(hInstance, TEXT("rcMenu"));
  hSun = LoadBitmap(hInstance, TEXT("Sun"));
  hMoon = LoadBitmap(hInstance, TEXT("Moon"));
  InvalidateRect(hWndMain, NULL, TRUE);

  while (GetMessage(&msg, NULL, 0, 0)) {
      if (!TranslateAccelerator(hWndMain, hAccel, &msg))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
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
    wcex.hIcon         = LoadIcon(hInstance, TEXT("rcIcon"));
    wcex.hCursor       = LoadCursor(NULL, TEXT("rcCursor"));
    wcex.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName  = TEXT("rcMenu");
    wcex.lpszClassName = TEXT("rcWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("Small"));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    //HWND hWnd;
    hInst = hInstance;

    hWndMain = CreateWindow(
            TEXT("rcWClass"),
            TEXT("位图示范程序"),
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

LRESULT APIENTRY MainWndProc(HWND hWnd,         // 窗口句柄
        UINT message,          // 消息类型
        WPARAM wParam,     // 消息附带信息
        LPARAM lParam)     // 消息附带信息
{
    HDC hdc, hMemDc;
    PAINTSTRUCT ps;
    TCHAR szBuffer[10];

    switch (message) {
        case WM_PAINT:
          hdc = BeginPaint(hWnd, &ps);
          SetTextColor(hdc, fIsRed ? RGB(255, 0, 0) : RGB(0, 0, 255));

          if (fIsDay) {
              LoadString(hInst, IDS_SUN, szBuffer, 10);
              TextOut(hdc, 20, 20, szBuffer, lstrlen(szBuffer));
              hMemDc = CreateCompatibleDC(hdc);
              SelectObject(hMemDc, hSun);
              BitBlt(hdc, 90, 8, 48, 48, hMemDc, 0, 0, SRCCOPY);
              DeleteDC(hMemDc);
          }

          if (fIsNight) {
              LoadString(hInst, IDS_MOON, szBuffer, 10);
              TextOut(hdc, 20, 90, szBuffer, lstrlen(szBuffer));
              hMemDc = CreateCompatibleDC(hdc);
              SelectObject(hMemDc, hMoon);
              BitBlt(hdc, 90, 78, 48, 48, hMemDc, 0, 0, SRCCOPY);
              DeleteDC(hMemDc);
          }
          EndPaint(hWnd, &ps);
          break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_OPEN:
                  DialogBox(hInst, TEXT("TextBox"), hWnd, (DLGPROC) TextDlgProc);
                  InvalidateRect(hWnd, NULL, TRUE);
                  break;
                case IDM_EXIT:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_ABOUT:
                    DialogBox(hInst, TEXT("AboutBox"), hWnd, (DLGPROC) TextDlgProc);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_DESTROY:
            DeleteObject(hSun);
            DeleteObject(hMoon);
            PostQuitMessage(0);
            break;
        default:
            // 调用默认窗口过程对未处理的消息进行必要的处理
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
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

BOOL APIENTRY TextDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message) {
    case WM_INITDIALOG:
      CheckRadioButton(hDlg, IDD_RED, IDD_BLUE, fIsRed ? IDD_RED : IDD_BLUE);
      if (fIsDay)
        CheckDlgButton(hDlg, IDD_DAY, BST_CHECKED);
      if (fIsNight)
        CheckDlgButton(hDlg, IDD_NIGHT, BST_CHECKED);
      return TRUE;
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDD_RED:
        case IDD_BLUE:
          CheckRadioButton(hDlg, IDD_RED, IDD_BLUE, LOWORD(wParam));
          return TRUE;
        case IDOK:
          fIsRed = IsDlgButtonChecked(hDlg, IDD_RED);
          fIsDay = IsDlgButtonChecked(hDlg, IDD_DAY);
          fIsNight = IsDlgButtonChecked(hDlg, IDD_NIGHT);
          EndDialog(hDlg, TRUE);
          return TRUE;
        case IDCANCEL:
          EndDialog(hDlg, FALSE);
          return TRUE;
      }
      break;
  }
  return FALSE;
}
