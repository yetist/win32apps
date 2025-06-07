/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * multh.c: This file is part of ...
 *
 * Copyright (c) 2024 yetist <yetist@gmail.com>
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

#define PROCMAX 5 //定义建立进程最大数目

#include <stdlib.h>
#include "windows.h"
#include <string.h>
#include <stdio.h>


BOOL FirstInit(HANDLE);
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT message,
                              WPARAM wParam, LPARAM lParam); //主窗口函数说明
DWORD MyThread1 (LPVOID lParam); //附加线程1
DWORD MyThread2 (LPVOID param); //附加线程2
char szWinName[] = "My Win";
wchar_t str[255];
int X=0,Y=0;
int maxX, maxY;
int procnum=0;

DWORD Tid1, Tid2;
HDC memdc;
HBITMAP hbit;
HBRUSH hbrush;

PROCESS_INFORMATION prinfo[PROCMAX];
typedef struct
{
    HWND hwndmain;
    HANDLE hInstance;
} ThreadData;

HANDLE hSema;
TEXTMETRIC tm;
HANDLE hInst;
HMENU hMenu;
HWND hMainWin;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{

    HWND hWndFrame; //hWndClient;
    HANDLE hAccel;
    MSG msg;
    if(hPrevInstance)
    {
        if(!FirstInit (hInstance))
            return FALSE;
    }

    hInst=hInstance;
    hMenu=LoadMenu (hInst, TEXT("MYMENU")); //主窗口菜单初始化
    hAccel=LoadAccelerators (hInst, TEXT("MYACCEL")); //快捷键资源初始化
    hWndFrame=CreateWindow(TEXT("Win_MDI:FRAME"),
                           TEXT("多进程/多线程应用程序示范"),
                           WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
                           CW_USEDEFAULT,CW_USEDEFAULT,
                           CW_USEDEFAULT,CW_USEDEFAULT,
                           NULL,hMenu,hInstance, NULL); //创建主窗口

    hMainWin=hWndFrame;
    ShowWindow (hWndFrame,nCmdShow);
    UpdateWindow (hWndFrame); //显示主窗口
    while (GetMessage (&msg,NULL,0,0)) //启动消息循环
    {
        if(!TranslateAccelerator (hWndFrame,hAccel,&msg))
            TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}

//窗口类初始化函数
BOOL FirstInit (HANDLE hInstance)
{
    WNDCLASS wcFirstClass;
    wcFirstClass.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcFirstClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    wcFirstClass.lpszMenuName=NULL;
    wcFirstClass.lpszClassName= TEXT("Win_MDI:FRAME");
    wcFirstClass.hbrBackground=GetStockObject(WHITE_BRUSH);
    wcFirstClass.hInstance=hInstance;
    wcFirstClass.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    wcFirstClass.lpfnWndProc=FrameWndProc;
    wcFirstClass.cbClsExtra=0;
    wcFirstClass.cbWndExtra=0;
    if(!RegisterClass(&wcFirstClass))//注册主窗口类
        return FALSE;
    return TRUE;
}

LRESULT CALLBACK FrameWndProc(HWND hWnd,UINT message ,WPARAM wParam,LPARAM lParam)//主窗口函数
{
    HDC hdc;
    PAINTSTRUCT paintstruct;
    STARTUPINFO startin;
    int flag=0;

    switch(message)
    {

        case WM_CREATE:
            hSema=CreateSemaphore(NULL,1,1, TEXT("mysem")); //建立信号机
            maxX=GetSystemMetrics(SM_CXSCREEN);
            maxY=GetSystemMetrics(SM_CXSCREEN);

            hdc=GetDC(hWnd);
            memdc=CreateCompatibleDC(hdc);
            hbit=CreateCompatibleBitmap(hdc,maxX,maxY); //创建位图,大小为 maxX* maxY
            SelectObject (memdc ,hbit);

            hbrush =GetStockObject(WHITE_BRUSH);
            SelectObject(memdc,hbrush);
            PatBlt (memdc, 0,0,maxX,maxY,PATCOPY);
            ReleaseDC(hWnd,hdc);
            break;
        case WM_COMMAND://菜单消息响应
            switch (LOWORD(wParam))
            {
                case 10:
                    if(procnum==PROCMAX)
                    {
                        MessageBox(hWnd, TEXT("can't create"), TEXT(""),MB_OK);
                        break;
                    }
                    GetTextMetrics(memdc, &tm);
                    wsprintf(str, TEXT("Execute Process %d"), procnum);
                    TextOut(memdc, X, Y, str, wcslen(str));
                    Y = Y + tm.tmHeight + tm.tmExternalLeading;

                    InvalidateRect(hWnd,NULL,1);

                    startin.cb=sizeof(STARTUPINFO);
                    startin.lpReserved=NULL;
                    startin.lpDesktop=NULL;
                    startin.lpTitle=NULL;
                    startin.dwFlags = STARTF_USESHOWWINDOW;
                    startin.cbReserved2=0;
                    startin.lpReserved2=NULL;
                    startin.wShowWindow=SW_SHOWMINIMIZED;

                    CreateProcess(NULL, TEXT("mdidemo.exe"), NULL,NULL,FALSE,0,
                                  NULL,NULL,&startin,&prinfo[procnum]);//建立一个进程
                    flag=1;
                    procnum++;
                    MessageBox(hWnd, TEXT("No process to terminate"), TEXT(""), MB_OK);
                    break;
                case 11:
                    if(procnum)
                        procnum--;
                    else
                    {
                        MessageBox(hWnd, TEXT("No process to terminate"), TEXT(""), MB_OK);
                        break ;
                    }
                    TerminateProcess(prinfo[procnum].hProcess, 0); //终止一个进程

                    wsprintf(str, TEXT("Terminate Process %d."), procnum);
                    TextOut(memdc,X,Y,str, wcslen(str));
                    Y=Y+tm.tmHeight + tm.tmExternalLeading;
                    flag=1;
                    InvalidateRect(hWnd,NULL,1);
                    break;
                    //mecmeped(hwnd, "Create a new Proces", "Message", MB_OK);
        case 12:
                    CreateThread (NULL,0,(LPTHREAD_START_ROUTINE) MyThread1,
                                  (LPVOID)hWnd,0, &Tid1); //建立线程1
                    CreateThread (NULL, 0,(LPTHREAD_START_ROUTINE)MyThread2,
                                  (LPVOID)hWnd,0, &Tid2);//建立线程2
                    flag=0;
                    break ;
        case 13:
                    MessageBox (hWnd,
                                TEXT("F2, Start Process\nF3, Kill Process\nF4, Start Thread"),
                                TEXT("Help"), MB_OK);
                    break;
            }
            break;
            if(flag==1){
                case WM_PAINT:
                    hdc=BeginPaint(hWnd, &paintstruct);
                    BitBlt (hdc,0,0, maxX, maxY ,memdc, 0, 0, SRCCOPY);
                    EndPaint(hWnd, &paintstruct);
                    break;
            }
        case WM_LBUTTONDOWN:
            X=LOWORD(lParam);
            Y=HIWORD(lParam);
            break;
        case WM_DESTROY:
            DeleteDC(memdc) ;
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam,lParam);
    }
    return 0;
}


