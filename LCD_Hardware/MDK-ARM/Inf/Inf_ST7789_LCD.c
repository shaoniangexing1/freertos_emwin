#include "Inf_ST7789_LCD.h"
#include "stdlib.h"
#include "font.h"
#include "spi.h"
// #include "delay.h"

void DelayM(uint16_t a)
{ // 延时函数 1MS/次
	HAL_Delay(a);
}

//================写一个字节=======================
void ST7789_SPI2_WriteByte(uint8_t Byte)
{
	HAL_SPI_Transmit(&hspi2, &Byte, 1, 100);
	//HAL_SPI_Transmit_DMA(&hspi2, &Byte, 1);
}

//================写命令=======================
void ST7789_LCD_WR_CMD(uint8_t cmd)
{
	// ST7789_LCD_RS=0;       //select command，选择命令
	// ST7789_LCD_CS=0;	   //chip select,打开片选
	// DATAIO=cmd; //put command，放置命令
	// ST7789_LCD_WR=0;       //select write，选择写模式
	// ST7789_LCD_WR=1;	   //command writing ，写入命令
	// ST7789_LCD_CS=1;
	LCD_ST7789_DCorRS_LOW; // DC=0 = 命令
	LCD_ST7789_CS_LOW;	   // 片选拉低

	ST7789_SPI2_WriteByte(cmd); // SPI 发送命令

	LCD_ST7789_CS_HIGH; // 片选拉高
}

//===================写8位数据 （显示数据）=========================

void ST7789_LCD_WR_DATA8(uint8_t dat)
{
	// ST7789_LCD_RS = 1;	  // 选择数据
	// ST7789_LCD_CS = 0;	  // 打开片选
	// DATAIO = dat; // put data，放置数据
	// ST7789_LCD_WR = 0;	  // 选择写模式
	// ST7789_LCD_WR = 1;	  // 写数据
	// ST7789_LCD_CS = 1;
	LCD_ST7789_DCorRS_HIGH; // DC=1 = 数据
	LCD_ST7789_CS_LOW;

	ST7789_SPI2_WriteByte(dat);

	LCD_ST7789_CS_HIGH;
}

//===================写16位数据 （显示数据）=========================
void ST7789_LCD_WR_DATA(uint16_t dat)
{
	//	ST7789_LCD_WR_DATA8(dat>>8);
	//	ST7789_LCD_WR_DATA8(dat);

	// ST7789_LCD_RS = 1; // 选择数据
	// ST7789_LCD_CS = 0; // 打开片选

	// DATAIO = dat >> 8; // put data，放置数据
	// ST7789_LCD_WR = 0;		   // 选择写模式
	// ST7789_LCD_WR = 1;		   // 写数据
	// DATAIO = dat;	   // 放置数据
	// ST7789_LCD_WR = 0;		   // 选择写模式
	// ST7789_LCD_WR = 1;		   // 写数据

	// ST7789_LCD_CS = 1;

	LCD_ST7789_DCorRS_HIGH;
	LCD_ST7789_CS_LOW;

	ST7789_SPI2_WriteByte(dat >> 8);   // 高8位
	ST7789_SPI2_WriteByte(dat & 0xFF); // 低8位

	LCD_ST7789_CS_HIGH;
}

void ST7789_LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	ST7789_LCD_WR_CMD(0x2a); // 列地址设置
	ST7789_LCD_WR_DATA(x1);
	ST7789_LCD_WR_DATA(x2);
	ST7789_LCD_WR_CMD(0x2b); // 行地址设置
	ST7789_LCD_WR_DATA(y1);
	ST7789_LCD_WR_DATA(y2);
	ST7789_LCD_WR_CMD(0x2c); // 储存器写
}

