/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * mouse.c: This file is part of ...
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

#include <windows.h>
#include <string.h>
#include "mouse.h"

HINSTANCE hInst;
HWND hWndMain;
HBITMAP hSun, hMoon;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg;

  if (!InitApplication(hInstance))
    return FALSE;

  if (!InitInstance(hInstance, nCmdShow))
    return FALSE;

  hSun = LoadBitmap(hInstance, TEXT("Sun"));
  hMoon = LoadBitmap(hInstance, TEXT("Moon"));
  InvalidateRect(hWndMain, NULL, TRUE);

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
    wcex.style         = CS_DBLCLKS;                                // 支持鼠标双击
    wcex.lpfnWndProc   = (WNDPROC) MainWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, TEXT("MouseIcon"));
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName  = TEXT("MouseMenu");
    wcex.lpszClassName = TEXT("MouseWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("MouseSmallIcon"));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndMain = CreateWindow(
            TEXT("MouseWClass"),
            TEXT("使用鼠标示范程序"),
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
    DWORD fwKeys;
    POINT ptMouse;
    TCHAR szTitle[100];
    static RECT rect1, rect2;
    static HCURSOR hCursorArrow, hCursorPen;

    switch (message) {
        case WM_CREATE:
          rect1.left        = 90;
          rect1.right       = 138;
          rect1.top         = 8;
          rect1.bottom      = 56;

          rect2.left        = 90;
          rect2.right       = 138;
          rect2.top         = 78;
          rect2.bottom      = 126;

          hCursorPen        = LoadCursor(hInst, TEXT("rcCursor"));
          hCursorArrow      = LoadCursor(NULL, IDC_ARROW);

          break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MOUSEMOVE:
          ptMouse.x = LOWORD(lParam);
          ptMouse.y = HIWORD(lParam);
          fwKeys = wParam;

          /* 下面代码对鼠标进行位置范围测试。如果鼠标落在位图上，那么
             把鼠标指针设置为毛笔外观；否则，采用普通的箭头鼠标 */
          if (PtInRect(&rect1, ptMouse) || PtInRect(&rect2, ptMouse)) {
              SetCursor(hCursorPen);
          } else {
              SetCursor(hCursorArrow);
          }

          /* 在标题栏上显示鼠标的当前位置和按键信息 */
          wsprintf(szTitle, TEXT("鼠标测试程序鼠标位置[%d,%d]"), ptMouse.x,
                   ptMouse.y);
          if(fwKeys != 0) {
              lstrcat(szTitle, TEXT("按下了"));
              if(fwKeys & MK_CONTROL)
                  lstrcat(szTitle, TEXT("Ctrl"));
              if(fwKeys & MK_SHIFT)
                  lstrcat(szTitle, TEXT("Shift"));
              if(fwKeys & MK_LBUTTON)
                  lstrcat(szTitle, TEXT("鼠标左按钮"));
              if(fwKeys & MK_RBUTTON)
                  lstrcat(szTitle, TEXT("鼠标右按钮"));
              lstrcat(szTitle, TEXT("键"));
          }
          SetWindowText(hWndMain, szTitle);
          break;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
          /* 如果鼠标双击了窗口客户区，那么就地显示坐标信息 */
          hdc = GetDC(hWnd);

          SetTextColor(hdc, (message == WM_LBUTTONDBLCLK) ? RGB(255, 0, 0) : RGB(0, 0, 255));
          wsprintf(szTitle, TEXT("(%d, %d)"), LOWORD(lParam), HIWORD(lParam));
          TextOut(hdc, LOWORD(lParam), HIWORD(lParam), szTitle, lstrlen(szTitle));

          ReleaseDC(hWnd, hdc);
          break;

        case WM_PAINT:
          hdc = BeginPaint(hWnd, &ps);

          hMemDc = CreateCompatibleDC(hdc);
          SelectObject(hMemDc, hSun);
          BitBlt(hdc, 90, 8, 48, 48, hMemDc, 0, 0, SRCCOPY);
          DeleteDC(hMemDc);

          hMemDc = CreateCompatibleDC(hdc);
          SelectObject(hMemDc, hMoon);
          BitBlt(hdc, 90, 78, 48, 48, hMemDc, 0, 0, SRCCOPY);
          DeleteDC(hMemDc);

          EndPaint(hWnd, &ps);
          break;

        case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case IDM_EXIT:
              DeleteObject(hSun);
              DeleteObject(hMoon);
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

