/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#pragma once

// 菜 单 和 令

// 〖 文 件 ] 荷 单
#define IDM_OPEN 100
#define IDM_EXIT 101

// 【 帮 助 ] 深 单
#define IDM_ABOUT 200

// 对话框
#define IDC_STATIC -1

// 函数原型说明

// 过程

LRESULT APIENTRY MainWndProc(HWND, UINT,WPARAM , LPARAM);
BOOL APIENTRY About(HWND, UINT, WPARAM, LPARAM);

BOOL APIENTRY TextDlgProc(HWND, UINT, WPARAM, LPARAM);

// 函数
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
