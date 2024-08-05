/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * debug.c: This file is part of ...
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

/* This file muse define the follow string tabes:
    IDS_DIALOG_INFO
    IDS_DIALOG_ERROR
    IDS_DIALOG_WARNING
    IDS_DIALOG_QUESTION
*/

#include <stdarg.h>
#include "resource.h"
#include "debug.h"

BOOL DebugPrint(const char* file, int line, const char* func, const wchar_t *fmt, ...)
{
	va_list ap;
	wchar_t msg[BUFSIZ] = {0};
	wchar_t buf[BUFSIZ] = {0};
    HANDLE g_hOutput = 0;   //标准输出句柄

	va_start(ap, fmt);
	vsnwprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	snwprintf(buf, sizeof(buf), TEXT("%s:%d %s(): %ls"), file, line, func, msg);
    wprintf(TEXT("DEBUG: %ls"), buf);
    return WriteConsole(g_hOutput, (CONST VOID *)buf, wcslen(buf), NULL, NULL);
}

static int DebugMessageBox(UINT type, const wchar_t* title, const wchar_t *fmt, ...)
{
	va_list ap;
	wchar_t text[BUFSIZ] = {0};

	va_start(ap, fmt);
	vsnwprintf(text, sizeof(text), fmt, ap);
	va_end(ap);

	return MessageBox(NULL, text, title, type);
}

#define DEFINE_DEBUG_DIALOG(fname, msgid, btnid, iconid)                    \
int debug_dialog_##fname (const wchar_t* fmt, ...)                          \
{                                                                           \
    HMODULE self;                                                           \
    va_list ap;	                                                            \
    wchar_t msg[BUFSIZ] = {0};	                                            \
    wchar_t caption[BUFSIZ] = {0};	                                        \
    self = GetModuleHandle(NULL);                                           \
    LoadString(self, IDS_DIALOG_##msgid, caption, sizeof(caption));	        \
    va_start(ap, fmt);	                                                    \
    vsnwprintf(msg, sizeof(msg), fmt, ap);	                                \
    va_end(ap);	                                                            \
    return DebugMessageBox(MB_##btnid|MB_##iconid, caption, msg);           \
}

DEFINE_DEBUG_DIALOG(info,  INFO,     OK,    ICONINFORMATION);
DEFINE_DEBUG_DIALOG(warn,  WARNING,  OK,    ICONWARNING);
DEFINE_DEBUG_DIALOG(error, ERROR,    OK,    ICONERROR);
DEFINE_DEBUG_DIALOG(yesno, QUESTION, YESNO, ICONQUESTION);
