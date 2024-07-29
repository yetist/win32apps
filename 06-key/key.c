/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */

#include <windows.h>
#include <string.h>
#include "key.h"

HINSTANCE hInst;
HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg;

  if (!InitApplication(hInstance))
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
  WNDCLASSEX wcexKeyApp;

  wcexKeyApp.cbSize        = sizeof(WNDCLASSEX);
  wcexKeyApp.style         = 0;
  wcexKeyApp.lpfnWndProc   = (WNDPROC) MainWndProc;
  wcexKeyApp.cbClsExtra    = 0;
  wcexKeyApp.cbWndExtra    = 0;
  wcexKeyApp.hInstance     = hInstance;
  wcexKeyApp.hIcon         = LoadIcon(hInstance, TEXT("KeyInputIcon"));
  wcexKeyApp.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcexKeyApp.hbrBackground = GetStockObject(WHITE_BRUSH);
  wcexKeyApp.lpszMenuName  = TEXT("KeyInputMenu");
  wcexKeyApp.lpszClassName = TEXT("KeyInputWClass");
  wcexKeyApp.hIconSm       = LoadIcon(hInstance, TEXT("KeyInputSmallIcon"));

  return RegisterClassEx(&wcexKeyApp);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance;

  hWndMain = CreateWindow(TEXT("KeyInputWClass"),
                          TEXT("键盘输入示范程序    [插入]"),
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

#define MAXBUFSIZE 65535

LRESULT APIENTRY MainWndProc(HWND hwndMain,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
  HDC hdc;
  TEXTMETRIC tm;
  static HWND hBmpCaret;
  static DWORD dwCharY;
  static DWORD dwClientX;
  static DWORD dwClientY;
  static int nCaretPosX = 0;
  static int nCaretPosY = 0;
  static int nCharWidth = 0;
  static int cch = 0;
  static int nCurChar = 0;
  static PTCHAR pchInputBuf;
  static BOOL fInsert = TRUE;
  int i;
  TCHAR ch;
  RECT rc;
  PAINTSTRUCT ps;

  switch (message) {
    case WM_CREATE:
      /* 检取当前字体的度量数据 */
      hdc = GetDC(hwndMain);
      GetTextMetrics(hdc, &tm);
      ReleaseDC(hwndMain, hdc);

      /* 保存字符的高度 */
      dwCharY = tm.tmHeight;

      /* 分配用于保存键盘输入字符的内存缓冲区 */
      pchInputBuf = (LPTSTR) GlobalAlloc(GPTR, MAXBUFSIZE * sizeof(TCHAR));

      /* 预先输入 "Hello", 并加回车符 */
      SendMessage(hwndMain, WM_CHAR, 0x48, 0);
      SendMessage(hwndMain, WM_CHAR, 0x65, 0);
      SendMessage(hwndMain, WM_CHAR, 0x6c, 0);
      SendMessage(hwndMain, WM_CHAR, 0x6c, 0);
      SendMessage(hwndMain, WM_CHAR, 0x6f, 0);
      SendMessage(hwndMain, WM_CHAR, 0x21, 0);
      SendMessage(hwndMain, WM_CHAR, 0x0d, 0);
      return 0;
    case WM_SIZE:
      /* 保存客户区的新的宽度和高度 */
      dwClientX = LOWORD(lParam);
      dwClientY = HIWORD(lParam);
      break;
    case WM_SETFOCUS:
      /* 当窗口拥有键盘输入焦点时，创建、定位并显示插入符 */
      /* 装入应用程序定义的插入符位图资源 */
      hBmpCaret = (HWND) LoadBitmap(hInst, TEXT("Caret"));
      /* 创建插入符 */
      CreateCaret(hwndMain, (HBITMAP) hBmpCaret, 0, 0);
      /* 调整插入符的位置 */
      SetCaretPos(nCaretPosX, nCaretPosY * dwCharY);
      /* 显示插入符 */
      ShowCaret(hwndMain);
      break;
    case WM_KILLFOCUS:
      HideCaret(hwndMain);
      DestroyCaret();
      break;
    case WM_CHAR:
      switch (wParam)
        {
        case 0x0A: /* linefeed */
        case VK_ESCAPE: //0x1B:
          MessageBeep(0xFFFFFFFF);
          return 0;
        case VK_BACK:   /* 退格字符 */
          if (nCurChar > 0)
            {
              DeleteCharFromBuffer(pchInputBuf, --nCurChar, cch--);
              GetCaretPosFromIndex(nCurChar, pchInputBuf, &nCaretPosX, &nCaretPosY);
            }
          break;
        case VK_TAB:   /* 制表字符 */
          /* 把制表符转换为  8 个连续空格字符 */
          for (i = 0; i < 8; i++)
            SendMessage(hwndMain, WM_CHAR, VK_SPACE/*0x20*/, 0);
          return 0;
        case VK_RETURN: //0x0D:  /* 回车换行字符  */
          /* 记录回车换行符，并把插入符定位到下一新行的起始处 */
          InsertCharToBuffer(VK_RETURN, pchInputBuf, nCurChar++, cch++);
          nCaretPosX = 0;
          nCaretPosY += 1;
          break;
        default:    /* 可显示字符 */
          ch = (TCHAR) wParam;
          HideCaret(hwndMain);

          /* 检取字符宽度并输出字符 */
          hdc = GetDC(hwndMain);
          GetCharWidth(hdc, (UINT) wParam, (UINT) wParam, &nCharWidth);
          ReleaseDC(hwndMain, hdc);

          /* 把字符保存到缓冲区中 */
          if (fInsert)
            InsertCharToBuffer(ch, pchInputBuf, nCurChar++, cch++);
          else
            pchInputBuf[nCurChar++] = ch;

          /* 计算新的插入twfy水平位置 */
          nCaretPosX += nCharWidth;
          ShowCaret(hwndMain);
          break;
        }
      SetCaretPos(nCaretPosX, nCaretPosY * dwCharY);
      InvalidateRect(hwndMain, NULL, TRUE);
      break;
    case WM_KEYDOWN:
      switch(wParam)
        {
        case VK_LEFT:   /* 左箭头 */
          /* 插入符向左移动；如果已经是当前行的起始处，并且不是第一行，那么就移动到上一行的结尾处 */
          if (nCaretPosX >= 0)
            {
              HideCaret(hwndMain);
              /* 检取插入符左侧的字符，计算其宽度，然后用插入符的当前水平位置送去这个宽度，就得出插入符的新的水平位置。 */
              if (nCurChar > 0)
                nCurChar--;
              else if (pchInputBuf[nCurChar-1] == 0x0D)
                nCurChar--;
              GetCaretPosFromIndex(nCurChar, pchInputBuf, &nCaretPosX, &nCaretPosY);
              ShowCaret(hwndMain);
            }
          break;
        case VK_RIGHT: /* 右箭头 */
          /* 插入符右移一个字符位置，如果遇上回车换行符，则移动下一行起始处 */
          if (nCurChar < cch) {
              HideCaret(hwndMain);

              /* 检取插入符右侧的字符，如果该字符是回车换行符，那么就把插入符定位到下一行的起始处 */
              ch = pchInputBuf[nCurChar];
              if (ch == 0x0D)
                {
                  nCaretPosX = 0;
                  nCaretPosY++;
                } else {
                    hdc = GetDC(hwndMain);
                    /* 检取该字符的宽度，计算插入符新的水平位置  */
                    GetCharWidth(hdc, ch, ch, &nCharWidth);
                    ReleaseDC(hwndMain, hdc);
                    nCaretPosX = nCaretPosX + nCharWidth;
                }
              nCurChar++;
              ShowCaret(hwndMain);
              break;
          }
          break;
        case VK_UP: /* 上箭头 */
        case VK_DOWN: /* 下箭头 */
          MessageBeep(0xFFFFFFFF);
          return 0;
        case VK_HOME:
          /* 把插入符设置到客户区左上角位置  */
          nCaretPosX = nCaretPosY = 0;
          nCurChar = 0;
          break;
        case VK_END:
          /* 把插入符移动到文字的结尾处 */
          nCurChar = cch;
          GetCaretPosFromIndex(nCurChar, pchInputBuf, &nCaretPosX, &nCaretPosY);
          break;
        case VK_DELETE:
          if(nCurChar < cch)
            {
              DeleteCharFromBuffer(pchInputBuf, nCurChar, cch--);
              InvalidateRect(hwndMain, NULL, TRUE);
              GetCaretPosFromIndex(nCurChar, pchInputBuf, &nCaretPosX, &nCaretPosY);
            }
          break;
        case VK_INSERT:
          fInsert = !fInsert;
          if(fInsert)
            SetWindowText(hwndMain, TEXT("键盘输入示范程序   [输入]"));
          else
            SetWindowText(hwndMain, TEXT("键盘输入示范程序   [改写]"));
          break;
        default:
          break;
        }
      SetCaretPos(nCaretPosX, nCaretPosY * dwCharY);
      break;
    case WM_PAINT:
      if (cch == 0)   /* 如果缓冲区中没有字符，则跳出 */
        break;

      hdc = BeginPaint(hwndMain, &ps);
      HideCaret(hwndMain);

      /* 设置剪裁矩形区域，然后把文字绘制到其中  */
      SetRect(&rc, 0, 0, dwClientX, dwClientY);
      DrawText(hdc, pchInputBuf, -1, &rc, DT_LEFT);

      ShowCaret(hwndMain);
      EndPaint(hwndMain, &ps);
      break;

    case WM_COMMAND:
      switch(LOWORD(wParam))
        {
        case IDM_EXIT:
          SendMessage(hwndMain, WM_CLOSE, 0, 0);
          break;
        case IDM_ABOUT:
          DialogBox(hInst, TEXT("AboutBox"), hwndMain, (DLGPROC) About);
          break;
        default:
          return DefWindowProc(hwndMain, message, wParam, lParam);
        }
      break;
    case WM_DESTROY:
      /* 释放输入缓冲区 */
      GlobalFree((HGLOBAL) pchInputBuf);
      DeleteObject(hBmpCaret);
      PostQuitMessage(0);
      break;
    default:
      // 调用默认窗口过程对未处理的消息进行必要的处理
      return DefWindowProc(hwndMain, message, wParam, lParam);
  }
  return 0L;
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

VOID InsertCharToBuffer (TCHAR ch, PTCHAR szBuffer, UINT index, UINT total)
{
  register UINT i;
  for (i = total; i>index; i--)
    szBuffer[i+1] = szBuffer[i];
  szBuffer[index] = ch;
  return;
}

VOID DeleteCharFromBuffer(PTCHAR szBuffer, UINT index, UINT total)
{
  register UINT i;
  for (i = index; i<total; i++)
    szBuffer[i] = szBuffer[i+1];
  szBuffer[total] = '\n';
  return;
}

VOID GetCaretPosFromIndex(UINT index, PTCHAR szBuffer, int *nCaretPosX, int *nCaretPosY)
{
  HDC hDC;
  UINT i, j;
  int cCR = 0;
  int nCRIndex = 0;
  TCHAR szBuf[128];
  SIZE sz;

  for (i = 0; i < index; i++) {
      /* 计算回车换行符的个数，并保存最近一个回车换行符的索引号 */
      if (szBuffer[i] == 0x0D) {
          cCR++;
          nCRIndex = i+1;
      }
  }

  *nCaretPosY = cCR;

  /* 把最近的回车换行符与当前字符之间的文字全部复现到一个临时缓冲区中 */
  for(i = nCRIndex, j = 0; i<index; i++, j++)
    szBuf[j] = szBuffer[i];
  szBuf[j] = TEXT('\0');

  /* 检取临时缓冲区中文字字符串的宽度，并依此来设置插入符的水平宽度 */
  hDC = GetDC(hWndMain);
  GetTextExtentPoint32(hDC, szBuf, lstrlen(szBuf), &sz);
  *nCaretPosX = sz.cx;
  ReleaseDC(hWndMain, hDC);
}
