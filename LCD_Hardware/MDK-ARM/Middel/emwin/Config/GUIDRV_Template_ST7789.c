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
* 文件        : GUIDRV_ST7789.c
* 目的        : ST7789 240x240 RGB565 彩色屏驱动，用于 emWin
* 说明        : 本驱动使用显存缓冲区（U16 数组），所有绘图函数直接操作该缓冲区。
*               硬件刷新由 LCD_X_DisplayDriver 中的 SHOWBUFFER 命令完成。
*               底层 SPI 写操作在 Inf_ST7789_LCD.c 中实现。
*---------------------------END-OF-HEADER------------------------------
*/
#include "GUI.h"
#include <stddef.h>

#include "GUI_Private.h"
#include "LCD_Private.h"
#include "LCD_SIM.h"
#include "LCD_ConfDefaults.h"

// 声明外部显存指针（在 LCDConf.c 中定义，类型为 U16*）
extern U16 *gui_vram_st7789;

#define DRIVER_CONTEXT DRIVER_CONTEXT_ST7789

/*********************************************************************
*       驱动上下文结构体
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
*       宏定义：坐标映射（可根据需要修改镜像/交换）
**********************************************************************
*/
#ifndef LOG2PHYS_X
  #define LOG2PHYS_X(x, y) x
#endif
#ifndef LOG2PHYS_Y
  #define LOG2PHYS_Y(x, y) y
#endif

/*********************************************************************
*       核心绘图函数（直接操作显存 gui_vram_st7789）
**********************************************************************
*/

// 设置像素颜色
static void _SetPixelIndex(GUI_DEVICE * pDevice, int x, int y, LCD_PIXELINDEX PixelIndex) {
    (void)pDevice;
    x = LOG2PHYS_X(x, y);
    y = LOG2PHYS_Y(x, y);
    gui_vram_st7789[y * 240 + x] = PixelIndex;
}

// 获取像素颜色
static LCD_PIXELINDEX _GetPixelIndex(GUI_DEVICE * pDevice, int x, int y) {
    (void)pDevice;
    x = LOG2PHYS_X(x, y);
    y = LOG2PHYS_Y(x, y);
    return gui_vram_st7789[y * 240 + x];
}

// XOR 像素（用于 XOR 绘图模式）
static void _XorPixel(GUI_DEVICE * pDevice, int x, int y) {
    LCD_PIXELINDEX PixelIndex = _GetPixelIndex(pDevice, x, y);
    LCD_PIXELINDEX IndexMask  = pDevice->pColorConvAPI->pfGetIndexMask();
    _SetPixelIndex(pDevice, x, y, PixelIndex ^ IndexMask);
}

// 填充矩形
static void _FillRect(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
    U16 color = LCD__GetColorIndex();
    int x, y;
    (void)pDevice;
    //printf("_FillRect: (%d,%d)-(%d,%d)\n", x0, y0, x1, y1);
    // 坐标映射
    x0 = LOG2PHYS_X(x0, y0);
    x1 = LOG2PHYS_X(x1, y1);
    y0 = LOG2PHYS_Y(x0, y0);
    y1 = LOG2PHYS_Y(x1, y1);
    // x0=x0;
    // y0=y0;
    // x1=x1-x0+1;
    // y1=y1-y0+1;

    if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
        for (y = y0; y <= y1; y++) {
            for (x = x0; x <= x1; x++) {
                gui_vram_st7789[y * 240 + x] ^= color;
            }
        }
    } else {
        for (y = y0; y <= y1; y++) {
            U16 *pLine = &gui_vram_st7789[y * 240 + x0];
            for (x = x0; x <= x1; x++) {
                *pLine++ = color;
            }
        }
    }
}

// 绘制水平线
static void _DrawHLine(GUI_DEVICE * pDevice, int x0, int y, int x1) {
    _FillRect(pDevice, x0, y, x1, y);
}

// 绘制垂直线
static void _DrawVLine(GUI_DEVICE * pDevice, int x, int y0, int y1) {
    _FillRect(pDevice, x, y0, x, y1);
}

/*********************************************************************
*       位图绘制函数（支持 1/2/4/8/16/32 bpp）
**********************************************************************
*/

// 1bpp 位图线
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

// 2bpp 位图线
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

// 4bpp 位图线
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

// 8bpp 位图线
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

