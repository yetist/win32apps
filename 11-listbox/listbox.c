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
    TEXT("abc")
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
    static TCHAR szBuf[128];
    HDC hDC;
    HFONT hFont = NULL;
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
            lstrcpy(LogFont.lfFaceName, pArrayOfTTFInfo[0].plf -> lfFaceName);
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
            DialogBox(hInst, TEXT("SelectFontFaceDlgBox"), hWnd, (DLGPROC)
                      SelectFontFaceDlgProc);
            break;
          case IDM_SELECTFONTSTYLE:
            DialogBox(hInst, TEXT("SelectFontStyleDlgBox"), hWnd, (DLGPROC)
                      SelectFontStyleDlgProc);
            break;
          case IDM_SELECTFONTORIENT:
            DialogBox(hInst, TEXT("SelectFontOrientDlgBox"), hWnd, (DLGPROC)
                      SelectFontOrientDlgProc);
            break;
          case IDM_SELECTFONTCOLOR:
            DialogBox(hInst, TEXT("SelectFontColorDlgBox"), hWnd, (DLGPROC)
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
            SetTextColor(hDC, crTextColor);

            _swprintf(szBuf, TEXT("系统中共安装有 %d 种 TrueType 字体"), nFaces);
            TextOut(hDC, 10, 150, szBuf, wcslen(szBuf));

            _swprintf(szBuf, TEXT("当前采用的字体是 %s"), LogFont.lfFaceName);
            TextOut(hDC, 10, 10, szBuf, wcslen(szBuf));

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
            MessageBox(NULL, TEXT("内存分配失败"), NULL, MB_OK);
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
    TCHAR szSize[10];
    HMENU hMenuMain;
    HMENU hMenuSize;

    hMenuMain = GetMenu(hWnd);

    hMenuSize = CreatePopupMenu();
    InsertMenu(hMenuMain, (UINT) 2, MF_BYPOSITION | MF_STRING | MF_POPUP,
               (UINT_PTR) hMenuSize, TEXT("尺寸(&Z)"));

    for(idx = 0; idx < NUM_POINTS; idx++) {
        wsprintf(szSize, TEXT("%d"), aPoints[idx]);
        AppendMenu(hMenuSize, MF_STRING | MF_UNCHECKED, IDM_SIZE + idx,
                    (LPCWSTR) szSize);
    }
}

