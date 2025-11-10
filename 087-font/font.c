/****************************************************
程序:   Font.c
用途:  说明字体的枚举、创建和使用, 以及文字的格式化输出 。
*************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "font.h"

HINSTANCE hInst;
HWND hWndMain;

int aPoints[NUM_POINTS] = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28,
    36, 48, 72 };

HDC hdcGlobal;
PTRUETYPEFONTINFO pArrayOfTTFInfo;
int iFace;
int nFaces;
BOOL bPage;

int LogPixelSy;
LOGFONT LogFont = {-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE,
    L"abc"
};


LONG Orientation[11]={ 0, 5, 10, 30, 60, 90, 135, 180, 315, 350, 355 };
UINT HTEXTALIGN[3] = { TA_LEFT, TA_CENTER, TA_RIGHT };

int APIENTRY WinMain (HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    HACCEL hAccel;

    if(!InitApplication(hInstance))
        return (FALSE);

    if(!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    hAccel = LoadAccelerators(hInstance, L"FontAccel") ;
    SendMessage (hWndMain, WM_COMMAND, IDM_NULL, 0);

    while (GetMessage (&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hWndMain, hAccel,&msg))
        {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }
    return (msg.wParam);
}

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcexFont;

    wcexFont.cbSize = sizeof (WNDCLASSEX);
    wcexFont.style = 0;
    wcexFont.lpfnWndProc = (WNDPROC) MainWndProc;
    wcexFont.cbClsExtra =10;
    wcexFont.cbWndExtra = 0;
    wcexFont.hInstance = hInstance;

    wcexFont.hIcon = LoadIcon (hInstance, IDI_APPLICATION);
    wcexFont.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcexFont.hbrBackground = GetStockObject (WHITE_BRUSH);
    wcexFont.lpszMenuName = L"FontMenu";
    wcexFont.lpszClassName = L"FontWClass";
    wcexFont.hIconSm = LoadIcon (hInstance, IDI_WINLOGO);

    return (RegisterClassEx (&wcexFont));
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd ;

    hInst = hInstance;

    hWnd = CreateWindow (L"FontWClass",
                         L"字体选择和文字输出示范程序" ,
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return (FALSE);

    hWndMain = hWnd;

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);
    return (TRUE);
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    HDC hDC;                    /* 设备描述表 */
    HFONT hFont;                /* 字体句柄 */
    static HMENU hMenu;         /* 菜单句柄 */
    static HMENU hMenuFont;     /* 字体菜单句柄 */
    static WCHAR  szBuf[128];   /* 文字缓冲区 */
    static BOOL bBold;          /* 粗体标志 */
    static BOOL bItalic;        /* 斜体标志 */
    static BOOL bUnderLine;     /* 加下划线标志 */
    static BOOL bStrikeOut ;    /* 加删除线标志 */
    static UINT AlignHorz;      /* 水平对齐方式 */
    static int nCharSpaceExtra; /* 额外字符间隔 */
    INT uItem;                  /* 菜单和后 */
    RECT Rect;                  /* 矩形结构 */

    switch (message)
    {
        case WM_CREATE:
            {
                /* 检取有关菜单句柄。*/
                hMenu = GetMenu(hWnd);
                hMenuFont = GetSubMenu(GetMenu(hWnd), 2);

                hDC = GetDC(hWnd);
                /* 检取设备的纵向分辨率(每逻辑英寸的像素点的数目)。*/
                LogPixelSy = GetDeviceCaps(hDC, LOGPIXELSY);

                /* 枚举操作系统中可用的TrueType 字体,建立字体信息列表 。*/
                pArrayOfTTFInfo = BuildFontList (hDC, &nFaces);

                /* 把字体字样和字体尺寸值填写到菜单中。*/
                FillMenu(hMenu, hMenuFont);

                /* 选择初始采用的字体字样为TrueType 字体数组中的第一项字体。 */
                lstrcpy(LogFont.lfFaceName, pArrayOfTTFInfo[0].plf -> lfFaceName);
                CheckMenuRadioItem (hMenu, IDM_FONT, IDM_FONT + nFaces - 1, IDM_FONT, MF_BYCOMMAND);

                /* 选择初始字体尺寸为12点。*/
                LogFont.lfHeight = - MulDiv(aPoints[4], LogPixelSy, 72);
                CheckMenuRadioItem (hMenu, IDM_SIZE, IDM_SIZE + NUM_POINTS - 1, IDM_SIZE + 4, MF_BYCOMMAND);

                /* 初始化字体效果。*/
                bItalic = FALSE;
                bUnderLine = FALSE;
                bBold = FALSE;
                bStrikeOut = FALSE;

                /* 初始化文字输出格式。*/
                AlignHorz = TA_CENTER;
                CheckMenuRadioItem (GetMenu(hWnd), IDM_ALIGNLEFT,
                                    IDM_ALIGNRIGHT, IDM_ALIGNCENTER, MF_BYCOMMAND);
                nCharSpaceExtra = 0;

                ReleaseDC (hWnd, hDC);
            }
            break;

        case WM_SIZE:
            SendMessage(hWndMain, WM_COMMAND, IDM_NULL, 0);

            break;
        case WM_COMMAND:
            uItem = LOWORD(wParam);
            switch (uItem)
            {
                case IDM_PAGEUP:
                    ChangeFontMenu (hMenuFont, 0);
                    return 0;

                case IDM_PAGEDOWN:
                    ChangeFontMenu (hMenuFont, 1);
                    return 0;

                case IDM_BOLD:

                    /* 切换粗体标志。*/
                    bBold = !bBold;
                    CheckMenuItem(hMenu, IDM_BOLD,
                                  MF_BYCOMMAND | ( bBold ? MF_CHECKED: MF_UNCHECKED));
                    LogFont.lfWeight = bBold ? FW_BOLD : FW_HEAVY;
                    break;

                case IDM_ITALIC:
                    /* 切换斜体标志。*/
                    bItalic = !bItalic;
                    CheckMenuItem (hMenu, IDM_ITALIC,
                                   MF_BYCOMMAND| (bItalic ? MF_CHECKED : MF_UNCHECKED));
                    LogFont.lfItalic = bItalic;
                    break;

                case IDM_UNDERLINE:
                    /* 切换加下划线标志。*/
                    bUnderLine = !bUnderLine;
                    CheckMenuItem(hMenu, IDM_UNDERLINE,
                                  MF_BYCOMMAND | ( bUnderLine ? MF_CHECKED : MF_UNCHECKED ));
                    LogFont.lfUnderline = bUnderLine;
                    break;

                case IDM_STRIKEOUT:
                    /* 切换加删除线标志。*/
                    bStrikeOut = !bStrikeOut;
                    CheckMenuItem (hMenu, IDM_STRIKEOUT,
                                   MF_BYCOMMAND | (bStrikeOut ? MF_CHECKED : MF_UNCHECKED));
                    LogFont.lfStrikeOut = bStrikeOut;
                    break;

                case IDM_ESC00:
                case IDM_ESC05:
                case IDM_ESC10:
                case IDM_ESC30:
                case IDM_ESC60:
                case IDM_ESC90:
                case IDM_ESC135:
                case IDM_ESC180:
                case IDM_ESC315:
                case IDM_ESC350:
                case IDM_ESC351:
                    /* 改变文字基准线方向。*/
                    CheckMenuRadioItem (hMenu, IDM_ESC00, IDM_ESC351,
                                        uItem, MF_BYCOMMAND);

                    LogFont.lfEscapement =
                        LogFont.lfOrientation = Orientation[uItem -
                        IDM_ESC00] * 10;

                    break;

                case IDM_ALIGNLEFT:
                case IDM_ALIGNCENTER:
                case IDM_ALIGNRIGHT:
                    /* 改变文字对齐方式。*/
                    CheckMenuRadioItem (GetMenu(hWnd), IDM_ALIGNLEFT,
                                        IDM_ALIGNRIGHT, uItem, MF_BYCOMMAND);
                    AlignHorz = HTEXTALIGN[uItem - IDM_ALIGNLEFT];
                    break;

                case IDM_INCSPACE:
                    /* 增加字符之间的间距。*/
                    nCharSpaceExtra++;
                    break;

                case IDM_DECSPACE:
                    /* 减小字符之间的间距。*/
                    nCharSpaceExtra--;
                    break;

                case IDM_EXIT:
                    SendMessage (hWnd, WM_CLOSE, 0, 0L);
                    return 0;

                case IDM_ABOUT:
                    DialogBox (hInst, L"AboutBox", hWnd,(DLGPROC)About);

                    return 0;

                case IDM_NULL:
                    break;

                default:

                    /* 改设字体字样。*/
                    if (uItem >= IDM_FONT && uItem - IDM_FONT + nFaces)
                    {
                        lstrcpy (LogFont.lfFaceName,
                                 pArrayOfTTFInfo[uItem-IDM_FONT].plf -> lfFaceName);
                        CheckMenuRadioItem (hMenu, IDM_FONT, IDM_FONT + nFaces - 1,
                                            uItem, MF_BYCOMMAND);

                    } else if (uItem >= IDM_SIZE && uItem < IDM_SIZE + NUM_POINTS) {
                        /* 改设字体尺寸 */
                        LogFont.lfHeight = - MulDiv(aPoints[uItem -
                                                    IDM_SIZE], LogPixelSy, 72);
                        CheckMenuRadioItem (hMenu, IDM_SIZE,
                                            IDM_SIZE + NUM_POINTS-1,
                                            uItem, MF_BYCOMMAND);
                    } else {
                        return (DefWindowProc(hWnd, message, wParam, lParam));
                    }
                    break;
            }
            /* 删除先前创建的逻辑字体。*/
            if (hFont != NULL)
                DeleteObject(hFont);

            /* 创建逻辑字体。*/
            hFont = CreateFontIndirect(&LogFont);
            if(hFont != NULL)
            {
                hDC = GetDC(hWnd);

                GetClientRect(hWnd ,&Rect) ;
                Rectangle(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom);

                SelectObject (hDC, hFont);
                _swprintf(szBuf, L"系统中共安装有 %d 种TrueType 字体", nFaces);
                TextOut (hDC, 10, 150, szBuf, wcslen(szBuf));

                _swprintf(szBuf, L"当前采用的字体是 %s", LogFont.lfFaceName);
                TextOut (hDC, 10, 10, szBuf, wcslen(szBuf));

                /* 设置文字对齐方式。*/
                SetTextAlign(hDC,AlignHorz | TA_BASELINE); /* 设置文字中字符之间的额外间距。*/
                SetTextCharacterExtra(hDC, nCharSpaceExtra);

                /* 设置文字的前景色和背景色。*/
                SetTextColor(hDC, RGB(0, 0, 255));
                SetBkColor(hDC, RGB(255, 255, 0));

                _swprintf(szBuf, L"当前字符额外间隔为 %d", GetTextCharacterExtra(hDC), nCharSpaceExtra);
                TextOut(hDC, Rect.right/2, Rect.bottom/3, szBuf, wcslen(szBuf));
                ReleaseDC(hWnd, hDC);
            }
            break;

        case WM_DESTROY:
            DeleteObject(hFont);
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}

