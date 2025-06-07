/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */

#ifndef __KEY_H__
#define __KEY_H__ 1

#define IDM_OPEN    100
#define IDM_EXIT    101

#define IDM_ABOUT   200

#define IDC_STATIC  -1

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID InsertCharToBuffer (TCHAR ch, PTCHAR szBuffer, UINT index, UINT total);
VOID DeleteCharFromBuffer(PTCHAR szBuffer, UINT index, UINT total);
VOID GetCaretPosFromIndex(UINT index, PTCHAR szBuffer, int *nCaretPosX, int *nCaretPosY);
LRESULT APIENTRY MainWndProc(HWND hwndMain, UINT message, WPARAM wParam, LPARAM lParam);

#endif /* __KEY_H__ */
