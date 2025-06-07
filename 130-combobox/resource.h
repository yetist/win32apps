#define IDM_ABOUT               200
#define IDM_EXIT                300

#define IDM_FONT                900
#define IDM_SELECTFONTFACE       760
#define IDM_SELECTFONTSTYLE     761
#define IDM_SELECTFONTORIENT    762
#define IDM_SELECTFONTSIZE      763
#define IDM_SELECTFONTCOLOR     764

#define IDM_SIZE                800
#define IDM_PAGEUP              750
#define IDM_PAGEDOWN            751
#define IDM_NULL                752

#define IDM_EDITUNDO            770
//#define IDM REDO
#define IDM_EDITCUT             771
#define IDM_EDITCOPY            772
#define IDM_EDITPASTE           773
#define IDM_EDITDELETE          774
#define IDM_EDITSELECTALL       775

#define IDM_BOLD                100
#define IDM_ITALIC              101
#define IDM_UNDERLINE           102
#define IDM_STRIKEOUT           103

//对话框中的控件标识号
#define IDC_STATIC              -1
#define IDL_FONTFACE            600
#define IDL_FONTSTYLE           601
#define IDE_FONTORIENT          602
#define IDE_FONTSIZE            603
#define IDE_PASSWORD            604

#define IDC_FONTFACE            610
#define IDC_FONTSTYLE           612
#define IDC_FONTORIENT          613
#define IDC_FONTSIZE            614
#define IDC_FONTCOLOR           615

#define ID_EDITCONTROL          605
#define IDM_TEXTSTRING          620
#define IDM_FONTSIZE            621

//其他常量
#define NUM_POINTS              16
#define BUF_SIZE                1024

#define WM_ENTER                (WM_USER)
#define WM_TAB                  (WM_USER+1)
#define WM_ESC                  (WM_USER+2)

//保存系统中可用的True Type字体信息的结构
typedef struct tagTrueTypeFontInfo {
    LOGFONT * plf;
    TEXTMETRIC * ptm;
} TRUETYPEFONTINFO, *PTRUETYPEFONTINFO;

//函数原型说明

// 过程
LRESULT APIENTRY MainWndProc(HWND, UINT, WPARAM,LPARAM);
LRESULT CALLBACK SubClassProc(HWND,UINT,WPARAM,LPARAM);
BOOL APIENTRY About(HWND, UINT,WPARAM, LPARAM);
BOOL APIENTRY SelectFontFaceDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SelectFontStyleDlgProc (HWND, UINT, WPARAM,LPARAM);
BOOL APIENTRY SelectFontOrientDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SelectFontColorDlgProc (HWND, UINT, WPARAM, LPARAM);

// 字体枚举回调函数
int APIENTRY MyEnumFontFacesProc (LPLOGFONT, LPTEXTMETRIC, DWORD, LPVOID);
int APIENTRY MyEnumFontCountProc (LPLOGFONT, LPTEXTMETRIC,DWORD, LPINT);

// 其他函数
BOOL InitApplication (HINSTANCE);
BOOL InitInstance (HINSTANCE, int);
PTRUETYPEFONTINFO BuildFontList (HDC, LPINT);
VOID ChangeFaceName (HWND, LPTSTR);
VOID ChangePointSize (HWND, int);
VOID FillComboBox (void);
