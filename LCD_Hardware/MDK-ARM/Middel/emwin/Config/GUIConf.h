#ifndef GUICONF_H 
#define GUICONF_H 
/********************************************************************* 
* 
*       Multi layer/display support 
*/ 
#include "stdio.h"
#define GUI_NUM_LAYERS             1       /* 显示的最大层数 */ 
 
/********************************************************************* 
* 
*       Multi tasking support 
*/ 
#define GUI_OS                     (1)     /* 0:不使用操作系统 */ 
 
/********************************************************************* 
* 
*       Configuration of touch support 
*/ 
#define GUI_SUPPORT_TOUCH          (0)     /* 0:不支持触摸 */ 
 
/********************************************************************* 
* 
*       Default font 
*/ 
#define GUI_DEFAULT_FONT           &GUI_Font6x8   /* 默认字体 */ 
 
/********************************************************************* 
* 
*         Configuration of available packages 
*/ 
#define GUI_SUPPORT_MOUSE      0       /* 支持鼠标 */ 
#define GUI_WINSUPPORT         1       /* 支持窗口管理 */ 
#define GUI_SUPPORT_MEMDEV     1       /* 支持存储设备 */ 
#define GUI_SUPPORT_DEVICES    1       /* 使用设备指针 */ 
 
#endif  // Avoid multiple inclusion 