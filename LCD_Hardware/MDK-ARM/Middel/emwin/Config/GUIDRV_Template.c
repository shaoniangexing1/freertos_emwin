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
*
* 文件        : GUIDRV_Template.c
* 目的        : 单色屏显示驱动模板，适用于 128x64 单色屏（I2C 接口）
* 说明        : 本驱动使用显存缓冲区，通过 LCD_X_DisplayDriver 中的 SHOWBUFFER
*               命令将缓冲区内容刷新到物理屏。底层绘图函数直接操作显存。
*               所有 I2C 操作在 LCD.c 中实现，由 start_flag/transfer/stop_flag 完成。
*               驱动 API 结构体 GUIDRV_Template_API 必须注册到 emWin。
* 作者        : 根据用户硬件适配
*---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include "LCD_Private.h"
#include "GUI_Private.h"
#include "LCD_SIM.h"
#include "LCD_ConfDefaults.h"
#include "stdio.h"

// 声明外部显存指针（在 LCDConf.c 中定义）
extern U8 *gui_vram;

/*********************************************************************
*       Macros for MIRROR_, SWAP_ and LUT_（保留模板原样，不修改）
**********************************************************************
*/
#if (!defined (LCD_LUT_COM) && !defined(LCD_LUT_SEG))
  #if   (!LCD_MIRROR_X && !LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) y
  #elif (!LCD_MIRROR_X && !LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) y
    #define LOG2PHYS_Y(x, y) x
  #elif (!LCD_MIRROR_X &&  LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) LCD_YSIZE - 1 - (y)
  #elif (!LCD_MIRROR_X &&  LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) y
    #define LOG2PHYS_Y(x, y) LCD_XSIZE - 1 - (x)
  #elif ( LCD_MIRROR_X && !LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_XSIZE - 1 - (x)
    #define LOG2PHYS_Y(x, y) y
  #elif ( LCD_MIRROR_X && !LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_YSIZE - 1 - (y)
    #define LOG2PHYS_Y(x, y) x
  #elif ( LCD_MIRROR_X &&  LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_XSIZE - 1 - (x)
    #define LOG2PHYS_Y(x, y) LCD_YSIZE - 1 - (y)
  #elif ( LCD_MIRROR_X &&  LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_YSIZE - 1 - (y)
    #define LOG2PHYS_Y(x, y) LCD_XSIZE - 1 - (x)
  #endif
#else
  #if   ( defined (LCD_LUT_COM) && !defined(LCD_LUT_SEG))
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) LCD__aLine2Com0[y]
  #elif (!defined (LCD_LUT_COM) &&  defined(LCD_LUT_SEG))
    #define LOG2PHYS_X(x, y) LCD__aCol2Seg0[x]
    #define LOG2PHYS_Y(x, y) y
  #elif ( defined (LCD_LUT_COM) &&  defined(LCD_LUT_SEG))
    #define LOG2PHYS_X(x, y) LCD__aCol2Seg0[x]
    #define LOG2PHYS_Y(x, y) LCD__aLine2Com0[y]
  #endif
#endif

#define DRIVER_CONTEXT DRIVER_CONTEXT_TEMPLATE

/*********************************************************************
*       驱动上下文结构体（存储显存地址、尺寸等信息）
**********************************************************************
*/
typedef struct {
  PTR_ADDR VRAMAddr;
  int xSize, ySize;
  int vxSize, vySize;
  int vxSizePhys;
  int BitsPerPixel;
} DRIVER_CONTEXT;

/*********************************************************************
*       核心绘图函数（直接操作显存缓冲区 gui_vram）
**********************************************************************
*/

/**
 * @brief 设置指定像素的颜色索引（0=黑，1=白）
 * @param pDevice 设备指针
 * @param x,y 坐标
 * @param PixelIndex 颜色索引（0 或 1）
 */
