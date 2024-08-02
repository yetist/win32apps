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

#include "desktop.h"
#include "debug.h"
#include "utils.h"

#if 0
BOOL setLogonShell(const char* value);

void show_desktop(void);
void disable_ctrl_alt_del(void);
#endif
BOOL set_login_shell(const wchar_t *shell);

void hide_desktop(void)
{
    wchar_t *shell = NULL;
    TCHAR* self_path = NULL;

    self_path = get_self_module_path();

    shell = get_login_shell();

    debug_print(shell);
    debug_print(self_path);
    debug_dialog_info(shell);
    debug_dialog_info(self_path);

    if (shell != NULL && wcscmp(shell, TEXT("explorer.exe")) == 0 ) {
        debug_print(shell);
        if (self_path != NULL) {
            set_login_shell(self_path);
            free(self_path);
        }
        free(shell);
    }
}

void show_desktop(void)
{
    wchar_t *shell = NULL;

    shell = get_login_shell();

    if (shell != NULL && wcscmp(shell, TEXT("explorer.exe")) == 1 ) {
        debug_print(shell);
        set_login_shell(TEXT("explorer.exe"));
        free(shell);
    }
}

#if 0
void kill_desktop(void)
{
    //PROCESSENTRY32进程快照的结构体
    PROCESSENTRY32 pe;

    //实例化后使用Process32First获取第一个快照的进程前必做的初始化操作
    pe.dwSize = sizeof(PROCESSENTRY32);

    //kill进程from名字
    //创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    //下面的IF效果同:
    //if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄
    if(!Process32First(hSnapShot,&pe)) {
        return ;
    }

    //将字符串转换为小写
    m_ProcessName.MakeLower();

    //如果句柄有效  则一直获取下一个句柄循环下去
    while (Process32Next(hSnapShot,&pe)) {
        //pe.szExeFile获取当前进程的可执行文件名称
        CString scTmp = pe.szExeFile;

        //将可执行文件名称所有英文字母修改为小写
        scTmp.MakeLower();

        //比较当前进程的可执行文件名称和传递进来的文件名称是否相同
        //相同的话Compare返回0
        if(!scTmp.Compare(m_ProcessName)) {
            if(MessageBox(str,
                          TEXT("Are you sure ？"),
                          MB_YESNO) != IDYES) {
                return;
            }

            //从快照进程中获取该进程的PID(即任务管理器中的PID)
            DWORD dwProcessID = pe.th32ProcessID;
            //OpenProcess 函数用来打开一个已存在的进程对象，并返回进程的句柄。
            HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
            ::TerminateProcess(hProcess,0);  //杀死进程
            CloseHandle(hProcess);
            return;
        }
        scTmp.ReleaseBuffer();
    }
    m_ProcessName.ReleaseBuffer();
}


#include "TlHelp32.h"

void CKillProcessDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //OnOK();
    UpdateData(TRUE);
    CString str;
    str.Format(TEXT("确定要杀死进程：%s?"),m_ProcessName.GetBuffer());
    //kill进程from名字
    //创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    //PROCESSENTRY32进程快照的结构体
    PROCESSENTRY32 pe;

    //实例化后使用Process32First获取第一个快照的进程前必做的初始化操作
    pe.dwSize = sizeof(PROCESSENTRY32);


    //下面的IF效果同:
    //if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄
    if(!Process32First(hSnapShot,&pe))
        {
        return ;
        }

    //将字符串转换为小写
    m_ProcessName.MakeLower();

    //如果句柄有效  则一直获取下一个句柄循环下去
    while (Process32Next(hSnapShot,&pe))
        {

        //pe.szExeFile获取当前进程的可执行文件名称
        CString scTmp = pe.szExeFile;


        //将可执行文件名称所有英文字母修改为小写
        scTmp.MakeLower();

        //比较当前进程的可执行文件名称和传递进来的文件名称是否相同
        //相同的话Compare返回0
        if(!scTmp.Compare(m_ProcessName)) {
            if(MessageBox(
                          str,
                          TEXT("Are you sure ？"),
                          MB_YESNO) != IDYES) {
                return;
            }

            //从快照进程中获取该进程的PID(即任务管理器中的PID)
            DWORD dwProcessID = pe.th32ProcessID;
            //OpenProcess 函数用来打开一个已存在的进程对象，并返回进程的句柄。
            HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
            ::TerminateProcess(hProcess,0);  //杀死进程
            CloseHandle(hProcess);
            return;
        }
        scTmp.ReleaseBuffer();
        }
    m_ProcessName.ReleaseBuffer();
}
#endif
