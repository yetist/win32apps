/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2024 Xiaotian Wu <yetist@gmail.com>
 */

#include <windows.h>
#include "resource.h"

HINSTANCE hgAppInst;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hThisApp, HINSTANCE hPrevApp, LPSTR lpCmd, int nShow)
{
  MSG msg;
  HWND hwnd;
  WNDCLASS wc = {0};
  hgAppInst = hThisApp;

  // 设计窗口类
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpfnWndProc = (WNDPROC)WindowProc;
  wc.lpszClassName = TEXT("supper_man");
  wc.hInstance = hThisApp;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  RegisterClass(&wc);
  // 创建窗口
  hwnd = CreateWindow(TEXT("supper_man"), TEXT("超级应用"), WS_OVERLAPPEDWINDOW, 40, 25, 380, 300, NULL, NULL, hThisApp, NULL);
  if(!hwnd)
	return 0;
  ShowWindow(hwnd, nShow);
  UpdateWindow(hwnd);
  // 消息循环
  while(GetMessage(&msg,NULL,0,0))
	{
	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
	}
  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
	{
	case WM_CREATE:
		{
		  // 创建对话框
		  HWND hdlg = CreateDialog(hgAppInst, MAKEINTRESOURCE(IDD_FORMVIEW), hwnd, (DLGPROC)DlgProc);
		  // 显示对话框
		  ShowWindow(hdlg, SW_SHOWNA);
		}
	  return 0;
	case WM_DESTROY:
	  PostQuitMessage(0);
	  return 0;
	default:
	  return DefWindowProc(hwnd,msg,wParam,lParam);
	}
  return 0;
}

// 处理对话框消息
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg) {
    case WM_INITDIALOG:
      // 设置对话框的图标
      SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hgAppInst, MAKEINTRESOURCE(IDI_APP)));
      return 0;
    case WM_SYSCOMMAND:
      if(wParam == SC_CLOSE) {
          PostQuitMessage(0);//退出
      }
      return 0;
  }
  return (INT_PTR)FALSE;
}