static void _SetPixelIndex(GUI_DEVICE * pDevice, int x, int y, LCD_PIXELINDEX PixelIndex) {
    // 计算页（8行一页）和页内位偏移
    U8 page = y / 8;
    U8 bit  = y % 8;
    // 更新显存缓冲区
    if (PixelIndex) {
       // printf("Set pixel at (%d, %d) to 1\n", x, y); // 调试输出
        gui_vram[page * 128 + x] |= (1 << bit);
    } else {
        gui_vram[page * 128 + x] &= ~(1 << bit);
    }
    (void)pDevice; // 避免未使用警告
}

/**
 * @brief 获取指定像素的颜色索引
 * @param pDevice 设备指针
 * @param x,y 坐标
 * @return 颜色索引（0 或 1）
 */
static LCD_PIXELINDEX _GetPixelIndex(GUI_DEVICE * pDevice, int x, int y) {
    U8 page = y / 8;
    U8 bit  = y % 8;
    (void)pDevice;
    return (gui_vram[page * 128 + x] >> bit) & 1;
}

/**
 * @brief 异或像素（用于 XOR 绘图模式）
 */
static void _XorPixel(GUI_DEVICE * pDevice, int x, int y) {
    LCD_PIXELINDEX PixelIndex = _GetPixelIndex(pDevice, x, y);
    LCD_PIXELINDEX IndexMask  = pDevice->pColorConvAPI->pfGetIndexMask();
    _SetPixelIndex(pDevice, x, y, PixelIndex ^ IndexMask);
}

/**
 * @brief 填充矩形区域
 * @param pDevice 设备指针
 * @param x0,y0 左上角
 * @param x1,y1 右下角
 */
static void _FillRect(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
    // 获取当前绘图颜色（由上层通过 LCD_SetColorIndex 等设置）
    U8 color = LCD__GetColorIndex();
    // 处理 XOR 模式（简单起见，使用 _XorPixel 逐点处理）
    if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
        for (int y = y0; y <= y1; y++) {
            for (int x = x0; x <= x1; x++) {
                _XorPixel(pDevice, x, y);
            }
        }
    } else {
        // 普通模式：按页和位直接修改显存
        for (int y = y0; y <= y1; y++) {
            U8 page = y / 8;
            U8 bit  = y % 8;
            for (int x = x0; x <= x1; x++) {
                if (color) {
                    gui_vram[page * 128 + x] |= (1 << bit);
                } else {
                    gui_vram[page * 128 + x] &= ~(1 << bit);
                }
            }
        }
    }
    (void)pDevice;
}

/**
 * @brief 绘制水平线（直接调用 _FillRect）
 */
static void _DrawHLine(GUI_DEVICE * pDevice, int x0, int y, int x1) {
    _FillRect(pDevice, x0, y, x1, y);
}

/**
 * @brief 绘制垂直线（直接调用 _FillRect）
 */
static void _DrawVLine(GUI_DEVICE * pDevice, int x, int y0, int y1) {
    _FillRect(pDevice, x, y0, x, y1);
}

/*********************************************************************
*       位图绘制函数（模板提供，直接使用）
*       说明：仅 1bpp 位图会被实际使用，其他位图深度保留但不修改。
**********************************************************************
*/

static void _DrawBitLine1BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
    LCD_PIXELINDEX IndexMask, Index0, Index1, Pixel;
    Index0 = *(pTrans + 0);
    Index1 = *(pTrans + 1);
    x += Diff;
    switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
        do {
            _SetPixelIndex(pDevice, x++, y, (*p & (0x80 >> Diff)) ? Index1 : Index0);
            if (++Diff == 8) { Diff = 0; p++; }
        } while (--xsize);
        break;
    case LCD_DRAWMODE_TRANS:
        do {
            if (*p & (0x80 >> Diff))
                _SetPixelIndex(pDevice, x, y, Index1);
            x++;
            if (++Diff == 8) { Diff = 0; p++; }
        } while (--xsize);
        break;
    case LCD_DRAWMODE_XOR | LCD_DRAWMODE_TRANS:
    case LCD_DRAWMODE_XOR:
        IndexMask = pDevice->pColorConvAPI->pfGetIndexMask();
        do {
            if (*p & (0x80 >> Diff)) {
                Pixel = _GetPixelIndex(pDevice, x, y);
                _SetPixelIndex(pDevice, x, y, Pixel ^ IndexMask);
            }
            x++;
            if (++Diff == 8) { Diff = 0; p++; }
        } while (--xsize);
        break;
    }
}