/**************************************
函数: BuildFontList(HDC, LPINT)
作用: 枚举当前DC的所有TrueType 字体, 并把字体信息复制到所需的
TRUETYPEFONTINFO 结构中。
***************************************/

PTRUETYPEFONTINFO BuildFontList(HDC  hDCFont, // 设备描述表句柄
                                LPINT retnFaces) // 返回字体数目的变量的指针
{
    nFaces = 0; // 全局字体计数变量初始化为0
    hdcGlobal = hDCFont ; // 保存好字体数组对应的设备描述表 save,

    // 以备回调函数中使用。

    // 首先算出系统中可用 TrueType 字体的数目。
    EnumFontFamilies (hdcGlobal, NULL , (FONTENUMPROC)MyEnumFontCountProc,
                      (LPARAM)&nFaces) ;

    // 分配字体信息结构的数组。
    pArrayOfTTFInfo = (PTRUETYPEFONTINFO)LocalAlloc (LPTR ,
                                                     sizeof(TRUETYPEFONTINFO) * (nFaces+1));

    /* 再次枚举所有可用的 TrueType 字体。对于每个字体, 都填写好各自的逻辑字体
* 信息结构 (LOGFONT) 和物理字体信息结构(TEXTMETRIC)。
*/
    iFace = 0;
    EnumFontFamilies (hdcGlobal, NULL ,
                      (FONTENUMPROC)MyEnumFontFacesProc, (LPARAM)NULL);

    *retnFaces = nFaces;
    return pArrayOfTTFInfo;
}

