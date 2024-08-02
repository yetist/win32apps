/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * utils.c: This file is part of ...
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
//#include <shlwapi.h>
//#include <winioctl.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <ctype.h>
#include <tchar.h>
#include "debug.h"

static BOOL getRegKey(const wchar_t* key, wchar_t* value, size_t size);
static BOOL writeRegKey(const wchar_t* key, const wchar_t* value);

void PrintError(TCHAR* msg)
{
    DWORD eNum;
    TCHAR sysMsg[BUFSIZ] = {0};
    TCHAR showmsg[BUFSIZ] = {0};
    TCHAR* p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, eNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  sysMsg, 256, NULL );

    /* 去掉末尾的换行符号 */
    p = sysMsg;
    while(p && (*p)) ++p;

    do {
        *p-- = '\0';
    } while(( p >= sysMsg ) && (( *p == '\r' ) || (*p == '\n')));

    // Display the message
    snwprintf(showmsg, sizeof(showmsg), TEXT("%s\n[ErrorID=%ld]%s"), msg, eNum, sysMsg);
    //SendMessage(GetDlgItem(hwnd, IDC_MAIN_STATUS), SB_SETTEXT, 0, showmsg);
    debug_dialog_error(showmsg);
}

// 得到可执行文件的绝对路径
wchar_t* get_self_module_path(void)
{
    wchar_t modFileName[MAX_PATH] = {0};

    HINSTANCE hInstance;

    hInstance = GetModuleHandle(NULL);

    if (hInstance == NULL)
        return NULL;

    if (GetModuleFileName(hInstance, modFileName, MAX_PATH) != 0) {
        debug_print(modFileName);
        return wcsdup(modFileName);
    } else {
        return NULL;
    }
}

wchar_t* get_login_shell(void)
{
	wchar_t buf[1024] = {0};

	if (getRegKey(TEXT("Shell"), (wchar_t*) buf, sizeof(buf))) {
		if (wcslen(buf) > 0) {
            return wcsdup(buf);
		}
	}
    return NULL;
    ///
}

BOOL set_login_shell(const wchar_t *shell)
{
    return writeRegKey(TEXT("Shell"), shell);
}

static BOOL getRegKey(const wchar_t* key, wchar_t* value, size_t size)
{
    HKEY hKey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"),
                    0,
                    KEY_READ|KEY_WRITE,
                    &hKey
                   ) == ERROR_SUCCESS) {
        int iret;
        DWORD dwType = REG_SZ;
        iret = RegQueryValueEx(hKey, (LPCWSTR)key, NULL, &dwType, (LPBYTE)value, (LPDWORD) &size);
        RegCloseKey(hKey);

        if(iret == 0) {
            value[size] = 0;
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL writeRegKey(const wchar_t* key, const wchar_t* value)
{
    HKEY hKey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"),
                    0,
                    KEY_WRITE,
                    &hKey
                   ) == ERROR_SUCCESS) {
        long lRet;
        lRet = RegSetValueEx(hKey, key, 0, REG_SZ, (CONST BYTE *)value, wcslen(value));

        RegCloseKey(hKey);
        if(lRet != ERROR_SUCCESS) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
