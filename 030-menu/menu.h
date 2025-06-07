/* vi: set sw=4 ts=4: */
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
#include <afxres.h>

#ifndef __MENU_H__
#define __MENU_H__ 1

// 文件菜单
#define IDM_EXIT            200

// 编辑菜单
#define IDM_EDITUNDO        210
#define IDM_EDITCUT         211
#define IDM_EDITCOPY        212
#define IDM_EDITPASTE       213
#define IDM_EDITDELETE      214
#define IDM_EDITSELECTALL   215

// 查看菜单
#define IDM_SHOWTOOLBAR     220
#define IDM_SHOWSTATUSBAR   221

// 格式菜单
#define IDM_ALIGNLEFT       230
#define IDM_ALIGNCENTER     231
#define IDM_ALIGNRIGHT      232

// 字体菜单
#define IDM_SELECTFONTFACE  240
#define IDM_SMALLFONTSIZE   250
#define IDM_NORMALFONTSIZE  251
#define IDM_LARGEFONTSIZE   252

// 帮助菜单
#define IDM_ABOUT           300

#define IDM_BOLD            100
#define IDM_ITALIC          101
#define IDM_UNDERLINE       102
#define IDM_STRIKEOUT       103

// 全局变量说明
HINSTANCE hInst;

//函数原型说明

// 过程
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

// 函数
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

VOID InitAlignMenu(HWND hWnd);

#endif /* __MENU_H__ */

/*
vi:ts=4:wrap:ai:
*/
