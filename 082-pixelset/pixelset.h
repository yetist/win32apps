/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

#pragma once

//菜单命令
//【文件】菜单

#define IDM_ZOOMIN  100
#define IDM_EXIT   101
//【帮助】菜单
#define IDM_ABOUT  200

#define IDC_STATIC -1

//函数说明

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam);
BOOL APIENTRY About(HWND, UINT, WPARAM, LPARAM);

//函数
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

int DrawMandelbrotSet(HDC hdc, int xMin, int yMin, int xMax, int yMax);
int DrawMyRect (HDC hdc, int x0, int y0, int x1, int y1);

//变量说明
HINSTANCE hInst;
HWND hWndMain;
double PMin=-2.5,PMax=0.9,QMin=-1.0,QMax=1.0;
double newPMin, newPMax, newQMin, newQMax;
