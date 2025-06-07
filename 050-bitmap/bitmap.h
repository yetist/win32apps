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

#ifndef __BITMAP_H__
#define __BITMAP_H__ 1


#define IDM_OPEN    100
#define IDM_EXIT    101
#define IDM_ABOUT   200

#define IDD_RED	    400
#define IDD_BLUE    401
#define IDD_DAY	    402
#define IDD_NIGHT   403
#define IDC_STATIC  -1

#define IDS_SUN     500
#define IDS_MOON    501

LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY TextDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

UINT fIsRed = 1;
UINT fIsDay = 1;
UINT fIsNight = 1;

#endif /* __BITMAP_H__ */

