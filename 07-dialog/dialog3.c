/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "resource.h"

INT_PTR CALLBACK DlgProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC) DlgProc);
    return 0;
}

INT_PTR CALLBACK DlgProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return (INT_PTR)TRUE;
        case WM_COMMAND:
            {
                switch(LOWORD(wParam))
                {
                    case IDC_BUTTON_HELLO:
                        MessageBox(NULL, TEXT("Hello, world!"), TEXT("Tips"), MB_OK);
                        return (INT_PTR)TRUE;
                }
            }
    }
    return (INT_PTR) FALSE;
}