/***************************************
函数: MyEnumFontFacesProc (LPLOGFONT, LPTEXTMETRIC, DWORD, LPVOID) *
用途: 枚举字体回调函数 , 先核查该字体是否为 TrueType 字体 ; 如果是,
则给 LOGFONT 和 TEXTMETRIC 结构分配内存。 然后把这两个结构填写
到可用字体信息数组中。

***************************************/

int APIENTRY MyEnumFontFacesProc(LPLOGFONT lpLogFont,
                                 LPTEXTMETRIC lpTEXTMETRICs,
                                 DWORD fFontType,
                                 LPVOID lpData)
{
    if (fFontType & TRUETYPE_FONTTYPE) /* 确定该字体属于TrueType 型字体。*/
    {
        pArrayOfTTFInfo[iFace].plf = (LPLOGFONT)LocalAlloc (LPTR,
                                                            sizeof (LOGFONT)); // * nFonts);
        pArrayOfTTFInfo[iFace].ptm = (LPTEXTMETRIC)LocalAlloc (LPTR,
                                                               sizeof (TEXTMETRIC) );// % nFonts);

        if ((pArrayOfTTFInfo[iFace].plf == NULL) || (pArrayOfTTFInfo[iFace].ptm == NULL))
        {
            MessageBox (NULL, L"内存分配失败", NULL, MB_OK);
            return FALSE;
        }

        *(pArrayOfTTFInfo[iFace].plf) = *lpLogFont;
        *(pArrayOfTTFInfo[iFace].ptm) = *lpTEXTMETRICs;
        iFace++;
    }
    return TRUE;
}

