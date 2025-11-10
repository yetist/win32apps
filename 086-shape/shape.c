/************************************
程序: Shape. c
用途: 介绍如何绘制各种封闭图形, 如矩形、多边形和椭圆形等。
还介绍如何使用画笔来绘制图形边界,如何使用画刷来填充图形内部 。

*/

#include <windows.h>
#include <string.h>
#include "shape.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance ,
                     LPSTR IpCmdLine,
                     int nCmdShow)

{
    MSG msg;

    if (!InitApplication(hInstance))
        return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (msg.wParam);
}


BOOL InitApplication(HINSTANCE hInstance)
{

    WNDCLASSEX wcexShape ;

    wcexShape.cbSize = sizeof (WNDCLASSEX);
    wcexShape.style = 0;
    wcexShape.lpfnWndProc = (WNDPROC) MainWndProc;
    wcexShape.cbClsExtra = 0;
    wcexShape.cbWndExtra = 0;
    wcexShape.hInstance = hInstance;
    wcexShape.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcexShape.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcexShape.hbrBackground = GetStockObject (WHITE_BRUSH) ;
    wcexShape.lpszMenuName = L"ShapeMenu";
    wcexShape.lpszClassName = L"ShapeWClass";
    wcexShape.hIconSm = LoadIcon(hInstance, IDI_WINLOGO);
    return (RegisterClassEx (&wcexShape));
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow )
{
    hInst = hInstance;

    hWndMain = CreateWindow (L"ShapeWClass",
                             L"封闭图绘制的示范程序",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInstance, NULL);

    if(!hWndMain)
        return (FALSE);

    ShowWindow (hWndMain, nCmdShow);
    UpdateWindow (hWndMain);
    return (TRUE);
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    HDC hdc;
    WORD uItem;

    static int fnPenStyle = PS_SOLID;
    static int nPenWidth = 1;
    static COLORREF crPenColor = 0;
    static COLORREF crBrushColor = 0;

    HPEN hPen;
    HBRUSH hBrush;

    switch (message)
    {
        case WM_CREATE:
            hMenu = GetMenu(hWndMain);
            break;

        case WM_SIZE:
            SendMessage (hWnd, (UINT)WM_COMMAND, (WPARAM)1000, (LPARAM)
                         NULL);
            break;

        case WM_COMMAND:
            uItem = LOWORD(wParam);
            hdc = GetDC(hWnd);
            switch (uItem)
            {
                case IDM_W1:
                case IDM_W2:
                case IDM_W3:
                case IDM_W4:
                    nPenWidth = uItem - IDM_W1 + 1;
                    CheckMenuRadioItem (hMenu, IDM_W1, IDM_W4, uItem, MF_BYCOMMAND);
                    {
                        RECT rect;

                        GetClientRect (hWnd, &rect);
                        SelectObject (hdc, CreatePen(PS_NULL, 1, (COLORREF)0));
                        SelectObject (hdc, GetStockObject (WHITE_BRUSH));
                        Rectangle (hdc,  rect.left, rect.top, rect.right, rect.bottom);
                    }
                    break;

                case IDM_SOLID:
                    fnPenStyle = PS_SOLID;
                    CheckMenuRadioItem (hMenu, IDM_SOLID, IDM_DOT, uItem, MF_BYCOMMAND);
                    break;

                case IDM_DASH:
                    fnPenStyle = PS_DASH;
                    CheckMenuRadioItem (hMenu, IDM_SOLID, IDM_DOT, uItem, MF_BYCOMMAND);
                    break;

                case IDM_DOT:
                    fnPenStyle = PS_DOT;
                    CheckMenuRadioItem (hMenu, IDM_SOLID, IDM_DOT, uItem, MF_BYCOMMAND);
                    break;

                case IDM_RED:
                    crPenColor = RGB(255, 0, 0);
                    CheckMenuRadioItem (hMenu, IDM_RED, IDM_BLACK, uItem, MF_BYCOMMAND);
                    break;

                case IDM_YELLOW:
                    crPenColor = RGB(255, 255, 0);
                    CheckMenuRadioItem (hMenu, IDM_RED, IDM_BLACK, uItem, MF_BYCOMMAND);
                    break ;

                case IDM_GREEN :
                    crPenColor = RGB(0, 255, 0);
                    CheckMenuRadioItem (hMenu, IDM_RED, IDM_BLACK, uItem, MF_BYCOMMAND);
                    break;

                case IDM_BLUE:
                    crPenColor = RGB(0, 0,255);
                    CheckMenuRadioItem (hMenu, IDM_RED, IDM_BLACK, uItem, MF_BYCOMMAND);
                    break;

                case IDM_BLACK:
                    crPenColor = RGB(0, 0, 0);
                    CheckMenuRadioItem (hMenu, IDM_RED, IDM_BLACK, uItem, MF_BYCOMMAND);
                    break;

                case IDM_BRED:
                case IDM_BYELLOW:
                case IDM_BGREEN:
                case IDM_BBLUE:
                    CheckMenuRadioItem (hMenu, IDM_BRED, IDM_BBLUE, uItem, MF_BYCOMMAND);

                    crBrushColor = stBrush[uItem - IDM_BWHITE];
                    hBrush = CreateSolidBrush (crBrushColor) ;
                    SelectObject (hdc, hBrush);
                    break;
                case IDM_BWHITE:
                case IDM_BLTGRAY:
                case IDM_BGRAY:
                case IDM_BBKGRAY:
                case IDM_BBLACK:
                case IDM_BNULL:
                    CheckMenuRadioItem (hMenu, IDM_BWHITE, IDM_BNULL,
                                        uItem, MF_BYCOMMAND);

                    crBrushColor = stBrush [uItem - IDM_BWHITE];
                    SelectObject(hdc, GetStockObject (crBrushColor));
                    break ;

                case IDM_BHORIZONTAL:
                case IDM_BVERTICAL:
                case IDM_BFDIAGONAL:
                case IDM_BBDIAGONAL:
                case IDM_BCROSS:
                case IDM_BDIAGCROSS:
                    CheckMenuRadioItem (hMenu, IDM_BHORIZONTAL, IDM_BDIAGCROSS,
                                        uItem, MF_BYCOMMAND);
                    hBrush = CreateHatchBrush (htBrush[uItem - IDM_BHORIZONTAL],
                                               crBrushColor);
                    SelectObject (hdc, hBrush);
                    break;

                case IDM_EXIT:
                    ReleaseDC(hWnd,hdc);
                    PostQuitMessage(0);
                    return (0);
            }

            hPen = CreatePen(fnPenStyle, nPenWidth, crPenColor);
            SelectObject (hdc, hPen);

            // 绘制矩形

            Rectangle(hdc, 10, 10, 90, 60);

            // 绘制圆角矩形

            RoundRect(hdc, 10, 110, 90, 160, 30, 20);

            // 绘制椭圆形

            Ellipse(hdc, 110, 10, 190, 60);

            // 绘制扇形

            Pie(hdc, 110, 110, 190, 160, 190, 110, 160, 160);
            // 绘制弦形

            Chord(hdc, 110, 210, 190, 260, 190, 210, 160, 260);
            // 绘制多边形

            Polygon(hdc, mypolygon, 5);

            // 绘制多多边形

            PolyPolygon(hdc, mypolypolygon, npolypolygon, 3);

            TextOut(hdc, 35,   62, L"矩形",4);
            TextOut(hdc, 20,  162, L"圆角矩形", 8);
            TextOut(hdc, 124,  62, L"椭圆形", 6);
            TextOut(hdc, 130, 162, L"扇形",4);
            TextOut(hdc, 130, 262, L"弦形",4);
            TextOut(hdc, 224, 92,  L"多边形",6);
            TextOut(hdc, 220, 192, L"多多边形",8);
            ReleaseDC(hWnd, hdc);
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default :
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}