BOOL APIENTRY SelectFontFaceDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam)
{
    TCHAR szFontFaceName[LF_FACESIZE];;
    int  nItem;
    int idx;
    static int  nCurrentFont = 0;

    switch(message) {
        case WM_INITDIALOG:
          for(idx = 0; idx < nFaces;  idx ++) {
              SendMessage(GetDlgItem(hDlg, IDL_FONTFACE), LB_ADDSTRING, 0,
                          (LPARAM) (LPCTSTR) pArrayOfTTFInfo[idx].plf ->
                          lfFaceName);
          }

          SendMessage(GetDlgItem(hDlg,  IDL_FONTFACE), LB_SELECTSTRING,
                      (WPARAM) -1, (LPARAM) (LPCTSTR)
                      pArrayOfTTFInfo[nCurrentFont].plf -> lfFaceName);
          SetFocus(GetDlgItem(hDlg, IDL_FONTFACE));
          return TRUE;
        case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case LBN_SELCHANGE:
            {
                  nItem = SendMessage(GetDlgItem(hDlg, IDL_FONTFACE),
                                      LB_GETCURSEL, 0, 0);
                  SendMessage(GetDlgItem(hDlg, IDL_FONTFACE),
                              LB_GETTEXT, nItem, (LPARAM)
                              szFontFaceName);

                  for(idx = 0; wcscmp(szFontFaceName, pArrayOfTTFInfo[idx].plf ->
                                      lfFaceName); idx++);

                  if(idx < nFaces) {
                      nCurrentFont = idx;
                  }
                  HFONT hFontSample;

                  hFontSample  =  CreateFont(-28, 0, 0, 0, FW_NORMAL, FALSE,
                                               FALSE, FALSE,
                                               pArrayOfTTFInfo[nCurrentFont].plf->lfCharSet,
                                               pArrayOfTTFInfo[nCurrentFont].plf->lfOutPrecision,
                                               pArrayOfTTFInfo[nCurrentFont].plf->lfClipPrecision,
                                               pArrayOfTTFInfo[nCurrentFont].plf->lfQuality,
                                               pArrayOfTTFInfo[nCurrentFont].plf->lfPitchAndFamily,
                                               pArrayOfTTFInfo[nCurrentFont].plf->lfFaceName);

                  SendDlgItemMessage(hDlg, IDC_FONTSAMPLE, WM_SETFONT,
                                     (WPARAM) hFontSample, 0L);
                  SetDlgItemText(hDlg, IDC_FONTSAMPLE,
                                  pArrayOfTTFInfo[nCurrentFont].plf ->
                                  lfFaceName);
                  DeleteObject(hFontSample);
                return TRUE;
            }
            case LBN_DBLCLK:
                nItem = SendMessage(GetDlgItem(hDlg, IDL_FONTFACE),
                                    LB_GETCURSEL, 0, 0);
                SendMessage(GetDlgItem(hDlg, IDL_FONTFACE), LB_GETTEXT, nItem,
                            (LPARAM) szFontFaceName);

                for(idx = 0; wcscmp(szFontFaceName, pArrayOfTTFInfo[idx].plf ->
                                    lfFaceName); idx++);

                if(idx < nFaces)
                    nCurrentFont = idx;

                lstrcpy(LogFont.lfFaceName, pArrayOfTTFInfo[nCurrentFont].plf ->
                        lfFaceName);
                EndDialog(hDlg, TRUE);

                return TRUE;
          }
          break;
        case IDOK:
          lstrcpy(LogFont.lfFaceName, pArrayOfTTFInfo[nCurrentFont].plf ->
                  lfFaceName);
          EndDialog(hDlg, TRUE);
          return TRUE;
        case IDCANCEL:
          EndDialog(hDlg, FALSE);
          return TRUE;
        default:
          return FALSE;
    }
    return FALSE;
}

BOOL APIENTRY SelectFontStyleDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam)
{
    HWND hListBox;
    TCHAR szFontFaceStyle[4][10] = {L"粗体", L"斜体", L"加下划线", L"加删除线"};
    int idx;
    static BOOL bBold = FALSE, bItalic = FALSE, bUnderLine = FALSE, StrikeOut =
      FALSE;

    switch(message) {
      case WM_INITDIALOG:
          for(idx = 0; idx < 4;  idx ++) {
              SendMessage(GetDlgItem(hDlg, IDL_FONTSTYLE), LB_ADDSTRING, 0,
                          (LPARAM) (LPCSTR) szFontFaceStyle[idx]);
          }

          SetFocus(GetDlgItem(hDlg, IDL_FONTSTYLE));

          if(bBold)
              SendMessage(GetDlgItem(hDlg,  IDL_FONTSTYLE),
                          LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(0,0));
          if(bItalic)
              SendMessage(GetDlgItem(hDlg,  IDL_FONTSTYLE),
                          LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(1,1));
          if(bUnderLine)
              SendMessage(GetDlgItem(hDlg,  IDL_FONTSTYLE),
                          LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(2,2));
          if(StrikeOut)
              SendMessage(GetDlgItem(hDlg,  IDL_FONTSTYLE),
                          LB_SELITEMRANGE, (WPARAM) TRUE, MAKELPARAM(3,3));

          return TRUE;
      case WM_COMMAND:
          switch(LOWORD(wParam)) {
            case IDOK:
              hListBox = GetDlgItem(hDlg, IDL_FONTSTYLE);
              bBold = (SendMessage(hListBox, LB_GETSEL,
                                   (WPARAM) 0, (LPARAM) 0) > 0) ? TRUE : FALSE;
              LogFont.lfWeight = bBold ? FW_BOLD : FW_NORMAL;

              bItalic = (SendMessage(hListBox, LB_GETSEL,
                                   (WPARAM) 1, (LPARAM) 0) > 0) ? TRUE : FALSE;
              LogFont.lfItalic = bItalic;

              bUnderLine = (SendMessage(hListBox, LB_GETSEL,
                                   (WPARAM) 2, (LPARAM) 0) > 0) ? TRUE : FALSE;
              LogFont.lfUnderline = bUnderLine;

              StrikeOut = (SendMessage(hListBox, LB_GETSEL, (WPARAM) 3, (LPARAM) 0) > 0) ? TRUE : FALSE;
              LogFont.lfStrikeOut = StrikeOut;

              EndDialog(hDlg, TRUE);

              return TRUE;
            case IDCANCEL:
              EndDialog(hDlg, FALSE);
              return TRUE;
            default:
              return FALSE;
          }
    }
    return FALSE;
}


