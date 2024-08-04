/*
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

#ifndef __DEBUG_H__
#define __DEBUG_H__ 1

#include <windows.h>
#include <stdio.h>

BOOL DebugPrint(const char* file, int line, const char* func, const wchar_t *fmt, ...);
int debug_dialog_info(const wchar_t* fmt, ...);
int debug_dialog_warn(const wchar_t* fmt, ...);
int debug_dialog_error(const wchar_t* fmt, ...);
int debug_dialog_yesno(const wchar_t* fmt, ...);

#ifdef _DEBUG
#	define debug_print(format...) DebugPrint(__FILE__, __LINE__, __FUNCTION__, ##format);
#else
#	define debug_print(format...)
#endif

#endif /* end of include guard __DEBUG_H__ */
