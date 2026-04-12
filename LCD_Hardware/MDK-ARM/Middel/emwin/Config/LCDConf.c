/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2020  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.16 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "LCD.h"
#include "GUIConf.h"
#include "I2C.h" // 你的硬件驱动头文件
#include "Inf_ST7785_LCD.h" // 你的LCD操作函数头文件
/*********************************************************************
 *
 *       Layer configuration (to be modified)
 *
 **********************************************************************
 */
//
// Physical display size
//   The display size should be adapted in order to match the size of
//   the target display.
//
#define XSIZE_PHYS 128
#define YSIZE_PHYS 64

// 显存缓冲区（单色屏：128x64=1024字节，按页存储：8页×128字节）
static U8 _aLCDBuffer[YSIZE_PHYS / 8 * XSIZE_PHYS];   // 1024 字节
U8 *gui_vram = _aLCDBuffer;   // 导出给驱动使用
//
// Color conversion
//   The color conversion functions should be selected according to
//   the color mode of the target display. Detaileds can be found in
//   the chapter "Colors" in the emWin user manual.
//
#define COLOR_CONVERSION GUICC_8888

//
// Display driver
//   GUIDRV_WIN32 is for use only within the emWin Simulation
//   environment. In order to use the target display controller, the
//   according emWin display driver should be configured as it is
//   described in the chapter "Display Drivers" in the emWin user
//   manual. Beyond that sample configuration files can be found in
//   The folder "Sample\LCDConf\%DISPLAY_DRIVER%\".
//
#define DISPLAY_DRIVER GUIDRV_WIN32

/*********************************************************************
 *
 *       Configuration checking
 *
 **********************************************************************
 */
#ifndef XSIZE_PHYS
#error Physical X size of display is not defined!
#endif
#ifndef YSIZE_PHYS
#error Physical Y size of display is not defined!
#endif
#ifndef COLOR_CONVERSION
#error Color conversion not defined!
#endif
#ifndef DISPLAY_DRIVER
#error No display driver defined!
#endif

/*********************************************************************
 *
 *       Public code
 *
 **********************************************************************
 */
/*********************************************************************
 *
 *       LCD_X_Config
 *
 * Function description
 *   Called during the initialization process in order to set up the
 *   display driver configuration.
 */
void LCD_X_Config(void)
{
  // 1. 初始化硬件LCD（复用你现有接口）
  LCD_Start();    // 调用现有LCD初始化函数
  clear_screen(); // 清屏

  //
  // 2. Set display driver and color conversion for 1st layer
  //
 // GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  // 3. 设置显存地址（emWin将绘制数据写入该缓冲区，后续同步到硬件）
  LCD_SetVRAMAddrEx(0, (void *)_aLCDBuffer);
  // 4. 设置显示尺寸（与 GUIConf.c 中的一致）
  LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);

  // 5 . 设置显示方向（匹配你现有LCD的扫描方向）
  //LCD_SetMirrorX(0, GUI_MIRROR_X | GUI_MIRROR_Y); // 按需调整，匹配硬件

  //
  // Display driver configuration
  //
  // if (LCD_GetSwapXY()) {
  //   LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
  //   LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  // } else {
  //   LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
  //   LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  // }
}

/*********************************************************************
 *
 *       LCD_X_DisplayDriver
 *
 * Function description
 *   Display driver callback function. This function is called by the
 *   Display driver for certain purposes. Using GUIDRV_Win32 it is not
 *   required to react to any command.
 * emWin 显示驱动回调，处理初始化、刷新缓冲区等命令
 */
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *pData)
{

		printf("LCD_X_DisplayDriver: cmd=%d\n", Cmd);
     (void)LayerIndex;
    switch (Cmd) {
        case LCD_X_INITCONTROLLER:
            // 硬件初始化已在LCD_Start中完成
            return 0;

        case LCD_X_SHOWBUFFER: {
           // LCD_X_SHOWBUFFER_INFO * pInfo = (LCD_X_SHOWBUFFER_INFO *)pData;
					printf("LCD_X_SHOWBUFFER called\n");
            U8 * pBuffer = gui_vram;   // 指向显存

            for (U8 page = 0; page < 8; page++) {
        // 构造页地址、列地址（此处为全列刷新）
        uchar page_addr = 0x40 + page;
        uchar col_low = 0xe0;   // 列起始低4位（列0）
        uchar col_high = 0xf0;  // 列起始高4位（列0）

        start_flag();
        transfer(0x7e);
        transfer(0x00);
        transfer(0x38);
        transfer(page_addr);
        transfer(col_low);
        transfer(col_high);
        stop_flag();

        start_flag();
        transfer(0x7e);
        transfer(0x40);   // 数据模式
        for (U16 col = 0; col < 128; col++) {
            transfer(pBuffer[page * 128 + col]);
        }
        stop_flag();
    }
    return 0;
        }

        // 其他命令可忽略或返回-1
        default:
            return -1;
    }
  }
  

  /*************************** End of file ****************************/
