/* vi: set sw=4 ts=4 sts=4 expandtab wrap ai: */
#include <stdlib.h>
#include <windows.h>

BOOL FirstInit (HANDLE);
LRESULT CALLBACK FrameWndProc (HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);
//主窗口函数说明
LRESULT CALLBACK DocWndProc (HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);

//子窗口函数说明
HANDLE hInst;
HMENU hMenu, hMenuDoc;

typedef struct {
    DWORD x;
    DWORD y;
}DOCDATA;

typedef DOCDATA *NPDOCDATA;  //子窗口私有数据结构

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCmd) //主函数
{
    HWND hWndFrame,hWndClient;
    HANDLE hAccel;
    MSG msg;
    if(!hPrevInstance)
    {
        if(!FirstInit (hInstance))
            return FALSE;
    }
    hInst = hInstance;
    hMenu = LoadMenu(hInst, L"MDIMENU"); //主窗口菜单初始化
    hMenuDoc = LoadMenu(hInst, L"MDIMENUDOC"); //子窗口菜单初始化
    hAccel = LoadAccelerators(hInst, L"MdiAccel"); //快捷键资源初始化
    hWndFrame = CreateWindow (L"WIN_MDI FRAME", L"多文档应用程序示范",
                              WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                              CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT,
                              NULL, hMenu, hInstance,NULL); //创建主窗口
    hWndClient= GetWindow (hWndFrame, WS_CHILD);
    ShowWindow (hWndFrame, nShowCmd);
    UpdateWindow (hWndFrame); //显示主窗口
    while(GetMessage(&msg,NULL,0,0)) //启动消息循环
    {
        if(!TranslateMDISysAccel (hWndClient, &msg) &&
            !TranslateAccelerator (hWndFrame, hAccel,&msg))
        {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }
    return msg.wParam;
}

BOOL FirstInit(HANDLE hInstance) //窗口类初始化函数ibm
{
    WNDCLASS wcFirstClass;
    wcFirstClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wcFirstClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wcFirstClass.lpszMenuName  = NULL;
    wcFirstClass.lpszClassName = L"WIN MDI FRAME";
    wcFirstClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wcFirstClass.hInstance     = hInstance;
    wcFirstClass.style         = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
    wcFirstClass.lpfnWndProc   = FrameWndProc;
    wcFirstClass.cbClsExtra    = 0;
    wcFirstClass.cbWndExtra    = 0;
    if(!RegisterClass(&wcFirstClass))//注册主窗口类
        return FALSE;

    wcFirstClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wcFirstClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wcFirstClass.lpszMenuName  = NULL;
    wcFirstClass.lpszClassName = L"WIN MDI DOC";
    wcFirstClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wcFirstClass.hInstance     = hInstance;
    wcFirstClass.style         = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    wcFirstClass.lpfnWndProc   = DocWndProc;
    wcFirstClass.cbClsExtra    = 0; 
    wcFirstClass.cbWndExtra    = sizeof (LOCALHANDLE);
    if(!RegisterClass(&wcFirstClass)) //注册子窗口类
        return FALSE;
    return TRUE;
}

LRESULT CALLBACK FrameWndProc (HWND hWnd, UINT message,
                               WPARAM wParam,LPARAM lParam)//主窗口函数
{
    static HWND hWndClient;
    HWND hWndChild;
    CLIENTCREATESTRUCT clcr;
    MDICREATESTRUCT mdi;
    switch (message)
    {
        case WM_CREATE: //完成用户窗口的创建
            clcr.hWindowMenu = hMenu;
            clcr.idFirstChild = 100;
            hWndClient = CreateWindow(L"MDICLIENT", NULL,
                                      WS_CHILD|WS_CLIPCHILDREN |WS_VISIBLE,
                                      0,0,0,0, hWnd, NULL, hInst, (LPSTR)&clcr);
            return 0L;
        case WM_COMMAND: //菜单消息响应
            switch (LOWORD(wParam)) 
            {
                case 10://创建MDI子窗口
                    mdi.szClass=L"WIN_MDI:DOC";
                    mdi.szTitle=L"子窗口";
                    mdi.hOwner=hInst;
                    mdi.x=CW_USEDEFAULT;
                    mdi.y=CW_USEDEFAULT;
                    mdi.cx=CW_USEDEFAULT;
                    mdi.cy=CW_USEDEFAULT;
                    mdi.style=0;
                    mdi.lParam = (LPARAM) NULL;
                    hWndChild = (HWND)SendMessage (hWndClient, WM_MDICREATE, 0,
                                                   (LPARAM) (LPMDICREATESTRUCT)&mdi);
                    return 0L;
                case 11://结束应用程序
                    SendMessage (hWnd, WM_CLOSE, 0, 0L);
                    return 0L;
                case 30://排列MDI子窗口图标
                    SendMessage (hWndClient, WM_MDIICONARRANGE, 0, 0L);
                    return 0L;
                case 31: //二叠置MDI子窗口
                    SendMessage(hWndClient, WM_MDICASCADE, 0, 0L);
                    return 0L;
                case 32: //并列MDI子窗口
                    SendMessage (hWndClient, WM_MDITILE, 0, 0L);
                    return 0L;
                case 33://激活下一个MDI子窗口
                    SendMessage (hWndClient, WM_MDINEXT, 0, 0L);
                    return 0L;
                case 34://关闭当前MDI子窗口
                    hWndChild=(HWND)SendMessage (hWndClient, WM_MDIGETACTIVE,
                                                 0, 0L);
                    SendMessage (hWndClient, WM_MDINEXT, (WPARAM)hWndChild, 0L);
                    SendMessage (hWndClient, WM_MDIDESTROY, (WPARAM)hWndChild, 0L);
                    return 0L;
                default: //把主窗口不处理的菜单消息传递给子窗口
                    hWndChild = (HWND)SendMessage (hWndClient, WM_MDIGETACTIVE, 0, 0L);
                    if (IsWindow (hWndChild))
                        SendMessage (hWndChild, WM_COMMAND, wParam, lParam);
                    break;
            }
            break;
        case WM_QUERYENDSESSION:
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0); //结束应用程序
            return 0L;
    }
    return (DefFrameProc(hWnd, hWndClient, message, wParam, lParam));
}