// 16bpp 位图线
static void _DrawBitLine16BPP(GUI_DEVICE * pDevice, int x, int y, U16 const * p, int xsize) {
    // switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    // case 0:
    //     for (; xsize > 0; xsize--, x++, p++) {
    //         _SetPixelIndex(pDevice, x, y, *p);
    //     }
    //     break;
    // case LCD_DRAWMODE_TRANS:
    //     for (; xsize > 0; xsize--, x++, p++) {
    //         if (*p) _SetPixelIndex(pDevice, x, y, *p);
    //     }
    //     break;
    // case LCD_DRAWMODE_XOR:
    //     for (; xsize > 0; xsize--, x++, p++) {
    //         LCD_PIXELINDEX Pixel = _GetPixelIndex(pDevice, x, y);
    //         _SetPixelIndex(pDevice, x, y, Pixel ^ *p);
    //     }
    //     break;
    // }

    for (; xsize > 0; xsize--, x++, p++) {
            //LCD_PIXELINDEX Pixel = _GetPixelIndex(pDevice, x, y);
            _SetPixelIndex(pDevice, x, y, *p);
        }
}

// 32bpp 位图线
static void _DrawBitLine32BPP(GUI_DEVICE * pDevice, int x, int y, U32 const * p, int xsize) {
    // switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    // case 0:
    //     for (; xsize > 0; xsize--, x++, p++) {
    //         _SetPixelIndex(pDevice, x, y, *p);
    //     }
    //     break;
    // case LCD_DRAWMODE_TRANS:
    //     for (; xsize > 0; xsize--, x++, p++) {
    //         if (*p) _SetPixelIndex(pDevice, x, y, *p);
    //     }
    //     break;
    // case LCD_DRAWMODE_XOR:
    //     for (; xsize > 0; xsize--, x++, p++) {
    //         LCD_PIXELINDEX Pixel = _GetPixelIndex(pDevice, x, y);
    //         _SetPixelIndex(pDevice, x, y, Pixel ^ *p);
    //     }
    //     break;
    // }
        for (; xsize > 0; xsize--, x++, p++) {
            //LCD_PIXELINDEX Pixel = _GetPixelIndex(pDevice, x, y);
            _SetPixelIndex(pDevice, x, y, *p);
        }

}

// 绘制位图主入口
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
            _DrawBitLine1BPP(pDevice, x0, y0 + i, pData, Diff, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 2:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine2BPP(pDevice, x0, y0 + i, pData, Diff, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 4:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine4BPP(pDevice, x0, y0 + i, pData, Diff, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 8:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine8BPP(pDevice, x0, y0 + i, pData, xSize, pTrans);
            pData += BytesPerLine;
        }
        break;
    case 16:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine16BPP(pDevice, x0, y0 + i, (const U16 *)pData, xSize);
            pData += BytesPerLine;
        }
        break;
    case 32:
        for (i = 0; i < ySize; i++) {
            _DrawBitLine32BPP(pDevice, x0, y0 + i, (const U32 *)pData, xSize);
            pData += BytesPerLine;
        }
        break;
    }
    //LCD_Refresh(x0, y0, xSize ,ySize);//ykk260423增加
}

/*********************************************************************
*       设备管理函数
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
    case LCD_DEVCAP_XSIZE:        ret = 240; break;
    case LCD_DEVCAP_YSIZE:        ret = 240; break;
    case LCD_DEVCAP_VXSIZE:       ret = pContext->vxSize; break;
    case LCD_DEVCAP_VYSIZE:       ret = pContext->vySize; break;
    case LCD_DEVCAP_BITSPERPIXEL: ret = pContext->BitsPerPixel; break;
    case LCD_DEVCAP_NUMCOLORS:    ret = 0; break;
    case LCD_DEVCAP_XMAG:         ret = 1; break;
    case LCD_DEVCAP_YMAG:         ret = 1; break;
    case LCD_DEVCAP_MIRROR_X:     ret = 0; break;
    case LCD_DEVCAP_MIRROR_Y:     ret = 0; break;
    case LCD_DEVCAP_SWAP_XY:      ret = 0; break;
    }
    return ret;
}

static void * _GetDevData(GUI_DEVICE * pDevice, int Index) {
    (void)pDevice;
#if GUI_SUPPORT_MEMDEV
    switch (Index) {
    case LCD_DEVDATA_MEMDEV:
        return (void *)&GUI_MEMDEV_DEVICE_16;  // 16bpp 内存设备
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
*       导出驱动 API 结构体
**********************************************************************
*/
const GUI_DEVICE_API GUIDRV_ST7789_API = {
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