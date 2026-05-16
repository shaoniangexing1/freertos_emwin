#include "ComGlobal.h"

//=============================
// 控件ID
//=============================
#define ID_WIN_BASE (GUI_ID_USER + 0x00)   // 黑色背景父窗口
#define ID_TEXT_TITLE (GUI_ID_USER + 0x01) // 标题
#define ID_EDIT_INPUT (GUI_ID_USER + 0x02) // 编辑框

#define WIDTH 128
#define HIGHT 64
#define WIDTH_240 240
#define HIGHT_240 240

#if defined(LCD_SELECT) && (LCD_SELECT == COM_LCD_BSP_ST7789)
#define WIN_WIDTH WIDTH_240
#define WIN_HIGHT HIGHT_240
#elif defined(LCD_SELECT) && (LCD_SELECT == COM_LCD_BSP_ST7785)
#define WIN_WIDTH WIDTH_128
#define WIN_HIGHT HIGHT_64
#endif
//=============================
// 控件创建表
//=============================
#if defined(LCD_SELECT) && (LCD_SELECT == COM_LCD_BSP_ST7785)
static const GUI_WIDGET_CREATE_INFO _aCreateInfo[] = {

    // ==============================================
    // 1. 先创建【黑色背景父窗口】（无标题栏、纯底色）
    // ==============================================
    {WINDOW_CreateIndirect, "COM", ID_WIN_BASE, 0, 0, WIN_WIDTH, WIN_HIGHT, 0, 0, 0},

    // ==============================================
    // 2. 标题文本
    // ==============================================
    {TEXT_CreateIndirect, "Edit Module", ID_TEXT_TITLE, 5, 0, WIN_WIDTH, 50, 0, 0, 0},

    // ==============================================
    // 3. 编辑框
    // ==============================================
    {EDIT_CreateIndirect, "", ID_EDIT_INPUT, 16, 24, 200, 12, 0, 0, 0},
};
#elif defined(LCD_SELECT) && (LCD_SELECT == COM_LCD_BSP_ST7789)
static const GUI_WIDGET_CREATE_INFO _aCreateInfo[] = {

    // ==============================================
    // 1. 先创建【黑色背景父窗口】（无标题栏、纯底色）
    // ==============================================
    {WINDOW_CreateIndirect, "COM", ID_WIN_BASE, 0, 0, WIN_WIDTH, WIN_HIGHT, 0, 0, 0},

    // ==============================================
    // 2. 标题文本
    // ==============================================
    {TEXT_CreateIndirect, "Edit Module", ID_TEXT_TITLE, 5, 0, WIN_WIDTH, 50, 0, 0, 0},

    // ==============================================
    // 3. 编辑框
    // ==============================================
    {EDIT_CreateIndirect, "", ID_EDIT_INPUT, 16, 100, 200, 12, 0, 0, 0},
};
#endif

//=============================
// 对话框回调
//=============================
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hDlg;
    WM_HWIN hWin;

    switch (pMsg->MsgId)
    {
    // case WM_PAINT:
    //     GUI_SetBkColor(GUI_BLUE);
    //     GUI_Clear();
    //     break;
    case WM_INIT_DIALOG:
        hDlg = pMsg->hWin;

        // --------------------------
        // 父窗口：黑色背景 + 无边框
        // --------------------------
        // hWin = WM_GetDialogItem(hDlg, ID_WIN_BASE);

        // 设置背景色
        // WINDOW_SetBkColor(hWin, GUI_WHITE);

        // --------------------------
        // 标题：白色字体
        // --------------------------
        hWin = WM_GetDialogItem(hDlg, ID_TEXT_TITLE);
        TEXT_SetFont(hWin, &GUI_Font6x8);
        TEXT_SetTextColor(hWin, GUI_BLACK);
        TEXT_SetTextAlign(hWin, GUI_TA_LEFT | GUI_TA_VCENTER);
        TEXT_SetText(hWin, "EDIT_TEXT"); // <-- 设置标题文本

        // --------------------------
        // 编辑框：黑底白字
        // --------------------------
        hWin = WM_GetDialogItem(hDlg, ID_EDIT_INPUT);
        EDIT_SetFont(hWin, &GUI_Font6x8);      // 只能使用这个字号的字体
        EDIT_SetBkColor(hWin, 0, GUI_WHITE);   // 背景黑
        EDIT_SetTextColor(hWin, 0, GUI_BLACK); // 字体白GUI_BLACK
        EDIT_SetTextAlign(hWin, GUI_TA_CENTER | GUI_TA_VCENTER);
        EDIT_SetText(hWin, " "); // <-- 设置编辑框内容
        WM_SetFocus(hWin);
         GUI_Exec();
        break;

    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

//=============================
// 创建界面（对外接口）
//=============================
WM_HWIN EditModule_Create(void)
{
    int xSize = LCD_GetXSize();
    int ySize = LCD_GetYSize();
    // printf("Desktop size: %d x %d\n", xSize, ySize);

    // 手动设置桌面窗口大小为全屏（如果不对）
    // printf("WIN_WIDTH = %d, WIN_HIGHT = %d\n", WIN_WIDTH, WIN_HIGHT);
    return GUI_CreateDialogBox(_aCreateInfo, GUI_COUNTOF(_aCreateInfo), _cbDialog, WM_HBKWIN, 0, 0);
}

static void _cbfatherDialog(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        // --------------------------
        // 父窗口：黑色背景 + 无边框
        // --------------------------
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}
void set_fatherwin(void)
{
    WM_SetCallback(WM_HBKWIN, _cbfatherDialog);
}