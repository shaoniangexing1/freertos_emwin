#include "GUI.h"
#include "stdlib.h"

#define USE_EXRAM 0              /* 根据开发板选择，0为不使用外部 SRAM  */
#define GUI_NUMBYTES (20 * 1024) /* 根据开发板选择，设置 EMWIN 内存大小 */

void GUI_X_Config(void)
{

  // #if (USE_EXRAM)
  //     /* 从外部 SRAM 中分配 GUI_NUMBYTES 字节的内存 */
  //     U32 *aMemory = mymalloc(SRAMEX, GUI_NUMBYTES);
  // #else
  //     //
  //     // 32 bit aligned memory area
  //     //
  //     static U32 aMemory[GUI_NUMBYTES / 4];
  // #endif

  //     //
  //     // Assign memory to emWin
  //     //
  //     GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  //     //
  //     // Set default font
  //     //
  //     GUI_SetDefaultFont(GUI_FONT_6X8);

   // 1. 分配内存给 emWin
    static U32 aMemory[GUI_NUMBYTES / 4];   // 静态数组，32位对齐
    //printf("GUI_X_Config start\n");
		GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
		//printf("Memory assigned\n");
    // 2. 创建显示驱动并链接
    //    使用模板驱动 GUIDRV_Template_API，颜色转换使用 GUICC_1（单色）
    GUI_DEVICE_CreateAndLink(&GUIDRV_Template_API, GUICC_1, 0, 0);
		//printf("Device linked\n");
    // 3. 设置显示尺寸（必须在创建驱动后调用）
    LCD_SetSizeEx(0, 128, 64);      // 逻辑尺寸
    LCD_SetVSizeEx(0, 128, 64);     // 虚拟尺寸（与实际相同）
}

