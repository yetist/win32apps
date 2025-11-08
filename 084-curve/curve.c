// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */
/*******************************************
* 程序: Curve.c 关
* 用途: 介绍贝塞尔(Bezier) 曲线的绘制。*
*******************************************/

#include <windows.h>
#include <string.h>
#include "curve.h"

HINSTANCE hInst;

HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    MSG msg;

    if (!InitApplication(hInstance))
        return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (msg.wParam);

}

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcexBezierCurve;

    wcexBezierCurve.cbSize = sizeof (WNDCLASSEX);
    wcexBezierCurve.style = 0;
    wcexBezierCurve.lpfnWndProc = (WNDPROC)MainWndProc;
    wcexBezierCurve.cbClsExtra = 0;
    wcexBezierCurve.cbWndExtra = 0;
    wcexBezierCurve.hInstance = hInstance;

    wcexBezierCurve.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcexBezierCurve.hCursor = LoadCursor (hInstance , IDC_ARROW);

    wcexBezierCurve.hbrBackground = GetStockObject (WHITE_BRUSH) ;
    wcexBezierCurve.lpszMenuName =  L"BezierCurveMenu";
    wcexBezierCurve.lpszClassName = L"BezierCurveWClass" ;
    wcexBezierCurve.hIconSm = LoadIcon (hInstance, IDI_WINLOGO);

    return (RegisterClassEx (&wcexBezierCurve));

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndMain = CreateWindow (
        L"BezierCurveWClass",
        L"绘制贝塞尔曲线",
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

LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static POINT Point[4]; /* 确定贝塞尔曲线的四点数组 */
    static int nState = 0; /* 曲线绘制过程中的状态 */
    static BOOL fErase = FALSE;
    int xPos,yPos;

    switch (message)
    {
        case WM_LBUTTONDOWN:
            xPos = LOWORD(lParam);
            yPos = HIWORD(lParam);

            SetCapture (hWnd) ;

            hdc = GetDC(hWnd);

            /* 设置光标绘制方式, 使得两次绘制同一条线, 能产生擦除的效果。*/

            SetROP2(hdc, R2_NOTXORPEN);

            switch (nState )
            {
                case 0: /* 确定曲线的起始点。*/
                    Point[0].x = xPos;
                    Point[0].y = yPos;
                    nState = 1;

                    break ;

                case 2: /* 试探曲线的起始控制点。*/

                    MoveToEx(hdc, Point[0].x, Point[0].y, NULL);
                    LineTo(hdc, Point[3].x, Point[3].y);

                    Point[1].x = xPos;
                    Point[1].y = yPos;
                    Point[2].x = xPos;
                    Point[2].y = yPos;
                    PolyBezier (hdc, Point, 4);
                    nState = 3;
                    fErase = TRUE;
                    break ;

                case 4: /* 试探曲线的终止控制点。*/

                    PolyBezier (hdc, Point, 4);

                    Point[2].x = xPos;
                    Point[2].y = yPos;

                    PolyBezier (hdc, Point, 4);

                    nState = 5;
                    fErase = TRUE;
                    break;
            }

            ReleaseDC(hWnd, hdc);
            break;

        case WM_LBUTTONUP:

            xPos = LOWORD(lParam) ;
            yPos = HIWORD (lParam) ;

            fErase = FALSE;

            switch (nState)
            {
                case 1: /* 确定曲线的终止点。*/
                    Point[3].x = xPos;
                    Point[3].y = yPos;
                    nState = 2;
                    break;
                case 3: /* 确定曲线的起始控制点 。*/
                    Point[1].x = xPos;
                    Point[1].y = yPos;
                    nState = 4;
                    break ;
                case 5: /* 确定曲线的终止控制点。*/
                    Point[2].x = xPos;
                    Point[2].y = yPos;

                    nState = 0;
                    break;
            }
            ReleaseCapture();
            break;

        case WM_MOUSEMOVE:
            xPos = LOWORD(lParam ) ;
            yPos = HIWORD (lParam) ;

            if (wParam & MK_LBUTTON)
            {
                hdc = GetDC(hWnd);
                SetROP2(hdc, R2_NOTXORPEN);

                switch (nState)
                {
                    case 1:
                        if (fErase)
                        {
                            MoveToEx(hdc, Point[0].x, Point[0].y, NULL);
                            LineTo(hdc, Point[3].x, Point[3].y);
                        }
                        Point[3].x = xPos;
                        Point[3].y = yPos;
                        MoveToEx (hdc, Point[0].x, Point[0].y, NULL);
                        LineTo(hdc, Point[3].x, Point[3].y);
                        break;

                    case 3:
                        if (fErase)
                            PolyBezier (hdc, Point, 4);
                        Point[1].x = xPos;
                        Point[1].y = yPos;
                        Point[2].x = xPos;
                        Point[2].y = yPos;
                        PolyBezier(hdc, Point, 4);
                        break;

                    case 5:
                        if (fErase)
                            PolyBezier(hdc, Point, 4);
                        Point[2].x = xPos;
                        Point[2].y = yPos;
                        PolyBezier (hdc, Point, 4);
                        break;
                }
                fErase = TRUE;
                ReleaseDC(hWnd, hdc);
            }
            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_EXIT:
                    PostQuitMessage (0);
                    break ;
                case IDM_ABOUT:
                    DialogBox (hInst, L"AboutBox", hWnd, (DLGPROC)About);
                    break;

                default:
                    return(DefWindowProc (hWnd, message, wParam, lParam));
            }
            break ;
        case WM_DESTROY:
            PostQuitMessage(0);
            break ;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}

BOOL APIENTRY About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message )
    {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD (wParam) == IDCANCEL)
            {
                EndDialog (hDlg, TRUE);
                return (TRUE);
            }
            break ;
    }
    return (FALSE) ;
}