BOOL APIENTRY SelectFontOrientDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam)
{
    TCHAR szFontOrientation[20];
    int idx;
    int nSelItem;
    static int nCurFontOrientation = 0;

    switch(message) {
      case WM_INITDIALOG:
        for (idx = 0; idx < 360; idx+=5) {
            wsprintf(szFontOrientation, TEXT("倾斜 %d 度"), (idx <= 180) ? idx : (idx - 360));

            SendMessage(GetDlgItem(hDlg, IDL_FONTORIENT), LB_ADDSTRING,
                        0, (LPARAM) (LPCSTR) szFontOrientation);

        }

        SetFocus(GetDlgItem(hDlg, IDL_FONTORIENT));

        wsprintf(szFontOrientation, TEXT("倾斜 %d 度"),
                (nCurFontOrientation <= 180) ? nCurFontOrientation :
                (nCurFontOrientation - 360));
        SendMessage(GetDlgItem(hDlg, IDL_FONTORIENT), LB_SELECTSTRING,
                    (WPARAM) -1, (LPARAM) (LPCSTR) szFontOrientation);
        return TRUE;
      case WM_COMMAND:
        switch(LOWORD(wParam)) {
          case IDOK:
          case IDL_FONTFACE:
            if(LOWORD(wParam) == IDOK || HIWORD(wParam) == LBN_DBLCLK) {
                nSelItem = SendMessage(GetDlgItem(hDlg, IDL_FONTORIENT),
                                       LB_GETCURSEL, 0, 0);
                nCurFontOrientation = nSelItem * 5;

                LogFont.lfEscapement = LogFont.lfOrientation = nCurFontOrientation * 10;

                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            break;
          case IDCANCEL:
            EndDialog(hDlg, FALSE);
            return TRUE;
          default:
            return FALSE;
        }
    }
    return FALSE;
}


BOOL APIENTRY SelectFontColorDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam)
{
    LPDRAWITEMSTRUCT    lpdis;
    LPMEASUREITEMSTRUCT lpmis;
    TEXTMETRIC          tm;
    int                 nItem;
    COLORREF            crFontColor;
    RECT                rect;
    TCHAR               szItemString[10];
    static HWND         hListBoxFontColor;
    static int          nItemCurSel = 0;

    switch(message) {
      case WM_INITDIALOG:
        hListBoxFontColor = GetDlgItem(hDlg, IDL_FONTCOLOR);
        nItem = SendMessage(hListBoxFontColor, LB_ADDSTRING,
                            0, (LPARAM) L"黑");
        SendMessage(hListBoxFontColor, LB_SETITEMDATA, nItem,
                    (LPARAM) RGB(0, 0, 0));
        nItem = SendMessage(hListBoxFontColor, LB_ADDSTRING,
                            0, (LPARAM) L"红");
        SendMessage(hListBoxFontColor, LB_SETITEMDATA, nItem,
                    (LPARAM) RGB(255, 0, 0));
        nItem = SendMessage(hListBoxFontColor, LB_ADDSTRING,
                            0, (LPARAM) L"绿");
        SendMessage(hListBoxFontColor, LB_SETITEMDATA, nItem,
                    (LPARAM) RGB(0, 255, 0));
        nItem = SendMessage(hListBoxFontColor, LB_ADDSTRING,
                            0, (LPARAM) L"蓝");
        SendMessage(hListBoxFontColor, LB_SETITEMDATA, nItem,
                    (LPARAM) RGB(0, 0, 255));
        nItem = SendMessage(hListBoxFontColor, LB_ADDSTRING,
                            0, (LPARAM) L"黄");
        SendMessage(hListBoxFontColor, LB_SETITEMDATA, nItem,
                    (LPARAM) RGB(255, 255, 0));
        nItem = SendMessage(hListBoxFontColor, LB_ADDSTRING,
                            0, (LPARAM) L"灰");
        SendMessage(hListBoxFontColor, LB_SETITEMDATA, nItem,
                    (LPARAM) RGB(150, 150, 150));

        SendMessage(hListBoxFontColor, LB_SETCURSEL,
                    (WPARAM) nItemCurSel, 0);
        return TRUE;
      case WM_DRAWITEM:
        lpdis = (LPDRAWITEMSTRUCT) lParam;

        if(lpdis -> itemID == -1) {
            break;
        }

        switch (lpdis -> itemAction) {
          case ODA_DRAWENTIRE:
          case ODA_SELECT:
            crFontColor = (COLORREF) SendMessage(lpdis -> hwndItem,
                                                 LB_GETITEMDATA, lpdis->itemID,
                                                 (LPARAM) 0);
            rect.left = (lpdis -> rcItem).left +4;
            rect.top = (lpdis -> rcItem).top + 4;
            rect.right = (lpdis -> rcItem).right - 30;
            rect.bottom = (lpdis -> rcItem).bottom - 4;

            FillRect(lpdis -> hDC, (LPRECT) &rect,
                     CreateSolidBrush(crFontColor));
            SendMessage(lpdis -> hwndItem, LB_GETTEXT, lpdis ->itemID,
                        (LPARAM) szItemString);
            GetTextMetrics (lpdis -> hDC, &tm);
            SetTextColor (lpdis -> hDC, crFontColor);

            TextOut(lpdis -> hDC, rect.right +4,
                    (lpdis -> rcItem.top + lpdis -> rcItem.bottom - tm.tmHeight)/2,
                    szItemString,
                    lstrlen(szItemString));

            if(lpdis -> itemState & ODS_SELECTED) {
                rect.left = (lpdis -> rcItem).left + 10;
                rect.top = (lpdis -> rcItem).top + 10;
                rect.right = (lpdis -> rcItem).right - 36;
                rect.bottom = (lpdis -> rcItem).bottom - 10;

                InvertRect(lpdis->hDC, (LPRECT) &rect);
            }
            break;
          case ODA_FOCUS:
            break;
        }

        /* 返回TRUE, 表示本过程已经处理了该条消息 */
        return TRUE;
        break;
      case WM_MEASUREITEM:
        lpmis = (LPMEASUREITEMSTRUCT) lParam;

        lpmis -> itemHeight = 30;
        break;
      case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
        break;
      case WM_COMMAND:
        switch(LOWORD(wParam)) {
          case IDOK:
            nItemCurSel = SendMessage(hListBoxFontColor,
                                      LB_GETCURSEL, 0, 0);
            crTextColor = (COLORREF) SendMessage(hListBoxFontColor,
                                                 LB_GETITEMDATA, (WPARAM)
                                                 nItemCurSel, 0);
          case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return TRUE;
            break;
          default:
            return FALSE;
            break;
        }
      default:
        return FALSE;
    }
    return TRUE;
}


BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message) {
    case WM_INITDIALOG:
      return TRUE;
    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
          EndDialog(hDlg, TRUE);
          return TRUE;
      }
      break;
  }

  return FALSE;
}
