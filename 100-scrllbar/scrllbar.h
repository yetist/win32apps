/*
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

#ifndef __SCRLLBAR_H__
#define __SCRLLBAR_H__ 1

#define IDM_ABOUT       200
#define IDM_DRAW        201
#define IDM_RGB         202
#define IDM_EXIT        203

// 对话框标识符
#define IDC_RED         500
#define IDC_GREEN       501
#define IDC_BLUE        502

#define IDC_VALRED      505
#define IDC_VALGREEN    506
#define IDC_VALBLUE     507

#define IDC_STATIC      -1

// 全局变量
HINSTANCE hInst;
HWND hWndMain;
int crRGB[3];

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY RGBDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif /* end of include guard __SCRLLBAR_H__ */

