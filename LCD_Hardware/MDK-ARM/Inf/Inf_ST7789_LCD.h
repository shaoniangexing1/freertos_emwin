#ifndef __INF_LCD_ST7789_H__
#define __INF_LCD_ST7789_H__

#include "I2C.h"
#define u8 unsigned char
#define u16  unsigned int
#define u32  unsigned long
	 


// //LCD参数
// extern _lcd_dev lcddev;	//管理LCD重要参数
// //LCD的画笔颜色和背景色	   
// extern u16  POINT_COLOR;//默认红色    
// extern u16  BACK_COLOR; //背景颜色.默认为白色

#define ST7789_LCD_W 240
#define ST7789_LCD_H 240

//控制信号线
#define LCD_ST7789_RST_HIGH (HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET))
#define LCD_ST7789_RST_LOW (HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET))
//读写命令线 0:命令 1:数据
#define LCD_ST7789_DCorRS_HIGH (HAL_GPIO_WritePin(ST7789_DC_GPIO_Port, ST7789_DC_Pin, GPIO_PIN_SET))
#define LCD_ST7789_DCorRS_LOW (HAL_GPIO_WritePin(ST7789_DC_GPIO_Port, ST7789_DC_Pin, GPIO_PIN_RESET))
//片选线
#define LCD_ST7789_CS_HIGH (HAL_GPIO_WritePin(ST7789_SPI2_CS_GPIO_Port, ST7789_SPI2_CS_Pin, GPIO_PIN_SET))
#define LCD_ST7789_CS_LOW (HAL_GPIO_WritePin(ST7789_SPI2_CS_GPIO_Port, ST7789_SPI2_CS_Pin, GPIO_PIN_RESET))
//背光控制脉冲
//



//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向





//画笔颜色
#define WHITE         	 0xFFFF		// 白色
#define BLACK         	 0x0000		// 黑色
#define BLUE         	 0x001F		// 蓝色
#define BRED             0XF81F		// 蓝红色（紫红色）
#define GRED 			 0XFFE0		// 绿红色（黄色系/金黄色）
#define GBLUE			 0X07FF		// 绿蓝色（青色/天蓝色）
#define RED           	 0xF800		// 红色
#define MAGENTA       	 0xF81F		// 品红/洋红（紫色）
#define GREEN         	 0x07E0		// 绿色
#define CYAN          	 0x7FFF		// 青色（蓝绿色）
#define YELLOW        	 0xFFE0		// 黄色
#define BROWN 			 0XBC40		// 棕色
#define BRRED 			 0XFC07		// 棕红色
#define GRAY  			 0X8430		// 灰色
//GUI颜色

#define DARKBLUE      	 0X01CF		// 深蓝色
#define LIGHTBLUE      	 0X7D7C		// 浅蓝色
#define GRAYBLUE       	 0X5458		// 灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN     	 0X841F		// 浅绿色
#define LGRAY 			 0XC618		// 浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651		// 浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12		// 浅棕蓝色(选择条目的反色)


//=============================================================================
// ST7789 初始化常用宏定义
//=============================================================================
// 1. 像素格式 COLMOD (3Ah)
#define ST7789_12BIT_RGB444      0x03    // 12bit 色彩
#define ST7789_16BIT_RGB565      0x05    // 16bit RGB565（最常用）
#define ST7789_18BIT_RGB666      0x06    // 18bit 色彩

// 2. 帧率设置 FRCTRL2 (C6h)
#define ST7789_FR_60HZ          0x0F     // 60Hz 标准刷新率
#define ST7789_FR_50HZ          0x11     // 50Hz
#define ST7789_FR_70HZ          0x0D     // 70Hz

// 3. 显示反转控制 INVON/INVOFF
#define ST7789_INV_ON           0x21     // 开启反转（防残影、底色均匀）
#define ST7789_INV_OFF          0x20     // 关闭反转

// 4. 系统命令
#define ST7789_SLP_OUT          0x11     // 退出睡眠
#define ST7789_DISP_ON          0x29     // 开启显示

// 5. VCOM 电压调节 (BBh)
#define ST7789_VCOM_19          0x19     // 通用默认
#define ST7789_VCOM_15          0x15     // 偏暗
#define ST7789_VCOM_20          0x20     // 偏亮

//==================== ST7789 屏幕方向宏定义
// 基准方向：0x20 = 0° 正常
//====================
#define  LCD_DIR_0         0x20    // 0°      基准（正常）
#define  LCD_DIR_90        0x60    // 90°     顺时针旋转
#define  LCD_DIR_180       0xE0    // 180°    旋转180°
#define  LCD_DIR_270       0xA0    // 270°    逆时针旋转

// 镜像方向（基于 0x20 基准）
#define  LCD_MIRROR_LR     0x40    // 左右镜像（水平翻转）
#define  LCD_MIRROR_UD     0x00    // 上下镜像（垂直翻转）

void ST7789_LCD_WR_DATA8(uint8_t dat);

void ST7789_LCD_WR_DATA(uint16_t dat);

void ST7789_LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ST7789_LCD_Clear(uint16_t Color);
void ST7789_LCD_Init(void);
void ST7789_LCD_DisplayOff(void);
void ST7789_LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t sizeS, uint8_t *p);
void ST7789_LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t sizeS);
void ST7789_LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void ST7789_LCD_DrawLine_Width(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t width, uint16_t color);
#endif /* __INF_LCD_ST7789_H__ */
