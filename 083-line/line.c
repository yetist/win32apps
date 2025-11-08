// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */
/*
* 程序 : Line.c *
* 目的 : 绘制直线示范 , 通过递归调用函数 , 画出一椎分形树 。
*/

#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include <math.h>
#include "line.h"

int DrawTree(HWND hWndProgress, HDC hdc, int xStart, int yStart, double length,
             double angle, int num);

HINSTANCE hInst;
HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR IpCmdLine,
                     int nCmdShow)
{
    MSG msg;

    if (!InitApplication (hInstance))
        return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
    }

    return (msg.wParam);
}

BOOL InitApplication(HINSTANCE hInstance)
{

    WNDCLASSEX wcexLine;


    wcexLine.cbSize = sizeof (WNDCLASSEX);
    wcexLine.style = 0;
    wcexLine.lpfnWndProc = (WNDPROC) MainWndProc;
    wcexLine.cbClsExtra = 0;
    wcexLine.cbWndExtra = 0;

    wcexLine.hInstance = hInstance ;
    wcexLine.hIcon  = LoadIcon (hInstance, IDI_APPLICATION);
    wcexLine.hCursor = LoadCursor (NULL, IDC_ARROW);

    wcexLine.hbrBackground = GetStockObject (WHITE_BRUSH) ;
    wcexLine.lpszMenuName = NULL;
    wcexLine.lpszClassName = L"LineWClass";
    wcexLine.hIconSm = LoadIcon (hInstance, IDI_HAND);

    return (RegisterClassEx (&wcexLine));
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndMain = CreateWindow (L"LineWClass",
                             L"绘制直线示范程序",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInstance, NULL);

    if (!hWndMain)
        return (FALSE);

    ShowWindow (hWndMain, nCmdShow);
    UpdateWindow (hWndMain);
    return (TRUE);
}

LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    HPEN hGreenPen;
    static LOGPEN lpGreenPen = {PS_SOLID, 1, 1, RGB(0, 255, 0)};

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps) ;
            hGreenPen = CreatePenIndirect(&lpGreenPen);
            SelectObject(hdc, hGreenPen);
            /* 设置图形映射方式 */
            SetMapMode(hdc, MM_LOENGLISH) ;
            GetClientRect(hWnd, &rect) ;
            /* 绘制递归分形树 */
            DrawTree(hWnd, hdc, rect.right/2, -rect.bottom,
                     (double)rect.bottom * 0.2, 1.57, 10);

            EndPaint (hWnd, &ps);
            DeleteObject (hGreenPen) ;
            break;

        case WM_SIZE:
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case WM_DESTROY :
            PostQuitMessage (0) ;
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}

/**************************************
*
函 数 : DrawTree(HWND, HDC, int, int, double, double, int)
用 途 , 画 递 归 三 叉 树 。 *
*
*
********************/
int DrawTree(HWND hWndProgress,         /* 进度条句柄 */
             HDC hdc,                   /*  显示设备描述表句柄 */
             int xStart, int yStart,    /*  树 枝 或 树 干 的 起 始 点 位 置 */
             double length,             /*  树 枝 或 树 干 的 长 度 */
             double angle,              /* 树 枝 或 树 干 的 倾 斜 角 度 ( 弧 度 ) */
             int num)                   /* 递 归 层 次 数 */
{
    int xEnd, yEnd;
    if (num==0) return 1;

    xEnd = xStart + (int) (length * cos(angle));
    yEnd = yStart + (int) (length * sin(angle));

    /* 画本子树的树干 */
    MoveToEx(hdc, xStart, yStart, NULL);
    LineTo(hdc, xEnd, yEnd);

    /* 使进度条的进度位置向前跃一步。*/
    SendMessage (hWndProgress, PBM_STEPIT, 0L, 0L);

    /* 画 本 子 树 的 左 叉 树 枝 。 */
    DrawTree(hWndProgress, hdc, xEnd, yEnd, length * 0.6, angle + 0.624, num-1);

    /* 画 本 子 树 的 中 叉 树 枝 。*/
    DrawTree(hWndProgress , hdc, xEnd, yEnd, length * 0.85, angle + 0.08, num-1);

    /* 画 本 子 树 的 右 叉 树 枝 。*/
    DrawTree(hWndProgress , hdc, xEnd, yEnd, length * 0.65, angle-0.6, num-1);

    return 1;
}
