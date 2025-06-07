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

#ifndef __LISTBOX_H__
#define __LISTBOX_H__ 1

#define IDM_EXIT                100
#define IDM_ABOUT               110

#define IDM_SELECTFONTFACE      200
#define IDM_SELECTFONTSTYLE     300
#define IDM_SELECTFONTORIENT    400
#define IDM_SELECTFONTCOLOR     500
#define IDM_SIZE                800

#define IDL_FONTFACE            600
#define IDL_FONTSTYLE           601
#define IDL_FONTORIENT          602
#define IDL_FONTCOLOR           603

#define IDC_STATIC              -1
#define IDC_FONTSAMPLE          2000
#define IDM_NULL                1000
#define NUM_POINTS              16

typedef struct tagTrueTypeFontInfo {
    LOGFONT         *plf;
    TEXTMETRIC      *ptm;
} TRUETYPEFONTINFO, *PTRUETYPEFONTINFO;


LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam);
BOOL InitApplication(HINSTANCE hInstance);
BOOL APIENTRY SelectFontFaceDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam);
BOOL APIENTRY SelectFontStyleDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam);
BOOL APIENTRY SelectFontOrientDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam);
BOOL APIENTRY SelectFontColorDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam);



int APIENTRY MyEnumFontFacesProc(LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPVOID lpData);
int APIENTRY MyEnumFontCountProc(LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPINT lpData);


BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
PTRUETYPEFONTINFO BuildFontList(HDC hDCFont, LPINT retnFaces);

VOID FillMenu(HWND hWnd);
BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif /* end of include guard __LISTBOX_H__ */

