/*************************************
*
* 程序, Menu.c
* 用途,菜单的使用示范:菜单的创建、修改、显示等功能的实现。
*/

#include <windows.h>
#include <string.h>
#include "menu.h"

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

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return (msg.wParam);
}

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcexMenuApp;

    wcexMenuApp.cbSize = sizeof (WNDCLASSEX);
    wcexMenuApp.style = 0;
    wcexMenuApp.lpfnWndProc = (WNDPROC) MainWndProc;
    wcexMenuApp.cbClsExtra = 0;
    wcexMenuApp.cbWndExtra = 0;
    wcexMenuApp.hInstance = hInstance;
    wcexMenuApp.hIcon = LoadIcon (hInstance, IDI_APPLICATION);

    wcexMenuApp.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcexMenuApp.hbrBackground = GetStockObject (WHITE_BRUSH);
    wcexMenuApp.lpszMenuName = L"MenuAppMenu";
    wcexMenuApp.lpszClassName = L"MenuAppWClass";
    wcexMenuApp.hIconSm = LoadIcon (hInstance, IDI_WINLOGO);


    return (RegisterClassEx (&wcexMenuApp));
}

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    hWnd = CreateWindow (
        L"MenuAppWClass",
        L"菜单示范程序",
        WS_OVERLAPPEDWINDOW ,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance , NULL);

    if (!hWnd)
        return (FALSE);

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);
    return (TRUE);
}

