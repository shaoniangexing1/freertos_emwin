#ifndef __COMGLOBAL_H__
#define __COMGLOBAL_H__

#include "WM.h"
#include "FRAMEWIN.h"
#include "TEXT.h"
#include "EDIT.h"
#include "stdio.h"
#include "DIALOG.h"

#define COM_LCD_BSP_ST7789  //240*240µçÈÝÆÁ
#define COM_LCD_BSP_ST7785  //128*64µ¥É«ÆÁ

#define LCD_SELECT  COM_LCD_BSP_ST7789


WM_HWIN EditModule_Create(void);


#endif /* __COMGLOBAL_H__ */
