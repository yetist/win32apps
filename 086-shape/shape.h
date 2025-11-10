// 菜单和命令

// [封闭图形]菜单

#define IDM_EXIT 100

// [画笔选项] 莱单

// [宽度] 子菜单

#define IDM_W1 200
#define IDM_W2 201
#define IDM_W3 202
#define IDM_W4 203

// [风格] 子菜单
#define IDM_SOLID   210
#define IDM_DASH    211
#define IDM_DOT     212

// [颜色] 子菜单
#define IDM_RED     220
#define IDM_YELLOW  221
#define IDM_GREEN   222
#define IDM_BLUE    223
#define IDM_BLACK   224

// [画刷选项] 菜单

// [实心画刷] 子菜单
#define IDM_BRED    230
#define IDM_BYELLOW 231
#define IDM_BGREEN  232
#define IDM_BBLUE   233

// [库存画刷] 子菜单
#define IDM_BWHITE  240
#define IDM_BLTGRAY 241
#define IDM_BGRAY   242
#define IDM_BBKGRAY 243
#define IDM_BBLACK  244
#define IDM_BNULL   245

// [阴影画刷] 子菜单
#define IDM_BHORIZONTAL 0
#define IDM_BVERTICAL   1
#define IDM_BFDIAGONAL  2
#define IDM_BBDIAGONAL  3
#define IDM_BCROSS      4
#define IDM_BDIAGCROSS  5

// 图标
#define RCICON_ICON     300
#define SMALL_ICON      301
#define IDC_STATIC      -1

// 全局变量

HINSTANCE hInst;
HWND hWndMain;
HMENU hMenu;

// 一个多边形的顶点坐标数组
const POINT mypolygon[5]={{247,10}, {230,90}, {290,35},
        {210,30}, {275,85}};

// 一个多多边形的顶点坐标数组
const POINT mypolypolygon[11] = {
        {228, 122}, {213, 170}, {267, 190}, {262, 140},
        {228, 122}, {262, 139}, {283, 119},
        {267, 190}, {262, 139}, {283, 119}
};

// 一个多多边形的边数目数组
const int npolypolygon[3] = { 4, 3, 3 };

// 实画刷的颜色
const COLORREF crBrush[5] = {
    0x000000ffL,    // 红色
    0x0000ffffL,    // 黄色
    0x0000ff00L,    // 绿色
    0x00ff0000L,    // 蒙色
    0x00000000L};   // 黑色

// 库存画刷类型
const int stBrush[6] = {
    WHITE_BRUSH,    // 白
    LTGRAY_BRUSH,   // 浅灰
    GRAY_BRUSH,     // 灰
    DKGRAY_BRUSH,   // 深灰
    BLACK_BRUSH,    // 黑
    NULL_BRUSH      // 中空
};

// 阴影画刷类型
const int htBrush[6] = {
    HS_HORIZONTAL,  // 水平线
    HS_VERTICAL,    // 垂直线
    HS_FDIAGONAL,   // 反对角线
    HS_BDIAGONAL,   // 正对角线
    HS_CROSS,       // 十字交叉线
    HS_DIAGCROSS    // 斜交叉线
};

// 函数说明

// 过程
LRESULT APIENTRY MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY CenterProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SizeProc(HWND, UINT, WPARAM, LPARAM);

// 函数
BOOL InitApplication (HINSTANCE);
BOOL InitInstance (HINSTANCE, int);
