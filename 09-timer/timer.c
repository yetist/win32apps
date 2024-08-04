/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */

#include <windows.h>
#include <string.h>
#include "timer.h"

UINT Counter = 0;
HINSTANCE hInst;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;

    DialogBox(hInstance, (LPCTSTR) TEXT("TimerDlgBox"), NULL, (DLGPROC) TimerDlgProc);
    return FALSE;
}

// 处理对话框消息
INT_PTR CALLBACK TimerDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static BOOL fShowSun = TRUE;
    static BOOL fStartTimer = TRUE;
    static HWND hSun, hMoon;

    switch(msg) {
      case WM_INITDIALOG:
        hSun = GetDlgItem(hDlg, IDC_SUN);
        hMoon = GetDlgItem(hDlg, IDC_MOON);
        SetTimer(hDlg, SUNTIMER, 1000, (TIMERPROC) NULL);
        return TRUE;
      case WM_TIMER:
          if(fShowSun) {
              ShowWindow(hMoon, SW_HIDE);
              ShowWindow(hSun, SW_SHOW);
          } else {
              ShowWindow(hMoon, SW_SHOW);
              ShowWindow(hSun, SW_HIDE);
          }
          fShowSun = !fShowSun;
          break;
      case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case IDC_STARTTIMER:
              if(fStartTimer) {
                  SetDlgItemText(hDlg, IDC_STARTTIMER,
                                 TEXT("停止下面的计时器"));
                  SetTimer(hDlg, MYTIMER, 300, (TIMERPROC) MyTimerProc);
              } else {
                  KillTimer(hDlg, MYTIMER);
                  SetDlgItemText(hDlg, IDC_STARTTIMER,
                                 TEXT("启动下面的计时器"));
                  Counter = 0;
              }
              fStartTimer = !fStartTimer;
              break;
            case IDCANCEL:
              KillTimer(hDlg, SUNTIMER);
              if(!fStartTimer)
                  KillTimer(hDlg, MYTIMER);
              EndDialog(hDlg, TRUE);
              return TRUE;
          }
          break;
    }
    return FALSE;
}

VOID CALLBACK MyTimerProc(HWND hWnd, UINT message, UINT idTimer, DWORD dwTime)
{
    SetDlgItemInt(hWnd, IDC_TIMECOUNTER, ++Counter, FALSE);
}
