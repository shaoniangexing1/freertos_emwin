#include "GUI.h"
#include "ComGlobal.h"
#include "stdlib.h"
#include "GUIConf.h"


#define USE_EXRAM 0              /* 根据开发板选择，0为不使用外部 SRAM  */
//#define GUI_NUMBYTES (20 * 1024) /* 根据开发板选择，设置 EMWIN 内存大小 */

void GUI_X_Config(void)
{

  // 1. 分配内存给 emWin
  static U32 aMemory[GUI_NUMBYTES / 4]; // 静态数组，32位对齐
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
#if (LCD_SELECT == COM_LCD_BSP_ST7789) 
{
  // ST7789 彩色屏：16bpp 驱动，颜色转换 GUICC_565
  extern const GUI_DEVICE_API GUIDRV_ST7789_API;
  GUI_DEVICE_CreateAndLink(&GUIDRV_ST7789_API, GUICC_565, 0, 0);
  LCD_SetSizeEx(0, 240, 240);
  LCD_SetVSizeEx(0, 240, 240);
}
#elif (LCD_SELECT == COM_LCD_BSP_ST7785) 
{
  // 2. 创建显示驱动并链接
  //    使用模板驱动 GUIDRV_Template_API，颜色转换使用 GUICC_1（单色）
  GUI_DEVICE_CreateAndLink(&GUIDRV_Template_API, GUICC_1, 0, 0);
  // printf("Device linked\n");
  //  3. 设置显示尺寸（必须在创建驱动后调用）
  LCD_SetSizeEx(0, 128, 64);  // 逻辑尺寸
  LCD_SetVSizeEx(0, 128, 64); // 虚拟尺寸（与实际相同）
}
#endif
}
