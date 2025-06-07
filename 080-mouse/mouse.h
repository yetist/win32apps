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

#ifndef __MOUSE_H__
#define __MOUSE_H__ 1

#define IDM_OPEN    100
#define IDM_EXIT    101

#define IDM_ABOUT   200

#define MYICON      300
#define SMALL_ICON  301

#define IDC_STATIC  -1

// 过程
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif /* __MOUSE_H__ */

