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

  // (void)LayerIndex; // 单图层，忽略

  // switch (Cmd)
  // {
  // // 指令1：初始化（已在LCD_X_Config完成，此处空实现）
  // case LCD_X_INITCONTROLLER:
  //   return 0;

  // // 指令2：更新缓冲区到硬件LCD（核心：将emWin显存刷到物理屏）
  // case LCD_X_COPYBUFFER:
  // {
  //   LCD_X_COPYBUFFER_INFO *pInfo = (LCD_X_COPYBUFFER_INFO *)pData;
  //   U8 *pBuffer = (U8 *)pInfo->pVRAM;
  //   U16 x0 = pInfo->x0;
  //   U16 y0 = pInfo->y0;
  //   U16 x1 = pInfo->x1;
  //   U16 y1 = pInfo->y1;

  //   // 按你的LCD页/列寻址方式，逐页更新显存到硬件
  //   for (U8 page = 0; page < 8; page++)
  //   { // 8页（64行=8页×8行）
  //     U16 y_start = page * 8;
  //     U16 y_end = y_start + 7;
  //     // 只更新需要刷新的区域
  //     if (y_end < y0 || y_start > y1)
  //       continue;

  //     // 调用现有接口更新该页的列数据
  //     // display_graphic_128x64(page+1, 1, &pBuffer[page*LCD_WIDTH]);
  //     // 或更精细的区域更新（复用你现有page/column操作逻辑）
  //     U8 page_addr = 0x40 + page;
  //     U8 col_L = 0xe0 + (x0 & 0x0f);
  //     U8 col_H = 0xf0 + ((x0 >> 4) & 0x0f);

  //     start_flag();
  //     transfer(0x7e);      // 从机地址
  //     transfer(0x00);      // 指令前缀
  //     transfer(0x38);      // 配置位
  //     transfer(page_addr); // 页地址
  //     transfer(col_L);     // 列地址低4位
  //     transfer(col_H);     // 列地址高4位
  //     stop_flag();

  //     start_flag();
  //     transfer(0x7e);
  //     transfer(0x40); // 数据前缀
  //     for (U16 x = x0; x <= x1; x++)
  //     {
  //       transfer(pBuffer[page * LCD_WIDTH + x]); // 写入该像素数据
  //     }
  //     stop_flag();
  //   }
  //   return 0;
  // }

  // // 指令3：设置单个像素（可选，emWin优先用缓冲区批量更新）
  // case LCD_X_SETPIXEL:
  // {
  //   LCD_X_SETPIXEL_INFO *pInfo = (LCD_X_SETPIXEL_INFO *)pData;
  //   U16 x = pInfo->x;
  //   U16 y = pInfo->y;
  //   U32 Color = pInfo->Color; // 单色屏：0=黑，1=白

  //   // 1. 更新显存缓冲区
  //   U8 Page = y / 8;
  //   U8 Bit = y % 8;
  //   if (Color)
  //   {
  //     _aLCDBuffer[Page * LCD_WIDTH + x] |= (1 << Bit);
  //   }
  //   else
  //   {
  //     _aLCDBuffer[Page * LCD_WIDTH + x] &= ~(1 << Bit);
  //   }

  //   // 2. （可选）立即刷新该像素到硬件（推荐批量刷新，此处仅示例）
  //   // 复用display_graphic_8x16或直接操作页/列
  //   return 0;
  // }

  // // 其他指令（如读像素、填充区域）按需实现
  // default:
  //   return -1; // 未实现指令

    //  (void)LayerIndex;   // 单层，忽略

    // switch (Cmd) {
    //     case LCD_X_INITCONTROLLER:
    //         // 控制器初始化已在 LCD_X_Config 中完成，这里无需额外操作
    //         return 0;

    //     case LCD_X_SHOWBUFFER: {
    //         // emWin 通知将显存缓冲区内容刷新到物理屏
    //         LCD_X_SHOWBUFFER * pInfo = (LCD_X_SHOWBUFFER *)pData;
    //         U8 * pBuffer = (U8 *)pInfo->pVRAM;   // 指向显存起始地址（即 _aLCDBuffer）
    //         U16 x0 = pInfo->x0;
    //         U16 y0 = pInfo->y0;
    //         U16 x1 = pInfo->x1;
    //         U16 y1 = pInfo->y1;

    //         // 将缓冲区中指定区域的数据更新到 LCD
    //         // 由于你的 LCD 是 128x64 单色屏，按页（8行一页）组织，需要逐页、逐列发送
    //         for (U8 page = 0; page < 8; page++) {       // 共 8 页（每页 8 行）
    //             U16 page_start_y = page * 8;            // 页起始行
    //             U16 page_end_y   = page_start_y + 7;    // 页结束行
    //             if (page_end_y < y0 || page_start_y > y1) continue;   // 该页不在刷新区域内

    //             // 计算该页在缓冲区中的起始索引
    //             U8 * pPage = pBuffer + page * XSIZE_PHYS;

    //             // 调用你的 LCD 刷新函数，刷新该页从 x0 到 x1 的列（全页刷新也可以）
    //             // 假设你有一个函数可以刷新一页的部分列，这里简化为全页刷新（效率稍低但简单）
    //             // 利用你的 display_graphic_128x64 函数，它需要页号（1~8）和列起始（0~127）
    //             // 注意：你的 display_graphic_128x64 接收页号（1-based）和列号（1-based），并且会固定刷新整个 128 列
    //             // 为了支持部分刷新，可以修改该函数或使用更精细的刷新方式。这里为简单起见，使用全页刷新。
    //             display_graphic_128x64(page+1, 1, pPage);   // page+1 转换为 1-based
    //         }
    //         return 0;
    //     }

    //     case LCD_X_SHOWBUFFER: {
    //         // 可选：emWin 可能直接调用设置单个像素，但通常通过 COPYBUFFER 批量刷新
    //         // 为了完整性，实现单个像素的更新（同时更新缓冲区和硬件）
    //         LCD_X_SETPIXEL_INFO * pInfo = (LCD_X_SETPIXEL_INFO *)pData;
    //         U16 x = pInfo->x;
    //         U16 y = pInfo->y;
    //         U32 Color = pInfo->Color;   // 单色屏：0 表示黑，非 0 表示白

    //         // 更新缓冲区
    //         U8 page = y / 8;
    //         U8 bit  = y % 8;
    //         if (Color) {
    //             _aLCDBuffer[page * XSIZE_PHYS + x] |=  (1 << bit);
    //         } else {
    //             _aLCDBuffer[page * XSIZE_PHYS + x] &= ~(1 << bit);
    //         }

    //         // 立即刷新该像素到 LCD（可选，一般由 COPYBUFFER 统一刷新）
    //         // 但为了实时性，可以调用单个点绘制函数。这里简单调用你的绘图函数（如果有）
    //         // 如果你的 LCD 支持单点操作，可以调用类似 lcd_draw_point(x, y, Color) 的函数。
    //         // 若没有，可以忽略，等待下次 COPYBUFFER。
    //         return 0;
    //     }

    //     default:
    //         return -1;   // 其他命令未实现
    // }


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
