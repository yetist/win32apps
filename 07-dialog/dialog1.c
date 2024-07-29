/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */

#include <windows.h>
#include "resource.h"

HINSTANCE hgInst;

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hThisApp, HINSTANCE hPrevApp, LPSTR lpCmd, int nShow)
{
  hgInst = hThisApp;
  MSG msg;
  HWND hdlg;

  // 创建无模式对话框
  hdlg = CreateDialog(hThisApp, MAKEINTRESOURCE(IDD_DIALOG_MAIN),GetDesktopWindow(),(DLGPROC)DlgProc);
  if(!hdlg) {
      return 0;
  }
  ShowWindow(hdlg,SW_SHOW);
  while(GetMessage(&msg,NULL,0,0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }
  return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg) {
    case WM_INITDIALOG:
      // 设置对话框的图标
      SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hgInst, MAKEINTRESOURCE(IDI_APP)));
      return 0;
    case WM_SYSCOMMAND:
      if(wParam == SC_CLOSE) {
          PostQuitMessage(0);//退出
      }
      return 0;
  }
  return (INT_PTR)FALSE;
}