/******************************************************************************
	  函数说明：ST7789_LCD清屏函数
	  入口数据：无
	  返回值：  无
******************************************************************************/
void ST7789_LCD_Clear(uint16_t Color)
{
	u32 i;
	ST7789_LCD_Address_Set(0, 0, 239, 239);
	for (i = 0; i < 57600; i++)
	{
		ST7789_LCD_WR_DATA(Color);
		//			DelayM(500);
	}
}

/******************************************************************************
	  函数说明：ST7789_LCD初始化函数
	  入口数据：无
	  返回值：  无
******************************************************************************/
void ST7789_LCD_Init(void)
{

	LCD_ST7789_RST_LOW;
	DelayM(200);
	LCD_ST7789_RST_HIGH;
	DelayM(20);
	DelayM(200);

	//************* Start Initial Sequence **********//
	//==================== 0. 屏幕方向设置0x36 ====================
	ST7789_LCD_WR_CMD(0x36);// 内存访问控制（方向/镜像/旋转）
	ST7789_LCD_WR_DATA8(LCD_DIR_0); // 0x20 基准方向（可替换：90/180/270/镜像）

	  //=============== 1. 像素格式设置 ===============
    ST7789_LCD_WR_CMD(0x3A);               // COLMOD：设置像素格式
    ST7789_LCD_WR_DATA8(ST7789_16BIT_RGB565); // RGB565（16位，通用）

    //=============== 2. 消隐周期设置 ===============
    ST7789_LCD_WR_CMD(0xB2);               // PORCTRL：前后沿消隐设置
    ST7789_LCD_WR_DATA8(0x0C);
    ST7789_LCD_WR_DATA8(0x0C);
    ST7789_LCD_WR_DATA8(0x00);
    ST7789_LCD_WR_DATA8(0x33);
    ST7789_LCD_WR_DATA8(0x33);
    // 作用：消除闪烁、横纹，稳定显示时序

    //=============== 3. 栅极控制 ===============
    ST7789_LCD_WR_CMD(0xB7);               // GCTRL：栅极（行驱动）控制
    ST7789_LCD_WR_DATA8(0x35);
    // 作用：设置VGH/VGL电压、行扫描方向，通用屏默认0x35

    //=============== 4. VCOM 公共电压 ===============
    ST7789_LCD_WR_CMD(0xBB);               // VCOMS：设置公共极电压
    ST7789_LCD_WR_DATA8(ST7789_VCOM_19);
    // 作用：影响底色均匀度，值越大底色越亮

    //=============== 5. LCM 驱动控制 ===============
    ST7789_LCD_WR_CMD(0xC0);               // LCMCTRL：液晶驱动基础设置
    ST7789_LCD_WR_DATA8(0x2C);

    //=============== 6. 使能 VRH/VDV 电压 ===============
    ST7789_LCD_WR_CMD(0xC2);               // VDVVRHEN：开启电压调节
    ST7789_LCD_WR_DATA8(0x01);

    //=============== 7. VRH 电压设置 ===============
    ST7789_LCD_WR_CMD(0xC3);               // VRHS：对比度核心电压
    ST7789_LCD_WR_DATA8(0x12);

    //=============== 8. VDV 电压设置 ===============
    ST7789_LCD_WR_CMD(0xC4);               // VDVS：对比度微调
    ST7789_LCD_WR_DATA8(0x20);

    //=============== 9. 帧率设置 ===============
    ST7789_LCD_WR_CMD(0xC6);               // FRCTRL2：刷新率设置
    ST7789_LCD_WR_DATA8(ST7789_FR_70HZ);   // 60Hz 最稳定

    //=============== 10. 电源控制 ===============
    ST7789_LCD_WR_CMD(0xD0);               // PWRCTRL1：电源&升压设置
    ST7789_LCD_WR_DATA8(0xA4);
    ST7789_LCD_WR_DATA8(0xA1);
    // 通用出厂配置，基本无需修改

    //=============== 11. 正极伽马校正 ===============
    ST7789_LCD_WR_CMD(0xE0);               // GMCTRP1：正极性伽马
    ST7789_LCD_WR_DATA8(0xD0);
    ST7789_LCD_WR_DATA8(0x04);
    ST7789_LCD_WR_DATA8(0x0D);
    ST7789_LCD_WR_DATA8(0x11);
    ST7789_LCD_WR_DATA8(0x13);
    ST7789_LCD_WR_DATA8(0x2B);
    ST7789_LCD_WR_DATA8(0x3F);
    ST7789_LCD_WR_DATA8(0x54);
    ST7789_LCD_WR_DATA8(0x4C);
    ST7789_LCD_WR_DATA8(0x18);
    ST7789_LCD_WR_DATA8(0x0D);
    ST7789_LCD_WR_DATA8(0x0B);
    ST7789_LCD_WR_DATA8(0x1F);
    ST7789_LCD_WR_DATA8(0x23);
    // 作用：调整亮部/暗部层次、色彩过渡

    //=============== 12. 负极伽马校正 ===============
    ST7789_LCD_WR_CMD(0xE1);               // GMCTRN1：负极性伽马
    ST7789_LCD_WR_DATA8(0xD0);
    ST7789_LCD_WR_DATA8(0x04);
    ST7789_LCD_WR_DATA8(0x0C);
    ST7789_LCD_WR_DATA8(0x11);
    ST7789_LCD_WR_DATA8(0x13);
    ST7789_LCD_WR_DATA8(0x2C);
    ST7789_LCD_WR_DATA8(0x3F);
    ST7789_LCD_WR_DATA8(0x44);
    ST7789_LCD_WR_DATA8(0x51);
    ST7789_LCD_WR_DATA8(0x2F);
    ST7789_LCD_WR_DATA8(0x1F);
    ST7789_LCD_WR_DATA8(0x1F);
    ST7789_LCD_WR_DATA8(0x20);
    ST7789_LCD_WR_DATA8(0x23);
    // 与E0配对，保证正负电压对称，显示均匀无残影

    //=============== 13. 显示反转使能 ===============
    ST7789_LCD_WR_CMD(ST7789_INV_ON);
    // 开启反转：防止液晶极化，底色更均匀，减少残影

    //=============== 14. 退出睡眠模式 ===============
    ST7789_LCD_WR_CMD(ST7789_SLP_OUT);
    HAL_Delay(120);    // 手册强制要求：SLPOUT后至少等待120ms

    //=============== 15. 开启显示 ===============
    ST7789_LCD_WR_CMD(ST7789_DISP_ON);

	//清屏
	//ST7789_LCD_Clear(GREEN);//

	
}
void ST7789_LCD_DisplayOff(void)
{
    // 1. 选中LCD设备
    LCD_ST7789_CS_LOW; // 片选拉低，选中LCD
    
    // 2. 先关显示，再进睡眠（避免睡眠过程中画面异常）
    ST7789_LCD_WR_CMD(0x28);  // 关显示输出
    ST7789_LCD_WR_CMD(0x10);  // 进入睡眠模式，关闭内部振荡器
    
    // 3. 释放SPI总线
    LCD_ST7789_CS_HIGH; // 片选拉高，释放总线
    
    // 4. 强制延时120ms，ST7789 datasheet强制要求
    // FreeRTOS环境可替换为：vTaskDelay(pdMS_TO_TICKS(120));
    HAL_Delay(120);

    // 【可选】关闭背光（你的背光引脚可控时添加）
    // HAL_GPIO_WritePin(LCD_BLK_GPIO_Port, LCD_BLK_Pin, GPIO_PIN_RESET);
}

