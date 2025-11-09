// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2025 yetist <yetist@gmail.com>
 */

//目的:通过使用画点函数,绘制Mandelbrot 点集
#include <windows.h>
#include <string.h>
#include <math.h>
#include "pixelset.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    MSG msg;
    HACCEL hAccel;

    if (!InitApplication(hInstance))
        return FALSE;

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    hAccel = LoadAccelerators(hInstance, L"PixelSetMenu");

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hWndMain, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}


BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = 0;
    wcex.lpfnWndProc   = (WNDPROC) MainWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName  = L"PixelSetMenu";
    wcex.lpszClassName = L"PixelSetWClass";
    wcex.hIconSm       = LoadIcon(hInstance, IDI_WINLOGO);

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndMain = CreateWindowW(L"PixelSetWClass",
                             L"分形图案-Mandelbrot 点集",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInstance, NULL);

    if (!hWndMain)
        return FALSE;

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    return TRUE;
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    HDC hdc;
    HCURSOR hCursor;
    PAINTSTRUCT ps;
    COLORREF rgbPixel;

    static RECT Rect;
    static POINT point[2];
    static BOOL fErase = FALSE;
    wchar_t szMsg[100];

    switch (message) {
        case WM_PAINT:
            //指定整个客户区为更新区域。
            InvalidateRgn(hWnd, NULL, TRUE);
            hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &Rect);

            //把光标设置为等待光标。
            hCursor = LoadCursor(NULL, IDC_WAIT);
            SetCursor(hCursor);

            //在窗口客户区中绘制 Mandelbrot 点集
            DrawMandelbrotSet(hdc, Rect.left, Rect.top, Rect.right, Rect.bottom);

            //把光标恢复为箭头光标
            hCursor = LoadCursor(NULL, IDC_ARROW);
            SetCursor(hCursor);

            EndPaint(hWnd, &ps);
            break;
        case WM_RBUTTONDOWN:
            hCursor = LoadCursor(hInst, IDC_IBEAM);
            SetCursor(hCursor);

            break;
        case WM_RBUTTONUP:
            hCursor = LoadCursor(NULL, IDC_ARROW);
            SetCursor(hCursor);

            hdc = GetDC(hWnd);
            rgbPixel = GetPixel (hdc, LOWORD(lParam), HIWORD(lParam));
            if (rgbPixel != CLR_INVALID) {
                wsprintf(szMsg, L"像素点颜色为(红:%03d, 绿:%03d, 蓝:%03d)",
                         rgbPixel & 0x000000FFL,
                         (rgbPixel & 0x0000FF00L) >>8,
                         (rgbPixel & 0x00FF0000L) >> 16);
                MessageBox(hWnd, szMsg, L"取像素点颜色", MB_OK | MB_ICONINFORMATION);
            }
            ReleaseDC(hWnd, hdc);
            break;
        case WM_LBUTTONDOWN:
            point[0].x = LOWORD(lParam);
            point[0].y = HIWORD(lParam);

            newPMin = PMin + (PMax - PMin) * point[0].x /Rect.right;
            newQMin = QMin + (QMax - QMin) * point[0].y /Rect.bottom;
            break;
        case WM_LBUTTONUP:
            point[1].x = LOWORD(lParam);
            point[1].y = HIWORD(lParam);
            fErase = FALSE;

            if (point[1].x == point[0].x || point[1].y == point[0].y)
                break;

            newPMax = PMin + (PMax - PMin) * point[1].x /Rect.right;
            newQMax = QMin + (QMax - QMin) * point[1].y /Rect.bottom;

            if(newPMax > newPMin)
            {
                PMax = newPMax;
                PMin = newPMin;
            } else {
                PMax = newPMin;
                PMin = newPMax;
            }

            if (newQMax > newQMin)
            {
                QMax = newQMax;
                QMin = newQMin;
            } else {
                QMax = newQMin;
                QMin = newQMax;
            }

            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case WM_MOUSEMOVE:
            if(wParam & MK_RBUTTON)
            {
                hCursor = LoadCursor(hInst, IDC_IBEAM);
                SetCursor(hCursor);
            } else if (wParam & MK_LBUTTON)
            {
                hdc = GetDC(hWnd);
                SetROP2(hdc, R2_NOTXORPEN);
                if(fErase)
                    DrawMyRect(hdc, point[0].x, point[0].y, point[1].x, point[1].y);
                fErase = TRUE;

                point[1].x = LOWORD(lParam);
                point[1].y = HIWORD(lParam);

                DrawMyRect(hdc, point[0].x, point[0].y, point[1].x, point[1].y);

                ReleaseDC(hWnd, hdc);
            }
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_ZOOMIN:
                    newPMin = 4 * PMin - 3 * PMax;
                    newQMin = 4 * QMin - 3 * QMax;
                    newPMax = 4 * PMax - 3 * PMin;

                    PMin = newPMin;
                    PMax = newPMax;
                    QMin = newQMin;

                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case IDM_EXIT:
                    PostQuitMessage(0);
                    break;
                case IDM_ABOUT:
                    DialogBox(hInst, L"AboutBox", hWnd, (DLGPROC) About);
                    break;
                default:
                    return (DefWindowProc(hWnd, message, wParam, lParam));
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            // 调用默认窗口过程对未处理的消息进行必要的处理
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int DrawMandelbrotSet(HDC hdc, int xMin, int yMin, int xMax, int yMax)
{
    double DetP, DetQ;
    double p, q, x1, y1, x, y, r;
    int M=100, K=256, k;
    int np, nq;

    QMax = QMin + (PMax - PMin) * (yMax - yMin)/(xMax - xMin);
    DetP = (PMax - PMin)/(xMax - xMin);
    DetQ = (QMax - QMin)/(yMax - yMin);

    for(nq = yMin; nq <= yMax; nq++) {
        for (np = xMin; np<=xMax; np++)
        {
            p = PMin + np * DetP;
            q=QMin + nq * DetQ;

            k=0;
            x1 = x = 0.0;
            y1 = y = 0.0;

            do {
                x = x1 * x1 - y1 * y1 + p;
                y = 2 * x1 * y1 + q;
                x1 = x; y1 = y;
                k++;
                r= x*x + y *y;
            } while ( r <= M && k < K);

            if (k == K) k--;
            k = 255 -k;

            SetPixel(hdc, np, nq, RGB(k, k, k));
        }
    }
    return 1;
}

int DrawMyRect (HDC hdc, int x0, int y0, int x1, int y1)
{
    MoveToEx(hdc, x0, y0, NULL);

    LineTo(hdc, x0, y1);
    LineTo(hdc, x1, y1);
    LineTo(hdc, x1, y0);
    LineTo(hdc, x0, y0);

    return 1;
}

BOOL APIENTRY About(HWND hDlg,
                    UINT message,
                    WPARAM wParam,
                    LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            break;
    }

    return FALSE;
}
