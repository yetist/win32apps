/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */


#ifndef __TIMER_H__
#define __TIMER_H__ 1

#define IDC_SUN         1000
#define IDC_MOON        1001
#define IDC_TIMECOUNTER 1002
#define IDC_STARTTIMER  1003
#define IDC_STATIC      -1


#define MYTIMER         1000
#define SUNTIMER        2000

INT_PTR CALLBACK TimerDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK MyTimerProc(HWND hWnd, UINT message, UINT idTimer, DWORD dwTime);
#endif /* __TIMER_H__ */
