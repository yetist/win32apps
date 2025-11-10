// 菜 单 和 后

// menu constants

#define IDM_ABOUT       200
#define IDM_EXIT        300
#define IDM_FONT        900
#define IDM_SIZE        800
#define IDM_PAGEUP      750
#define IDM_PAGEDOWN    751
#define IDM_NULL        752
#define IDM_BOLD        100
#define IDM_ITALIC      101
#define IDM_UNDERLINE   102
#define IDM_STRIKEOUT   103
#define IDM_ESC00       410
#define IDM_ESC05       411
#define IDM_ESC10       412
#define IDM_ESC30       413
#define IDM_ESC60       414
#define IDM_ESC90       415
#define IDM_ESC135      416
#define IDM_ESC180      417
#define IDM_ESC315      418
#define IDM_ESC350      419
#define IDM_ESC351      420
#define IDM_ALIGNLEFT   450
#define IDM_ALIGNCENTER 451

#define IDM_ALIGNRIGHT  452

#define IDM_INCSPACE    460
#define IDM_DECSPACE    461
// 其他常量

#define IDC_STATIC      -1
#define NUM_POINTS      16

// 保存系统中可用的TrueType 字体信息的结构
typedef struct tagTrueTypeFontInfo {
    LOGFONT *plf;           // 逻辑字体结构
    TEXTMETRIC *ptm;        // 物理字体结构

} TRUETYPEFONTINFO,* PTRUETYPEFONTINFO;

// 函数原型说明

// 过程
LRESULT APIENTRY MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY About(HWND, UINT, WPARAM, LPARAM);

// 字体枚举回调函数
int APIENTRY MyEnumFontFacesProc(LPLOGFONT, LPTEXTMETRIC, DWORD, LPVOID);
int APIENTRY MyEnumFontCountProc(LPLOGFONT, LPTEXTMETRIC, DWORD, LPINT);

// 其他函数
BOOL InitApplication (HINSTANCE);
BOOL InitInstance (HINSTANCE, int);
PTRUETYPEFONTINFO BuildFontList (HDC, LPINT);
VOID FillMenu(HMENU, HMENU);

VOID ChangeFontMenu(HMENU, int);
VOID ChangeFaceName(HWND, LPTSTR);
VOID ChangePointSize(HWND, int);
