/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * desktop.c: This file is part of ...
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
#include <tlhelp32.h>
#include "desktop.h"
#include "debug.h"
#include "utils.h"

#define REG_PATH      "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"
#define DEFAULT_SHELL "explorer.exe"

static BOOL getRegKey(HKEY hkey, const char* path, const char* key, char* value, size_t size)
{
    HKEY hKey;
    if(RegOpenKeyExA(hkey, path, 0, KEY_READ|KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        int iret;
        DWORD dwType = REG_SZ;
        iret = RegQueryValueExA(hKey, key, NULL, &dwType, (LPBYTE)value, (LPDWORD) &size);
        RegCloseKey(hKey);

        if(iret == 0) {
            value[size] = 0;
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL writeRegKey(HKEY hkey, const char* path, const char* key, const char* value)
{
    HKEY hKey;
    long lRet;

    if(RegOpenKeyExA(hkey, path, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        lRet = RegSetValueExA(hKey, key, 0, REG_SZ, (CONST BYTE *)value, strlen(value));

        RegCloseKey(hKey);
        if(lRet != ERROR_SUCCESS) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

static char* get_login_shell(void)
{
	char buf[1024] = {0};

	if (getRegKey(HKEY_LOCAL_MACHINE, REG_PATH, "Shell", buf, sizeof(buf))) {
        return strdup(buf);
	}
    return NULL;
}

static BOOL set_login_shell(const char* shell)
{

    return writeRegKey(HKEY_LOCAL_MACHINE, REG_PATH, "Shell", shell);
}

// 得到可执行文件的绝对路径
static char* get_myself_path(void)
{
    char modFileName[MAX_PATH] = {0};

    HINSTANCE hInstance;

    hInstance = GetModuleHandle(NULL);

    if (hInstance == NULL)
        return NULL;

    if (GetModuleFileNameA(hInstance, modFileName, MAX_PATH) != 0) {
        return strdup(modFileName);
    } else {
        return NULL;
    }
}

static void stop_desktop(void)
{
    //PROCESSENTRY32进程快照的结构体
    PROCESSENTRY32 pe;

    //实例化后使用Process32First获取第一个快照的进程前必做的初始化操作
    pe.dwSize = sizeof(PROCESSENTRY32);

    //kill进程from名字
    //创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    if(!Process32First(hSnapShot,&pe)) {
        return ;
    }

    //如果句柄有效  则一直获取下一个句柄循环下去
    while (Process32Next(hSnapShot,&pe)) {
        debug_print(L"%ls\n", pe.szExeFile);

        if (wcsicmp(pe.szExeFile, TEXT(DEFAULT_SHELL)) == 0) {
            debug_print(L"kill %ls ...\n", pe.szExeFile);
            //从快照进程中获取该进程的PID(即任务管理器中的PID)
            DWORD dwProcessID = pe.th32ProcessID;
            //OpenProcess 函数用来打开一个已存在的进程对象，并返回进程的句柄。
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
            TerminateProcess(hProcess,0);  //杀死进程
            CloseHandle(hProcess);
            return;
        }
    }
}

static BOOL start_desktop(void)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

	ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);
	ZeroMemory(&pi,sizeof(pi));

    // Start the child process.
    if(!CreateProcess(NULL,                    // No module name (use command line)
                      TEXT(DEFAULT_SHELL),     // Command line
                      NULL,                    // Process handle not inheritable
                      NULL,                    // Thread handle not inheritable
                      FALSE,                   // Set handle inheritance to FALSE
                      0,                       // No creation flags
                      NULL,                    // Use parent's environment block
                      NULL,                    // Use parent's starting directory 
                      &si,                     // Pointer to STARTUPINFO structure
                      &pi))                    // Pointer to PROCESS_INFORMATION structure
    {
        printf( "CreateProcess failed (%ld).\n", GetLastError());
        return FALSE;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	return TRUE;
}

void hide_desktop(void)
{
    char *myself = NULL;
	char *shell = NULL;

    shell = get_login_shell();
    if (shell == NULL) {
        return;
    }

    myself = get_myself_path();
    if (myself == NULL) {
        free (shell);
        return;
    }


    debug_print(L"get shell: %s", shell);

    if (strcmp(shell, DEFAULT_SHELL) == 0 ) {
        set_login_shell(myself);
    }
    free(myself);
    free(shell);
    stop_desktop();
}

void show_desktop(void)
{
    char *shell = NULL;

    start_desktop();
    shell = get_login_shell();
    if (shell == NULL) {
        return;
    }

    if (strcmp(shell, DEFAULT_SHELL) != 0) {
        set_login_shell(DEFAULT_SHELL);
        free(shell);
    }
}