//线程函数1
DWORD MyThread1(LPVOID param)
{
    DWORD curTid=Tid1;
    HDC hdc;
    HBRUSH hBrush, hOldBrush;
    HRGN hRgn;

    int i,n;

    if(WaitForSingleObject(hSema,10000)==WAIT_TIMEOUT)
        MessageBox((HWND)param, TEXT("Time Out Thread1"),
                   TEXT("Semaphore Error"), MB_OK);

    LOGBRUSH lbBrush;
    lbBrush.lbStyle=BS_SOLID;
    hdc=GetDC((HWND)param);
    SelectObject (hdc,GetStockObject(GRAY_BRUSH));
    SelectObject (hdc,GetStockObject(BLACK_PEN));
    Rectangle(hdc,10,10,100,100);

    for(n=0;n<5;n++)
    {
        for(i=0;i<=40;i++)
            {
            lbBrush.lbColor=RGB(5 * i, 255-5 * i,0);
            hBrush=CreateBrushIndirect(&lbBrush);
            hOldBrush =SelectObject (hdc, hBrush);


            hRgn=CreateEllipticRgn(50-i,50-i,60+i,60+i);
            SelectClipRgn(hdc,hRgn);
            Rectangle(hdc,10,10,100,100);
            DeleteObject(hRgn);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);

            DWORD j,k;
            for(j=0;j<1000;j++)
                for(k=0;k<1000;k++){;}
        }

        for(i=40;i>=0;i--)
        {
            SelectObject (hdc,GetStockObject(GRAY_BRUSH));
            SelectObject (hdc,GetStockObject(BLACK_PEN));
            Rectangle(hdc,10,10,100,100);

            lbBrush.lbColor=RGB(50* n,255-50 * n,0);
            hBrush=CreateBrushIndirect(&lbBrush);

            hOldBrush=SelectObject(hdc,hBrush);
            hRgn=CreateEllipticRgn(50-i,50-i,60+i,60+i);
            SelectClipRgn (hdc,hRgn);
            Rectangle(hdc,10,10,100,100);
            DeleteObject(hRgn);
            SelectObject(hdc,hOldBrush);
            DeleteObject(hBrush);

            DWORD j,k;
            for(j=0;j<1000;j++)
                for(k=0;k<1000;k++) {;}
        }
    }
    ReleaseSemaphore(hSema,1,NULL);
    return 1;
}

