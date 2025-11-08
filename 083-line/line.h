/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#pragma once

// 函 数 原 型 说 明
// 过 程
LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam);

// 函 数
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