//快速画点
//x,y:坐标
//color:颜色
void ST7789_LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{

//	ST7789_LCD_WR_CMD(ST7789_LCDdev.setxcmd);ST7789_LCD_WR_DATA(x>>8);
//	ST7789_LCD_WR_CMD(ST7789_LCDdev.setxcmd+1);ST7789_LCD_WR_DATA(x&0XFF);
//	ST7789_LCD_WR_CMD(ST7789_LCDdev.setycmd);ST7789_LCD_WR_DATA(y>>8);
//	ST7789_LCD_WR_CMD(ST7789_LCDdev.setycmd+1);ST7789_LCD_WR_DATA(y&0XFF);

	ST7789_LCD_WR_CMD(0x2a);//列地址设置
	ST7789_LCD_WR_DATA(x);
	ST7789_LCD_WR_DATA(x);
	ST7789_LCD_WR_CMD(0x2b);//行地址设置
	ST7789_LCD_WR_DATA(y);
	ST7789_LCD_WR_DATA(y);


	ST7789_LCD_WR_CMD(0x2C);		//储存器写
	ST7789_LCD_WR_DATA(color);		//写数据
}
/******************************************************************************
* 函数名：ST7789_LCD_DrawLine_Width
* 功  能：画任意角度、可设置线宽的直线
* 参  数：x1,y1  起点
*         x2,y2  终点
*         width  线宽（1~任意）
*         color  颜色
******************************************************************************/
void ST7789_LCD_DrawLine_Width(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t width, uint16_t color)
{
	     int16_t dx = abs(x2 - x1);
    int16_t dy = abs(y2 - y1);
    int16_t sx = x1 < x2 ? 1 : -1;
    int16_t sy = y1 < y2 ? 1 : -1;
    int16_t err = dx - dy;
    int16_t e2;
    int16_t w;

    int hw = width / 2;

    while (1)
    {
        // ==============================================
        // 核心：适配你屏幕 XY 互换，直接反写 x/y
        // ==============================================
        for (w = -hw; w <= hw; w++)
        {
            if (dx >= dy)
                ST7789_LCD_Fast_DrawPoint(y1 + w, x1, color);
            else
                ST7789_LCD_Fast_DrawPoint(y1, x1 + w, color);
        }

        if (x1 == x2 && y1 == y2) break;

        e2 = err * 2;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void Set_addr(uint16_t x, uint16_t y, uint8_t size)
{
	//ST7789_LCD_WR_CMD(0x36);
	//ST7789_LCD_WR_DATA8(0x60); // 从右往左 从上到下

	ST7789_LCD_WR_CMD(0x2a); // 列地址设置
	ST7789_LCD_WR_DATA(y);
	ST7789_LCD_WR_DATA(y + size - 1);
	ST7789_LCD_WR_CMD(0x2b); // 行地址设置
	ST7789_LCD_WR_DATA(x);
	ST7789_LCD_WR_DATA(x + size/2 - 1);
	ST7789_LCD_WR_CMD(0x2C);
}

// void xie(uint16_t x,uint16_t y,uint8_t num,uint8_t size)
//{
//	uint8_t temp;
//	uint16_t a,a1;
//	uint8_t x1=0,x2=x1;
//	uint8_t csize=((size/8+((size%8)?1:0))*(size/2));
//	Set_addr(x,y,size);
//	num=num-' ';
//	for(a=0;a<csize;a++)
//	{
//		if(size==12)temp=asc2_1206[num][a]; 	 	//调用1206字体
//		else if(size==16)temp=asc2_1608[num][a];	//调用1608字体
//		else if(size==24)temp=asc2_2412[num][a];	//调用2412字体
//		else return;		//没有的字库
//		for(a1=0;a1<8;a1++)
//		{
//			if(temp&0x80)	ST7789_LCD_WR_DATA(0x0000);
//			else			ST7789_LCD_WR_DATA(0xf800);
//			temp<<=1;
//			x1++;
//			if((x1-x2)==size)
//			{
//				x1=x2;
//				break;
//			}
//		}
//
//	}
//
// }

// void ST7789_LCD_xieyichuan(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
//{
//	uint8_t x0=x;
//	width+=x;
//	height+=y;
//     while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
//     {
//		if(x>=width){x=x0;y+=size;}
//		if(y>=height)break;//退出
//         xie(x,y,*p,size);
//         x+=size/2;
//         p++;
//     }
// }
// 在指定位置显示一个字符
// x,y:起始坐标
// num:要显示的字符:" "--->"~"
// size:字体大小 12/16/24
// mode:叠加方式(1)还是非叠加方式(0)

void ST7789_LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t sizeS)
{
	uint8_t temp;
	uint16_t a, a1;
	uint8_t x1 = 0, x2 = x1;
	uint8_t csize = ((sizeS / 8 + ((sizeS % 8) ? 1 : 0)) * (sizeS / 2));
	Set_addr(x, y, sizeS);
	num = num - ' ';
	for (a = 0; a < csize; a++)
	{
		if (sizeS == 12)
			temp = asc2_1206[num][a]; // 调用1206字体
		else if (sizeS == 16)
			temp = asc2_1608[num][a]; // 调用1608字体
		else if (sizeS == 24)
			temp = asc2_2412[num][a]; // 调用2412字体
		else
			return; // 没有的字库
		for (a1 = 0; a1 < 8; a1++)
		{
			if (temp & 0x80)
				ST7789_LCD_WR_DATA(BLACK);//背景颜色

			else
				ST7789_LCD_WR_DATA(WHITE); //字体颜色
			temp <<= 1;
			x1++;
			if ((x1 - x2) == sizeS)
			{
				x1 = x2;
				break;
			}
		}
	}
	//ST7789_LCD_WR_CMD(0x36);
	//ST7789_LCD_WR_DATA8(0x60);
}

// void ST7789_LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
//{
//     uint8_t temp,t1,t;
//	uint16_t y0=y;
//	uint8_t csize=((size/8+((size%8)?1:0))*(size/2));		//得到字体一个字符对应点阵集所占的字节数
////	uint8_t csize=size*(size/2);
// 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
//	Set_addr(x,y,size);
//
//
//	for(t=0;t<csize;t++)
//	{
//		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
//		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
//		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
//		else return;		//没有的字库
//
//
//
//		for(t1=0;t1<8;t1++)
//		{
////			delay_ms(100);
//			if(temp&0x80)	ST7789_LCD_WR_DATA(0x0000);	//ST7789_LCD_Fast_DrawPoint(x,y,0x0000);
//			else if(mode==0)ST7789_LCD_WR_DATA(0xf800);	//ST7789_LCD_Fast_DrawPoint(x,y,0xffff);
//			temp<<=1;
//			x++;
////			if(x>=240)return;		//超区域了
//			if((x-y0)==size)
//			{
//				x=y0;
////				y++;
////				if(y>=320)return;	//超区域了
//				break;
//			}
//		}
////
//
//
////		for(t1=0;t1<8;t1++)
////		{
//////			delay_ms(5);
////			if(temp&0x80)	ST7789_LCD_Fast_DrawPoint(x,y,0x0000);
////			else if(mode==0)ST7789_LCD_Fast_DrawPoint(x,y,0xffff);
////			temp<<=1;
////			y++;
////			if(y>=320)return;		//超区域了
////			if((y-y0)==size)
////			{
////				y=y0;
////				x++;
////				if(x>=240)return;	//超区域了
////				break;
////			}
////		}
//	}
//		delay_ms(5000);
//}

// 显示字符串
// x,y:起点坐标
// width,height:区域大小
// size:字体大小
//*p:字符串起始地址
void ST7789_LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t sizeS, uint8_t *p)
{
	uint8_t x0 = x;
	width += x;
	height += y;
	while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
	{
		if (x >= width)
		{
			x = x0;
			y += sizeS;
		}
		if (y >= height)
			break; // 退出
		ST7789_LCD_ShowChar(x, y, *p, sizeS);
		x += sizeS / 2;
		p++;
	}
}