static void _DrawBitLine2BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
    LCD_PIXELINDEX Pixels, PixelIndex;
    int CurrentPixel, Shift, Index;
    Pixels = *p;
    CurrentPixel = Diff;
    x += Diff;
    switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
        if (pTrans) {
            do {
                Shift = (3 - CurrentPixel) << 1;
                Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
                PixelIndex = *(pTrans + Index);
                _SetPixelIndex(pDevice, x++, y, PixelIndex);
                if (++CurrentPixel == 4) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        } else {
            do {
                Shift = (3 - CurrentPixel) << 1;
                Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
                _SetPixelIndex(pDevice, x++, y, Index);
                if (++CurrentPixel == 4) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        }
        break;
    case LCD_DRAWMODE_TRANS:
        if (pTrans) {
            do {
                Shift = (3 - CurrentPixel) << 1;
                Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
                if (Index) {
                    PixelIndex = *(pTrans + Index);
                    _SetPixelIndex(pDevice, x, y, PixelIndex);
                }
                x++;
                if (++CurrentPixel == 4) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        } else {
            do {
                Shift = (3 - CurrentPixel) << 1;
                Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
                if (Index) {
                    _SetPixelIndex(pDevice, x, y, Index);
                }
                x++;
                if (++CurrentPixel == 4) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        }
        break;
    }
}

static void _DrawBitLine4BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
    LCD_PIXELINDEX Pixels, PixelIndex;
    int CurrentPixel, Shift, Index;
    Pixels = *p;
    CurrentPixel = Diff;
    x += Diff;
    switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
        if (pTrans) {
            do {
                Shift = (1 - CurrentPixel) << 2;
                Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
                PixelIndex = *(pTrans + Index);
                _SetPixelIndex(pDevice, x++, y, PixelIndex);
                if (++CurrentPixel == 2) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        } else {
            do {
                Shift = (1 - CurrentPixel) << 2;
                Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
                _SetPixelIndex(pDevice, x++, y, Index);
                if (++CurrentPixel == 2) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        }
        break;
    case LCD_DRAWMODE_TRANS:
        if (pTrans) {
            do {
                Shift = (1 - CurrentPixel) << 2;
                Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
                if (Index) {
                    PixelIndex = *(pTrans + Index);
                    _SetPixelIndex(pDevice, x, y, PixelIndex);
                }
                x++;
                if (++CurrentPixel == 2) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        } else {
            do {
                Shift = (1 - CurrentPixel) << 2;
                Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
                if (Index) {
                    _SetPixelIndex(pDevice, x, y, Index);
                }
                x++;
                if (++CurrentPixel == 2) { CurrentPixel = 0; Pixels = *(++p); }
            } while (--xsize);
        }
        break;
    }
}

static void _DrawBitLine8BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int xsize, const LCD_PIXELINDEX * pTrans) {
    LCD_PIXELINDEX Pixel;
    switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
        if (pTrans) {
            for (; xsize > 0; xsize--, x++, p++) {
                Pixel = *p;
                _SetPixelIndex(pDevice, x, y, *(pTrans + Pixel));
            }
        } else {
            for (; xsize > 0; xsize--, x++, p++) {
                _SetPixelIndex(pDevice, x, y, *p);
            }
        }
        break;
    case LCD_DRAWMODE_TRANS:
        if (pTrans) {
            for (; xsize > 0; xsize--, x++, p++) {
                Pixel = *p;
                if (Pixel) _SetPixelIndex(pDevice, x, y, *(pTrans + Pixel));
            }
        } else {
            for (; xsize > 0; xsize--, x++, p++) {
                Pixel = *p;
                if (Pixel) _SetPixelIndex(pDevice, x, y, Pixel);
            }
        }
        break;
    }
}

// 16bpp 和 32bpp 位图绘制函数（保留原模板，但实际不会被单色屏使用）
static void _DrawBitLine16BPP(GUI_DEVICE * pDevice, int x, int y, U16 const * p, int xsize) {
    for (; xsize > 0; xsize--, x++, p++) {
        _SetPixelIndex(pDevice, x, y, *p);
    }
}

static void _DrawBitLine32BPP(GUI_DEVICE * pDevice, int x, int y, U32 const * p, int xsize) {
    for (; xsize > 0; xsize--, x++, p++) {
        _SetPixelIndex(pDevice, x, y, *p);
    }
}

/**
 * @brief 绘制位图的总入口
 */
static void _DrawBitmap(GUI_DEVICE * pDevice, int x0, int y0,
                        int xSize, int ySize,
                        int BitsPerPixel,
                        int BytesPerLine,
                        const U8 * pData, int Diff,
                        const LCD_PIXELINDEX * pTrans) {
    int i;
    switch (BitsPerPixel) {
    case 1:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine1BPP(pDevice, x0, i + y0, pData, Diff, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 2:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine2BPP(pDevice, x0, i + y0, pData, Diff, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 4:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine4BPP(pDevice, x0, i + y0, pData, Diff, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 8:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine8BPP(pDevice, x0, i + y0, pData, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 16:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine16BPP(pDevice, x0, i + y0, (const U16 *)pData, xSize);
            pData += BytesPerLine;
        }
        break;
    case 32:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine32BPP(pDevice, x0, i + y0, (const U32 *)pData, xSize);
            pData += BytesPerLine;
        }
        break;
    }
}

/*********************************************************************
*       驱动管理函数（模板必需）
**********************************************************************
*/

static int _InitOnce(GUI_DEVICE * pDevice) {
    DRIVER_CONTEXT * pContext;
    if (pDevice->u.pContext == NULL) {
        pDevice->u.pContext = GUI_ALLOC_GetFixedBlock(sizeof(DRIVER_CONTEXT));
        pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
        pContext->BitsPerPixel = LCD__GetBPP(pDevice->pColorConvAPI->pfGetIndexMask());
    }
    return pDevice->u.pContext ? 0 : 1;
}

static I32 _GetDevProp(GUI_DEVICE * pDevice, int Index) {
    I32 ret = 0;
    DRIVER_CONTEXT * pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    switch (Index) {
    case LCD_DEVCAP_XSIZE:        ret=pContext->xSize; break;
    case LCD_DEVCAP_YSIZE:        ret=pContext->ySize; break;
    case LCD_DEVCAP_VXSIZE:       ret=pContext->vxSize; break;
    case LCD_DEVCAP_VYSIZE:       ret=pContext->vySize; break;
    case LCD_DEVCAP_BITSPERPIXEL: ret=pContext->BitsPerPixel; break;
    case LCD_DEVCAP_NUMCOLORS:    ret=0; break;
    case LCD_DEVCAP_XMAG:         ret=1; break;
    case LCD_DEVCAP_YMAG:         ret=1; break;
    case LCD_DEVCAP_MIRROR_X:     ret=0; break;
    case LCD_DEVCAP_MIRROR_Y:     ret=0; break;
    case LCD_DEVCAP_SWAP_XY:      ret=0; break;
    }
    printf("_GetDevProp: index=%d, ret=%ld\n", Index, ret);
    return ret;
}

static void * _GetDevData(GUI_DEVICE * pDevice, int Index) {
    (void)pDevice;
#if GUI_SUPPORT_MEMDEV
    switch (Index) {
    case LCD_DEVDATA_MEMDEV:
        return (void *)&GUI_MEMDEV_DEVICE_1; // 单色屏使用 1bpp 内存设备
    }
#else
    (void)Index;
#endif
    return NULL;
}

static void _GetRect(GUI_DEVICE * pDevice, LCD_RECT * pRect) {
    DRIVER_CONTEXT * pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = pContext->vxSize - 1;
    pRect->y1 = pContext->vySize - 1;
}

static void _SetOrg(GUI_DEVICE * pDevice, int x, int y) {
    LCD_X_SETORG_INFO Data = {0};
    Data.xPos = x;
    Data.yPos = y;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETORG, (void *)&Data);
}

static void _SetVRAMAddr(GUI_DEVICE * pDevice, void * pVRAM) {
    DRIVER_CONTEXT * pContext;
    LCD_X_SETVRAMADDR_INFO Data = {0};
    _InitOnce(pDevice);
    if (pDevice->u.pContext) {
        pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
        pContext->VRAMAddr = (PTR_ADDR)pVRAM;
        Data.pVRAM = pVRAM;
        LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETVRAMADDR, (void *)&Data);
    }
}

static void _SetVSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
    DRIVER_CONTEXT * pContext;
    _InitOnce(pDevice);
    if (pDevice->u.pContext) {
        pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
        pContext->vxSize = xSize;
        pContext->vySize = ySize;
        pContext->vxSizePhys = xSize;
    }
}

