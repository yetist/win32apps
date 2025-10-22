/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 yetist <yetist@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int CDECL MessageBoxPrintf (LPCWSTR szCaption, LPCWSTR szFormat, ...)
{
    TCHAR szBuffer [1024];
    va_list pArgList;
    // The va_start macro (defined in STDARG.H) is usually equivalent to:
    // pArgList = (char *) &szFormat + sizeof (szFormat) ;
    va_start (pArgList, szFormat) ;
    // The last argument to wvsprintf points to the arguments
    vsnwprintf (szBuffer, sizeof (szBuffer) / sizeof (TCHAR),
                 szFormat, pArgList);
    // The va_end macro just zeroes out pArgList for no good reason
    va_end (pArgList) ;
    return MessageBoxW(NULL, szBuffer, szCaption, 0) ;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    int cxScreen, cyScreen ;
    cxScreen = GetSystemMetrics (SM_CXSCREEN) ;
    cyScreen = GetSystemMetrics (SM_CYSCREEN) ;
    MessageBoxPrintf (TEXT("你ScrnSize"),
                      TEXT("好The screen is %i pixels wide by %i pixels high."),
                      cxScreen, cyScreen) ;
    return 0 ;
}