//传递主窗口函数不处理的消息
LRESULT CALLBACK DocWndProc (HWND hWnd,UINT message,
                             WPARAM wParam,LPARAM lParam)//子窗口函数
{
    static HWND hWndClient, hWndFrame;
    int i, n;
    LOCALHANDLE hDocData;
    NPDOCDATA npDocData;
    HDC hdc;
    HBRUSH hBrush, hOldBrush;
    HRGN hRgn;
    switch (message)
    {
        case WM_CREATE: //建立子窗口私有数据
            hDocData = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (DOCDATA));
            npDocData = (NPDOCDATA)LocalLock (hDocData);
            npDocData->x = 10;
            npDocData->y = 10;
            SetWindowLong(hWnd, 0, (WPARAM)hDocData);
            LocalUnlock (hDocData);
            hWndClient = GetParent (hWnd);
            hWndFrame = GetParent (hWndClient);
            return 0L;
        case WM_COMMAND: //子窗口菜单消息响应
            switch (LOWORD(wParam))
            {
                case 20: //在一个固定背景区域内，实现一个大小和颜色不断变化的圆
                    LOGBRUSH lbBrush;
                    lbBrush.lbStyle = BS_SOLID;
                    hdc = GetDC(hWnd);
                    SelectObject (hdc, GetStockObject(GRAY_BRUSH));
                    SelectObject (hdc, GetStockObject (BLACK_PEN));
                    Rectangle (hdc, 10, 10, 100, 100);
                    for(n = 0; n < 5; n++)
                    {
                        for (i = 0; i<-40; i++)
                        {
                            lbBrush.lbColor = RGB(5*i, 255-5*i, 0);
                            hBrush = CreateBrushIndirect (&lbBrush);
                            hOldBrush = SelectObject (hdc, hBrush);
                            hRgn = CreateEllipticRgn (50-i, 50-i, 60+i, 60+i);
                            SelectClipRgn (hdc, hRgn);
                            Rectangle (hdc, 10,10,100,100);
                            DeleteObject (hRgn);
                            SelectObject (hdc, hOldBrush);
                            DeleteObject (hBrush);
                            DWORD j,k;
                            for(j = 0; j<1000; j++)
                                for(k = 0; k < 5000; k++) {;}
                        }
                        for (i=40; i >= 0; i--)
                            SelectObject (hdc, GetStockObject (GRAY_BRUSH));
                        SelectObject (hdc, GetStockObject (BLACK_PEN));
                        Rectangle (hdc,10,10,100,100);
                        lbBrush.lbColor=RGB(50*n,255-50*n,0);
                        hBrush = CreateBrushIndirect (&lbBrush);
                        hOldBrush=SelectObject (hdc, hBrush);
                        hRgn=CreateEllipticRgn (50-i, 50-i, 60+i, 60+i);
                        SelectClipRgn (hdc,hRgn);
                        Rectangle (hdc,10,10,100,100);
                        DeleteObject (hRgn);
                        SelectObject (hdc,hOldBrush);
                        DeleteObject (hBrush);
                        DWORD j,k;
                        for (j=0; j<10000;j++)
                            for (k=0;k<1000;k++){;}
                    }
                    return 0L;
                case 21: //测试子窗口私有数据
                    HDC hdc;
                    hdc=GetDC(hWnd);
                    hDocData = (LOCALHANDLE) GetWindowLong (hWnd,0);
                    npDocData = (NPDOCDATA)LocalLock (hDocData);
                    TextOut (hdc, npDocData->x, npDocData->y, L"This window's possition", 24);
                    LocalUnlock (hDocData);
                    return 0L;
                default:
                    return 0L;
            }
        case WM_LBUTTONDBLCLK: //建立当前子窗口私有数据
            HDC hdc;
            hdc = GetDC (hWnd);
            hDocData = (LOCALHANDLE) GetWindowLong(hWnd, 0);
            npDocData = (NPDOCDATA)LocalLock(hDocData);
            npDocData->x = LOWORD(lParam);
            npDocData->y = HIWORD(lParam);
            TextOut (hdc, npDocData->x, npDocData->y, L"Save the Position of DBCLK", 27);
            LocalUnlock (hDocData);
        case WM_MDIACTIVATE: //完成主窗口和子窗口菜单之间的切换
            if (lParam)
                SendMessage(hWndClient, WM_MDISETMENU, (WPARAM)hMenuDoc,0);
            else 
                SendMessage (hWndClient, WM_MDISETMENU, (WPARAM)hMenu, 0);
            DrawMenuBar (hWndFrame);
            return 0L;
        case WM_CLOSE:
        case WM_QUERYENDSESSION:
        case WM_DESTROY:
            hDocData=(LOCALHANDLE)GetWindowLong (hWnd, 0);
            LocalFree (hDocData);
            //关闭当前子窗口
            SendMessage (hWndClient, WM_MDIDESTROY,(WPARAM)hWnd,0L);
            return 0L;
    }

    // 传递子窗口函数不处理的消息
    return (DefMDIChildProc (hWnd, message, wParam, lParam));
}
