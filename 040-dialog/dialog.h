/*
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

#ifndef __DIALOG_H__
#define __DIALOG_H__ 1


// 文件菜单
#define IDM_OPENMODELESS  100
#define IDM_OPENMODEL	  101
#define IDM_EXIT	  102

// 帮助菜单
#define IDM_ABOUT	  200

// 对话框中的控件标识号
#define IDD_RED	    400
#define IDD_BLUE    401
#define IDD_DAY	    402
#define IDD_NIGHT   403
#define IDC_STATIC  -1


// 函数原型说明

// 过程
LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY TextDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY TextDlg2Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

UINT fIsRed = 1;
UINT fIsDay = 1;
UINT fIsNight = 0;
TCHAR *szDay = TEXT("天上有个太阳");
TCHAR *szNight = TEXT("水中有个月亮");

#endif /* __DIALOG_H__ */

