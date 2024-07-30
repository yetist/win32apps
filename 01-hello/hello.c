/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * hello.c: This file is part of ...
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

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int CDECL MessageBoxPrintf (TCHAR * szCaption, TCHAR * szFormat, ...)
{
    TCHAR szBuffer [1024] ;
    va_list pArgList ;
    // The va_start macro (defined in STDARG.H) is usually equivalent to:
    // pArgList = (char *) &szFormat + sizeof (szFormat) ;
    va_start (pArgList, szFormat) ;
    // The last argument to wvsprintf points to the arguments
    _vsntprintf ( szBuffer, sizeof (szBuffer) / sizeof (TCHAR),
                 szFormat, pArgList) ;
    // The va_end macro just zeroes out pArgList for no good reason
    va_end (pArgList) ;
    return MessageBox (NULL, szBuffer, szCaption, 0) ;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    int cxScreen, cyScreen ;
    cxScreen = GetSystemMetrics (SM_CXSCREEN) ;
    cyScreen = GetSystemMetrics (SM_CYSCREEN) ;
    MessageBoxPrintf ( TEXT ("ScrnSize"),
                      TEXT ("The screen is %i pixels wide by %i pixels high."),
                      cxScreen, cyScreen) ;
    return 0 ;
}