/******************************************************************
函数: MyEnumFontCountProc(LPLOGFONT, LPTEXTMETRIC, DWORD, LPVOID)
作用: 每次调用本函数, 字体计数变量就增1.
******************************************************************/

int APIENTRY MyEnumFontCountProc(
    LPLOGFONT lpLogFont ,
    LPTEXTMETRIC lpTEXTMETRICs ,
    DWORD fFontType,
    LPINT lpData)
{
    if (fFontType & TRUETYPE_FONTTYPE)
        ( *lpData)++;
    return TRUE;
}

/**************************************
*  函数: FillMenu(HMENU,HMENU )
%  用途: 初始化菜单系统中的字体字样菜单和字体尺寸菜单 。
**************************************/

VOID FillMenu (HMENU hMenuMain, HMENU hMenuFont)
{
    int idx;
    WCHAR szSize[10];
    HMENU hMenuSize;

    // 把可用的字样名称追加到 [字样] 菜单中 。
    for (idx=0; idx < ((nFaces >=10)?10:nFaces); idx++)
    {
        AppendMenu (hMenuFont, MF_STRING | MF_UNCHECKED,
                    IDM_FONT + idx, (LPCWSTR)pArrayOfTTFInfo[idx].plf->lfFaceName);
    }

    // 如果[字样] 菜单中菜单项太多, 则采用分页方式。
    if (nFaces >= 10)
        bPage = TRUE;
    else
        bPage = FALSE;
    if (bPage == TRUE)
        AppendMenu(hMenuFont, MF_STRING | MF_UNCHECKED,
                   IDM_PAGEDOWN, L">(后面的字体)");

    /* 创建一个初始为空的下拉式主菜单。*/
    hMenuSize = CreatePopupMenu ();
    /* 把新的下拉式菜单插入到主菜单的第四个位置, 并加上字符串。*/

    InsertMenu (hMenuMain, (UINT)3, MF_BYPOSITION | MF_STRING | MF_POPUP,
                (UINT_PTR) hMenuSize, L"尺寸(&Z)");

    /* 把预定字体尺寸的菜单项分别追加到 [尺寸] 菜单中。*/
    for (idx=0; idx < NUM_POINTS; idx++ )
    {
        _swprintf (szSize, L"%d", aPoints[idx]);
        AppendMenu (hMenuSize, MF_STRING | MF_UNCHECKED,
                    IDM_SIZE + idx, (LPCWSTR) szSize);
    }
    return;
}

// 更改[字体] 菜单中的菜单项
VOID ChangeFontMenu (HMENU hMenuFont, int nDirect )
{
    int idx;

    INT nFirst, nLast, nStart;

    if (bPage==FALSE)
        return ;
    nFirst = GetMenuItemID(hMenuFont, 1);
    nLast = GetMenuItemID(hMenuFont, 10);
    if (nDirect == 1 && nLast == IDM_FONT + nFaces - 1)
    {
        EnableMenuItem (hMenuFont, IDM_PAGEDOWN, MF_BYCOMMAND | MF_GRAYED);
        return;
    }
    if (nDirect == 0 && nFirst == IDM_FONT )
    {
        EnableMenuItem (hMenuFont, IDM_PAGEUP, MF_BYCOMMAND | MF_GRAYED);
        return;
    }

    if (nDirect == 1)
    {
        if (nLast < IDM_FONT + nFaces - 10)
            nStart = nFirst + 10;
        else
            nStart = IDM_FONT + nFaces - 10;
    }
    else
{
        nStart = nFirst - 10;
        if (nStart < IDM_FONT)
            nStart = IDM_FONT;
    }

    for (idx=0; idx < 10; idx++)
        ModifyMenu(hMenuFont, idx + 1,
                   MF_STRING | MF_BYPOSITION, nStart + idx,
                   (LPCTSTR) pArrayOfTTFInfo[nStart + idx - IDM_FONT].plf->lfFaceName) ;

    EnableMenuItem (hMenuFont, IDM_PAGEDOWN,
                    MF_BYCOMMAND | (nStart==IDM_FONT + nFaces - 10)? MF_GRAYED:MF_ENABLED);
    EnableMenuItem (hMenuFont, IDM_PAGEUP,
                    MF_BYCOMMAND | (nStart == IDM_FONT)? MF_GRAYED: MF_ENABLED);
    return ;
}

BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM IParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog (hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}
