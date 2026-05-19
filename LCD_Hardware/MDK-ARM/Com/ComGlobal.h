#ifndef __COMGLOBAL_H__
#define __COMGLOBAL_H__



#include "WM.h"
#include "FRAMEWIN.h"
#include "TEXT.h"
#include "EDIT.h"
#include "stdio.h"
#include "DIALOG.h"

#define COM_LCD_BSP_ST7789  0 //240*240电容屏
#define COM_LCD_BSP_ST7785  1 //128*64单色屏

#define LCD_SELECT  COM_LCD_BSP_ST7789




WM_HWIN EditModule_Create(void);

void set_fatherwin(void);

#endif /* __COMGLOBAL_H__ */