DWORD MyThread2(LPVOID param) //线程函数2
{
    HDC hdc;
    HBRUSH hBrush,hOldBrush;
    HRGN hRgn;
    int i,n;
    DWORD curTid=Tid2;

    if(WaitForSingleObject(hSema,10000)==WAIT_TIMEOUT)
        MessageBox((HWND)param, TEXT("Time Out Thread2"),
                   TEXT("Semaphore Error"), MB_OK);


    LOGBRUSH lbBrush;
    lbBrush.lbStyle=BS_SOLID;
    hdc=GetDC((HWND)param);
    SelectObject(hdc,GetStockObject(GRAY_BRUSH));
    SelectObject(hdc,GetStockObject(BLACK_PEN));

    Rectangle(hdc,10,10,100,100);
    for(n=0;n<5;n++)
    {
        for(i=0;i<=40;i++)
        {
            lbBrush.lbColor=RGB(5 *i,255-5 * i,0);
            hBrush=CreateBrushIndirect(&lbBrush);
            hOldBrush=SelectObject(hdc,hBrush);
            hRgn=CreateEllipticRgn(150-i,50-i,160+i,60+i);
            SelectClipRgn(hdc, hRgn);
            Rectangle(hdc,110,10,200,100);
            DeleteObject(hRgn);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
            DWORD j,k;
            for(j=0;j<1000;j++)
                for(k=0;k<1000;k++){;}
        }

        for(i=40;i>=0;i--)
        {
            SelectObject(hdc ,GetStockObject(GRAY_BRUSH));
            SelectObject(hdc ,GetStockObject(BLACK_PEN));
            Rectangle(hdc,110,10,200,100);
            lbBrush.lbColor=RGB(50 * n,255-50 * n,0);
            hBrush =CreateBrushIndirect (&lbBrush);
            hOldBrush=SelectObject(hdc, hBrush);
            hRgn=CreateEllipticRgn(150-i, 50-i, 160+i, 60+i);
            SelectClipRgn(hdc, hRgn);
            Rectangle(hdc,110,10,200,100);
            DeleteObject (hRgn);
            SelectObject (hdc, hOldBrush);
            DeleteObject (hBrush);

            DWORD j,k;
            for(j=0;j<1000;j++)
                for(k=0;k<1000;k++){;}
        }
    }
    ReleaseSemaphore(hSema,1,NULL);
    return 1;
}
