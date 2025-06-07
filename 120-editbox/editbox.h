/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * x.c: This file is part of ...
 *
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

//菜单命令
#define IDM_ABOUT 200
#define IDM_EXIT 300
#define IDM_UPDATE 301
#define IDM_SETWINPOSITION 762
#define IDM_SETWINCAPTION 763
#define IDM_NULL 752
#define IDM_EDITUNDO 770
#define IDM_EDITCUT 771
#define IDM_EDITCOPY 772
#define IDM_EDITPASTE 773
#define IDM_EDITDELETE 774
#define IDM_EDITSELECTALL 775

//对话框中的控件标识号
#define IDC_STATIC -1
#define IDE_PASSWORD 604
#define IDC_EDITWINCAPTION 1008
#define IDC_EDITWINLEFT 1010
#define IDC_EDITWINTOP 1011

//日主窗口中的编辑子窗口控件标识号
#define ID_EDITCONTROL 605 //函数原型说明

// 过程
LRESULT APIENTRY MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY PasswordDlgProc (HWND, UINT,WPARAM, LPARAM);
BOOL APIENTRY About (HWND,UINT, WPARAM,LPARAM);
BOOL APIENTRY SetWinPositionDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SetWinCaptionDlgProc (HWND, UINT, WPARAM, LPARAM);

// 函数
BOOL InitApplication (HINSTANCE);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