LRESULT APIENTRY MainWndProc(HWND hWnd,
                             UINT message,
                             WPARAM wParam,
                             LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    UINT idCommand ;

    static BOOL bFontStyle[4];
    static HBITMAP hBmpCheckMark , hBmpUnCheckMark ;
    LPMEASUREITEMSTRUCT lpmis;

    switch (message)
    {
        case WM_CREATE:
            InitAlignMenu (hWnd);
            hBmpCheckMark = LoadBitmap (hInst, L"CheckMark");
            hBmpUnCheckMark = LoadBitmap (hInst, L"UnCheckMark");
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_BOLD, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_ITALIC, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_UNDERLINE, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_STRIKEOUT, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_SMALLFONTSIZE, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_NORMALFONTSIZE, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);
            SetMenuItemBitmaps (GetMenu(hWnd), IDM_LARGEFONTSIZE, MF_BYCOMMAND, hBmpUnCheckMark, hBmpCheckMark);

            // 对菜单项的状态进行初始化 。
            CheckMenuRadioItem(GetMenu(hWnd), IDM_ALIGNLEFT, IDM_ALIGNRIGHT, IDM_ALIGNLEFT, MF_BYCOMMAND);
            CheckMenuRadioItem(GetMenu(hWnd), IDM_SMALLFONTSIZE, IDM_LARGEFONTSIZE, IDM_SMALLFONTSIZE, MF_BYCOMMAND);
            CheckMenuRadioItem(GetMenu(hWnd), IDM_FONTBLACK, IDM_FONTGRAY, IDM_FONTBLACK, MF_BYCOMMAND) ;

            EnableMenuItem (GetMenu(hWnd), IDM_EDITPASTE, MF_BYCOMMAND | MF_GRAYED);
            bFontStyle[0] = bFontStyle[1] = bFontStyle[2] = bFontStyle[3] = FALSE;
            CheckMenuItem (GetMenu(hWnd), IDM_BOLD, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem (GetMenu(hWnd), IDM_ITALIC, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem (GetMenu(hWnd), IDM_UNDERLINE, MF_BYCOMMAND | MF_UNCHECKED);
            CheckMenuItem (GetMenu(hWnd), IDM_STRIKEOUT, MF_BYCOMMAND | MF_UNCHECKED);
            break ;

        case WM_PAINT:
            {
                WCHAR  szText[] = L"请用鼠标右按钮单击,试一试弹出式菜单";

                hdc = BeginPaint(hWnd, &ps);
                TextOut(hdc, 20, 20, szText, lstrlen(szText));
                EndPaint(hWnd, &ps);
            }
            break;
        case WM_SIZE:
            break;

        case WM_COMMAND:

            idCommand = LOWORD( wParam );
            switch( idCommand )
            {

                case IDM_BOLD:
                case IDM_ITALIC:
                case IDM_UNDERLINE:
                case IDM_STRIKEOUT:
                    bFontStyle[idCommand - IDM_BOLD]
                        = !bFontStyle[idCommand -IDM_BOLD];
                    CheckMenuItem (GetMenu(hWnd),
                                   idCommand,
                                   MF_BYCOMMAND | bFontStyle[idCommand -IDM_BOLD]? MF_CHECKED: MF_UNCHECKED);
                    break;

                case IDM_SHOWTOOLBAR:
                case IDM_SHOWSTATUSBAR:
                    if (GetMenuState(GetMenu(hWnd), idCommand, MF_BYCOMMAND) & MF_CHECKED)
                        CheckMenuItem(GetMenu(hWnd), idCommand, MF_BYCOMMAND | MF_UNCHECKED);
                    else
                        CheckMenuItem(GetMenu(hWnd) , idCommand , MF_BYCOMMAND | MF_CHECKED) ;
                    break;
                case IDM_ALIGNLEFT:
                case IDM_ALIGNCENTER:
                case IDM_ALIGNRIGHT:
                    CheckMenuRadioItem(GetMenu(hWnd), IDM_ALIGNLEFT, IDM_ALIGNRIGHT, idCommand, MF_BYCOMMAND);
                    break ;
                case IDM_SMALLFONTSIZE:
                case IDM_NORMALFONTSIZE:
                case IDM_LARGEFONTSIZE:
                    CheckMenuRadioItem(GetMenu(hWnd), IDM_SMALLFONTSIZE, IDM_LARGEFONTSIZE, idCommand, MF_BYCOMMAND);
                    break ;
                case IDM_FONTBLACK:
                case IDM_FONTRED:
                case IDM_FONTGREEN:
                case IDM_FONTBLUE:
                case IDM_FONTYELLOW:
                case IDM_FONTGRAY:
                    CheckMenuRadioItem(GetMenu(hWnd), IDM_FONTBLACK, IDM_FONTGRAY, idCommand, MF_BYCOMMAND);
                    break;
                case IDM_EDITUNDO:
                    EnableMenuItem(GetMenu(hWnd), IDM_EDITUNDO, MF_BYCOMMAND | MF_GRAYED);
                    break;
                case IDM_EDITCOPY:
                case IDM_EDITCUT:
                    EnableMenuItem(GetMenu(hWnd), IDM_EDITPASTE, MF_BYCOMMAND | MF_ENABLED);
                case IDM_EDITPASTE:
                    EnableMenuItem(GetMenu(hWnd), IDM_EDITUNDO, MF_BYCOMMAND | MF_ENABLED);
                    break;
                case IDM_EDITDELETE:
                    EnableMenuItem(GetMenu(hWnd), IDM_EDITPASTE, MF_BYCOMMAND | MF_GRAYED);
                    EnableMenuItem(GetMenu(hWnd), IDM_EDITUNDO, MF_BYCOMMAND | MF_ENABLED);
                    break;
                case IDM_EDITSELECTALL:
                case IDM_SELECTFONTFACE:
                case IDM_SELECTFONTSTYLE:
                    MessageBox(NULL, L"在本示范程序中,该命令的功能暂缺...", L"命令", MB_OK);
                    break;
                case IDM_EXIT:
                    PostQuitMessage(0);
                    break;
                case IDM_ABOUT:
                    DialogBox(hInst, L"AboutBox", hWnd, (DLGPROC)About);
                    break;
                default:
                    return (DefWindowProc(hWnd , message, wParam, lParam));
            }
            break ;

        case WM_RBUTTONDOWN:
            {
                RECT  rectClient, rect;
                POINT point;

                /* 检取鼠标右按钮单击时的窗口坐标位置。*/
                point.x = LOWORD (lParam);
                point.y = HIWORD (lParam ) ;

                /* 检取窗口客户区限定矩形 */
                GetClientRect(hWnd,(LPRECT) &rectClient);

                rect.left = rectClient.left;
                rect.top = rectClient.top;
                rect.right = rectClient.right/2;
                rect.bottom = rectClient.bottom/2;

                if (PtInRect ((LPRECT)&rect, point))
                    HandleTrackPopupMenu(hWnd, L"EditPopupMenu", point);

                rect.left = rectClient.right/2;
                rect.top = rectClient.bottom/2;
                rect.right = rectClient.right;
                rect.bottom = rectClient.bottom;

                if (PtInRect ((LPRECT) &rect, point))
                    HandleTrackPopupMenu(hWnd, L"FontStylePopupMenu", point);
            }
            break;

        case WM_MEASUREITEM:
            lpmis = (LPMEASUREITEMSTRUCT) lParam;

            /* 指定自画菜单项的高度和宽度。*/
            lpmis->itemHeight = 25;
            lpmis->itemWidth = 60;
            return (TRUE);
            break;

        case WM_DRAWITEM:
            HandleDrawItem (hWnd, (LPDRAWITEMSTRUCT) lParam);
            return (TRUE);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (0);
}

/**************************************
函数 : InitAlignMenu(HWND)
用途 : 在主菜单中插入格式]下拉菜单,并追加位图菜单项。
***************************************/

VOID InitAlignMenu(HWND hWnd)
{
    HBITMAP hBmpAlignLeft;      /* 左对齐方式位图句柄*/
    HBITMAP hBmpAlignCenter ;   /* 居中对齐方式位囹旬柄*/
    HBITMAP hBmpAlignRight ;    /* 右对齐方式位图句柄*/
    HMENU   hMenuMain;          /* 主菜单句柄*/
    HMENU   hMenuAlign;         /* [格式]下拉菜单句柄*/
    HMENU   hMenuFont;
    HMENU   hMenuFontColor;

    /* 装入三种对齐方式的菜单项对应的位图资源。*/
    hBmpAlignLeft = LoadBitmap (hInst, L"AlignLeft");
    hBmpAlignCenter = LoadBitmap (hInst, L"AlignCenter");
    hBmpAlignRight = LoadBitmap (hInst, L"AlignRight");

    /* 检取主菜单的句柄 。*/
    hMenuMain = GetMenu (hWnd);
    hMenuFont = GetSubMenu (hMenuMain, 3);

    hMenuFontColor = CreatePopupMenu () ;
    InsertMenu (hMenuFont, (UINT)3, MF_BYPOSITION | MF_STRING | MF_POPUP,
                (UINT_PTR)hMenuFontColor, L"字体颜色(&C)");


    AppendMenu (hMenuFontColor, MF_OWNERDRAW | MF_BYCOMMAND,
                IDM_FONTBLACK, (LPCTSTR) RGB (0,0,0));

    AppendMenu (hMenuFontColor, MF_OWNERDRAW | MF_BYCOMMAND,
                IDM_FONTRED, (LPCTSTR) RGB (255,0,0));

    AppendMenu (hMenuFontColor, MF_OWNERDRAW | MF_BYCOMMAND,
                IDM_FONTGREEN, (LPCTSTR) RGB (0,255,0));

    AppendMenu (hMenuFontColor, MF_OWNERDRAW | MF_BYCOMMAND,
                IDM_FONTBLUE, (LPCTSTR) RGB (0,0,255));

    AppendMenu (hMenuFontColor, MF_OWNERDRAW | MF_BYCOMMAND,
                IDM_FONTYELLOW, (LPCTSTR) RGB (255,255,0));

    AppendMenu (hMenuFontColor, MF_OWNERDRAW | MF_BYCOMMAND,
                IDM_FONTGRAY, (LPCTSTR) RGB (150,150,150));

    /* 创建一个初始为空的下拉式主菜单。*/

    hMenuAlign = CreatePopupMenu ();

    /* 把新的下拉式菜单插入到主菜单的第四个位置,并加上字符串。*/
    InsertMenu (hMenuMain, (UINT)3, MF_BYPOSITION | MF_STRING | MF_POPUP,
                (UINT_PTR) hMenuAlign, L"格式(&M)");
    /* 把三种对齐方式的位图菜单项分别追加到[格式]菜单中。*/

    AppendMenu (hMenuAlign, MF_BITMAP, IDM_ALIGNLEFT, (LPCTSTR) hBmpAlignLeft);
    AppendMenu (hMenuAlign, MF_BITMAP, IDM_ALIGNCENTER, (LPCTSTR) hBmpAlignCenter);
    AppendMenu (hMenuAlign, MF_BITMAP, IDM_ALIGNRIGHT, (LPCTSTR) hBmpAlignRight);
    return;
}

/**************************************
函 数 : HandlePopupMenu(HWND,POINT)
用 途 : 创建并显示一个浮动的弹出式菜单
*************************************/

VOID HandleTrackPopupMenu(HWND hWnd,
                          LPCTSTR lpTrackPopupMenuName,
                          POINT point)
{
    HMENU hMenu;
    HMENU hMenuTrackPopup;

    hMenu = LoadMenu (hInst, lpTrackPopupMenuName);
    if (hMenu == NULL)
        return;

    hMenuTrackPopup = GetSubMenu (hMenu, 0);
    InitTrackPopupMenu (GetMenu(hWnd), hMenuTrackPopup);

    ClientToScreen(hWnd, (LPPOINT) &point);
    TrackPopupMenu (hMenuTrackPopup,                    // 弹出式菜单的句柄
                    TPM_LEFTALIGN | TPM_RIGHTBUTTON,    // 屏幕位置对齐方式以及鼠标按钮标志
                    point.x, point.y,                   // 弹出式菜单的屏幕坐标位置
                    0,                                  // 保留,必须设定为0
                    hWnd,                               // 所属窗口句柄
                    NULL);                              // 没有定义不撒销弹出式菜单的额外区域

    DestroyMenu (hMenu);
    return;
}

VOID InitTrackPopupMenu(HMENU hMenuMain, HMENU hMenuTrackPopup)
{
    static UINT arrayCommand[8] = {
        IDM_EDITPASTE,
        IDM_BOLD, IDM_ITALIC, IDM_UNDERLINE, IDM_STRIKEOUT ,
        IDM_ALIGNLEFT, IDM_ALIGNCENTER, IDM_ALIGNRIGHT};
    MENUITEMINFO MenuItemInfo;

    int index;

    MenuItemInfo.cbSize = sizeof (MENUITEMINFO) ;
    MenuItemInfo.fMask = MIIM_STATE | MIIM_CHECKMARKS;

    for(index=0; index<8; index++ )
    {
        if (index > 4)
            MenuItemInfo.fMask |= MIIM_TYPE;
        GetMenuItemInfo (hMenuMain, arrayCommand[index],
                         MF_BYCOMMAND, &MenuItemInfo);
        SetMenuItemInfo (hMenuTrackPopup, arrayCommand[index],
                         MF_BYCOMMAND, &MenuItemInfo);
    }
    return;
}

/**************************************
函 数 : HandleDrawItem (HWND, LPDRAWITEMSTRUCT)
作 用 : 处理主窗口中的WM_DRAWITEM 消息, 自画菜单项 。
*/

VOID HandleDrawItem(HWND hWnd, // 窗口句柄
                    LPDRAWITEMSTRUCT lpdis // WM_DRAWITEM 消息所带前 DRAWITEMSTRUCT绪构的地址
                    )
{
    HBRUSH hbr;
    RECT rect;
    COLORREF crFontColor;

    switch (lpdis->itemAction)
    {
        case ODA_DRAWENTIRE:
        case ODA_SELECT:
            crFontColor=(COLORREF)lpdis->itemData;//检取自画菜单项对应的颜色值。

            //按照上面检取的颜色,在菜单项区域中画一个矩形。
            rect. left = (lpdis->rcItem).left + 30;
            rect. top = (lpdis->rcItem ).top + 4;
            rect. right = (lpdis->rcItem).right - 4;
            rect. bottom = (lpdis->rcItem).bottom - 4;

            hbr = CreateSolidBrush (crFontColor);
            FillRect (lpdis->hDC, (LPRECT)&rect, hbr);
            DeleteObject(hbr);

            //如果该菜单是处在被选择的状态,则再在大矩形中画一个反色的小矩形。
            if (lpdis->itemState & ODS_SELECTED)
            {
                rect.left = (lpdis->rcItem).left +34;
                rect.top = (lpdis->rcItem).top +10;
                rect.right = (lpdis->rcItem).right-8;
                rect.bottom = (lpdis->rcItem).bottom -10;

                InvertRect(lpdis->hDC, (LPRECT)&rect);
            }

            //如果该菜单项是要求整个重画,那么还要在菜单项前端重画选中标志。
            //如果是未选中的,则画一个空心圆。
            //如果是选中的,就画一个实心圆。

            if (lpdis->itemAction == ODA_DRAWENTIRE)
            {
                rect.left = (lpdis->rcItem).left +4;
                rect.top = (lpdis->rcItem).top +4;
                rect.right = (lpdis->rcItem).left + 21;
                rect.bottom = (lpdis->rcItem).bottom -4;
                if(lpdis->itemState & ODS_CHECKED)
                {
                    hbr = CreateSolidBrush ((crFontColor));
                }
                else
            {
                    hbr = CreateSolidBrush (GetSysColor (COLOR_MENU));
                }
                SelectObject(lpdis->hDC, hbr);
                Ellipse(lpdis->hDC, rect.left, rect.top, rect.right , rect.bottom);
                DeleteObject(hbr);
            }
            break;
        case ODA_FOCUS:
            break;
    }
    return;
}

BOOL APIENTRY About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