static void _SetSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
    DRIVER_CONTEXT * pContext;
    LCD_X_SETSIZE_INFO Data = {0};
    _InitOnce(pDevice);
    if (pDevice->u.pContext) {
        pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
        pContext->vxSizePhys = (pContext->vxSizePhys == 0) ? xSize : pContext->vxSizePhys;
        pContext->xSize = xSize;
        pContext->ySize = ySize;
        Data.xSize = xSize;
        Data.ySize = ySize;
        LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETSIZE, (void *)&Data);
    }
}

static int _Init(GUI_DEVICE * pDevice) {
    int r = _InitOnce(pDevice);
    r |= LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_INITCONTROLLER, NULL);
    return r;
}

static void _On(GUI_DEVICE * pDevice) {
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_ON, NULL);
}

static void _Off(GUI_DEVICE * pDevice) {
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_OFF, NULL);
}

static void _SetLUTEntry(GUI_DEVICE * pDevice, U8 Pos, LCD_COLOR Color) {
    LCD_X_SETLUTENTRY_INFO Data = {0};
    Data.Pos   = Pos;
    Data.Color = Color;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETLUTENTRY, (void *)&Data);
}

static void (* _GetDevFunc(GUI_DEVICE ** ppDevice, int Index))(void) {
    (void)ppDevice;
    switch (Index) {
    case LCD_DEVFUNC_SET_VRAM_ADDR: return (void (*)(void))_SetVRAMAddr;
    case LCD_DEVFUNC_SET_VSIZE:     return (void (*)(void))_SetVSize;
    case LCD_DEVFUNC_SET_SIZE:      return (void (*)(void))_SetSize;
    case LCD_DEVFUNC_INIT:          return (void (*)(void))_Init;
    case LCD_DEVFUNC_ON:            return (void (*)(void))_On;
    case LCD_DEVFUNC_OFF:           return (void (*)(void))_Off;
    case LCD_DEVFUNC_SETLUTENTRY:   return (void (*)(void))_SetLUTEntry;
    }
    return NULL;
}

/*********************************************************************
*       驱动 API 结构体（必须导出，供 emWin 链接）
**********************************************************************
*/
const GUI_DEVICE_API GUIDRV_Template_API = {
    DEVICE_CLASS_DRIVER,
    _DrawBitmap,
    _DrawHLine,
    _DrawVLine,
    _FillRect,
    _GetPixelIndex,
    _SetPixelIndex,
    _XorPixel,
    _SetOrg,
    _GetDevFunc,
    _GetDevProp,
    _GetDevData,
    _GetRect,
};

/*************************** End of file ****************************/