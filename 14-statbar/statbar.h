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

#ifndef __STATBAR_H__
#define __STATBAR_H__ 1

#define IDM_EXIT                100
#define IDM_ABOUT               110

#define IDM_EDITUNDO     200
#define IDM_EDITCUT      201

#define IDM_EDITCOPY 202
#define IDM_EDITPASTE 203
#define IDM_EDITDELETE 204
#define IDM_EDITSELECTALL 205

//显示菜单
# define IDM_SHOWSTATUSBAR 300

// 格式菜单
#define IDM_BOLD 400
#define IDM_ITALIC 401
#define IDM_UNDERLINE 402
#define IDM_STRIKEOUT 403
#define IDM_ALIGNLEFT 405
#define IDM_ALIGNCENTER 406
#define IDM_ALIGNRIGHT 407

// 【字体】菜单
#define IDM_SELECTFONTFACE 500
#define IDM_SELECTFONTSIZE 501

//对话框中的控件标识号
#define IDC_STATIC -1
#define IDC_FONTFACE 610
#define IDC_FONTSIZE 614

//多文本编辑窗口标识号
#define ID_RICHEDIT 1000
//状态栏标识号
#define ID_STATUSBAR 1001

//其他常量
# define NUM POINTS 16

//保存系统中可用的True Type字体信息的结构
typedef struct tagTrueTypeFontInfo {
    LOGFONT *plf;
    TEXTMETRIC *ptm;
} TRUETYPEFONTINFO, *PTRUETYPEFONTINFO;

//函数原型说明
// 过程
LRESULT APIENTRY MainWndProc (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY PasswordDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY About (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SelectFontFaceDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SelectFontSizeDlgProc (HWND, UINT, WPARAM, LPARAM);
// 字体枚举回调函数
int APIENTRY MyEnumFontFacesProc (LPLOGFONT, LPTEXTMETRIC, DWORD,
                                  LPVOID);
int APIENTRY MyEnumFontCountProc (LPLOGFONT, LPTEXTMETRIC, DWORD,
                                   LPINT);
//其他函数
BOOL InitApplication (HINSTANCE);
BOOL InitInstance (HINSTANCE, int);
PTRUETYPEFONTINFO BuildFontList (HDC, LPINT);
VOID ChangeFaceName (HWND, LPTSTR);
VOID ChangePointSize (HWND, int);
VOID HandleAlignCommand (HWND, HWND, int);
VOID UpdateCaretPositionState (HWND, HWND);
VOID UpdateAlignState (HWND, HWND, HWND);
VOID UpdateCharFormatState (HWND, HWND, HWND);
void HandleCharEffectCommand (HWND, UINT);

#endif /* __STATBAR_H__ */
