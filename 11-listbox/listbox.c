/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
/*
 * listbox.c: This file is part of ...
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
#include <string.h>
#include <stdio.h>
#include "listbox.h"

HINSTANCE hInst;
HWND hWndMain;

int aPoints[NUM_POINTS] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28,
    36, 48, 72};

COLORREF crTextColor;
HMENU hMenu;

HDC hdcGlobal;
PTRUETYPEFONTINFO pArrayOfTTFInfo;
int iFace;
int nFaces;
int LogPixelSy;

LOGFONT LogFont = { -18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE,
    "abc"
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg;

  if (!InitApplication(hInstance))
    return FALSE;

  if (!InitInstance(hInstance, nCmdShow))
    return FALSE;

  SendMessage(hWndMain, WM_COMMAND, IDM_SIZE + 4, 0);

  while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
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
    wcex.lpszMenuName  = TEXT("ListBoxMenu");
    wcex.lpszClassName = TEXT("ListBoxWClass");
    wcex.hIconSm       = LoadIcon(hInstance, TEXT("Small"));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    hWnd = CreateWindow(TEXT("ListBoxWClass"),
                            TEXT("列表框示范程序"),
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    static CHAR szBuf[128];
    HDC hDC;
    HFONT hFont;
    static int nCurSize;
    UINT uItem;
    RECT Rect;

    switch (message) {
      case WM_CREATE:
            {
            hMenu = GetMenu(hWnd);
            crTextColor = RGB(0, 0, 0);
            hDC = GetDC(hWnd);
            LogPixelSy = GetDeviceCaps(hDC, LOGPIXELSY);

            // 枚举字体，建立字体列表
            pArrayOfTTFInfo = BuildFontList (hDC, &nFaces);

            // 填充字体尺寸菜单
            FillMenu(hWnd);

            // 确定初始字体的字样
            lstrcpy(LogFont, lfFaceName, pArrayOfTTFInfo[0].plf -> lfFaceName);
            ReleaseDC(hWnd, hDC);
            }
        break;
      case WM_SIZE:
        SendMessage(hWnd, WM_COMMAND, IDM_NULL, 0);
        break;
      case WM_COMMAND:
        uItem = LOWORD(wParam);
        switch(uItem) {
          case IDM_SELECTFONTFACE:
            DialogBox(hInst, "SelectFontFaceDlgBox", hWnd, (DLGPROC)
                      SelectFontFaceDlgProc);
            break;
          case IDM_SELECTFONTFACE:
            DialogBox(hInst, "SelectFontStyleDlgBox", hWnd, (DLGPROC)
                      SelectFontStyleDlgProc);
            break;
          case IDM_SELECTFONTORIENT:
            DialogBox(hInst, "SelectFontOrientDlgBox", hWnd, (DLGPROC)
                      SelectFontOrientDlgProc);
            break;
          case IDM_SELECTFONTCOLOR:
            DialogBox(hInst, "SelectFontColorDlgBox", hWnd, (DLGPROC)
                      SelectFontColorDlgProc);
            break;
          case IDM_NULL:
            break;
          case IDM_EXIT:
            SendMessage(hWnd, WM_CLOSE, 0, 0L);
            break;
          case IDM_ABOUT:
            DialogBox(hInst, TEXT("AboutBox"), hWnd, (DLGPROC) About);
            break;
          default:
            if(uItem >= IDM_SIZE && uItem < IDM_SIZE + NUM_POINTS) {
                nCurSize = uItem - IDM_SIZE;
                LogFont.lfHeight = -MulDiv(aPoints[nCurSize], LogPixelSy, 72);
                CheckMenuRadioItem(GetMenu(hWnd), IDM_SIZE, IDM_SIZE + 15,
                                   uItem, MF_BYCOMMAND);
            } else {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        if (hFont != NULL)
            DeleteObject(hFont);
        hFont = CreateFontIndirect(&LogFont);
        if (hFont != NULL) {
            hDC = GetDC(hWnd);
            GetClientRect(hWnd, &Rect);

            // 清除窗口客户区中的内容
            Rectangle(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom);

            // 采用新选择的字体来显示文字
            SelectObject(hDC, hFont);
            SetTextColor(hDc, crTextColor);

            sprintf(szBuf, "系统中共安装有 %d 种 TrueType 字体", nFaces);
            TextOut(hDc, 10, 150, szBuf, strlen(szBuf));

            sprintf(szBuf, "当前采用的字体是 %s", LogFont.lfFaceName);
            TextOut(hDc, 10, 10, szBuf, strlen(szBuf));

            ReleaseDC(hWnd, hDC);
        }
        break;
      case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
      default:
        // 调用默认窗口过程对未处理的消息进行必要的处理
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

PTRUETYPEFONTINFO BuildFontList(HDC hDCFont,    // 设备描述表句柄
                                LPINT retnFaces) //返回字体数目的变量的指针
{
    nFaces = 0;

    hdcGlobal = hDCFont;

    EnumFontFamilies(hdcGlobal, NULL, (FONTENUMPROC) MyEnumFontCountProc,
                     (LPARAM) &nFaces);

    pArrayOfTTFInfo = (PTRUETYPEFONTINFO) LocalAlloc(LPTR,
                                                     sizeof(TRUETYPEFONTINFO) *
                                                     (nFaces + 1));

    iFace = 0;
    EnumFontFamilies(hdcGlobal, NULL, (FONTENUMPROC) MyEnumFontFacesProc,
                     (LPARAM) NULL);
    *retnFaces = nFaces;
    return pArrayOfTTFInfo;
}

int APIENTRY MyEnumFontFacesProc(LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPVOID lpData)
{
    if(fFontType & TRUETYPE_FONTTYPE) {
        pArrayOfTTFInfo[iFace].plf = (LPLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));
        pArrayOfTTFInfo[iFace].ptm = (LPTEXTMETRIC) LocalAlloc(LPTR, sizeof(TEXTMETRIC));

        if ((pArrayOfTTFInfo[iFace].plf == NULL) || (pArrayOfTTFInfo[iFace].ptm == NULL)) {
            MessageBox(NULL, "内存分配失败", NULL, MB_OK);
            return FALSE;
        }

        *(pArrayOfTTFInfo[iFace].plf) = *lpLogFont;
        *(pArrayOfTTFInfo[iFace].ptm) = *lpTEXTMETRICs;

        iFace++;
    }
    return TRUE;
}

int APIENTRY MyEnumFontCountProc(LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPINT lpData)
{
    if(fFontType & TRUETYPE_FONTTYPE)
        (*lpData) ++;
    return TRUE;
}

VOID FillMenu(HWND hWnd)
{
    int idx;
    char szSize[10];
    HMENU hMenuMain;
    HMENU hMenuSize;

    hMenuMain = GetMenu(hWnd);

    hMenuSize = CreatePopupMenu();
    //TODO: pg283


    break;
  case WM_LBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
    /* 如果鼠标双击了窗口客户区，那么就地显示坐标信息 */
    hdc = GetDC(hWnd);

    SetTextColor(hdc, (message == WM_LBUTTONDBLCLK) ? RGB(255, 0, 0) : RGB(0, 0, 255));
    wsprintf(szTitle, TEXT("(%d, %d)"), LOWORD(lParam), HIWORD(lParam));
    TextOut(hdc, LOWORD(lParam), HIWORD(lParam), szTitle, lstrlen(szTitle));

    ReleaseDC(hWnd, hdc);
    break;

  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);

    hMemDc = CreateCompatibleDC(hdc);
    SelectObject(hMemDc, hSun);
    BitBlt(hdc, 90, 8, 48, 48, hMemDc, 0, 0, SRCCOPY);
    DeleteDC(hMemDc);

    hMemDc = CreateCompatibleDC(hdc);
    SelectObject(hMemDc, hMoon);
    BitBlt(hdc, 90, 78, 48, 48, hMemDc, 0, 0, SRCCOPY);
    DeleteDC(hMemDc);

    EndPaint(hWnd, &ps);
    break;

  case WM_DESTROY:
    DeleteObject(hSun);
    DeleteObject(hMoon);
    PostQuitMessage(0);
    break;
  default:
    // 调用默认窗口过程对未处理的消息进行必要的处理
    return DefWindowProc(hWnd, message, wParam, lParam);
}

return 0;
}


