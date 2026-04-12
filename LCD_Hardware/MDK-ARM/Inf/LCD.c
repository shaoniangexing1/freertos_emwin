#include "Inf_ST7785_LCD.h"

extern uint8_t key1_pressed;
void start_flag(void)
{
    //taskENTER_CRITICAL();
    I2C_Start();
   // taskEXIT_CRITICAL();
}
void stop_flag(void)
{
    //taskENTER_CRITICAL();
    I2C_Stop();
    //taskEXIT_CRITICAL();
}
void transfer(uint8_t byte)
{
   // taskENTER_CRITICAL();
    I2C_WriteByte(byte);
   // taskEXIT_CRITICAL();
}

void send_command_to_ROM( uchar datu )
{
    SPI_WriteByte( datu );
}
static uchar get_data_from_ROM( )
{
    return SPI_ReadByte( );
}

void delay(uint16_t ms)
{
    HAL_Delay(ms);
}

void waitkey()
{
    while(!key1_pressed){
		vTaskDelay(50);
		if (key1_pressed)
		{
			break;
		}
	};
	key1_pressed = 0;
}

void clear_screen()
{
	int i,j;
	for(j=0;j<10;j++)
	{
		start_flag();		/*开始标志*/
		transfer(0x7e);		/*选择SLAVE ADDRESS*/
		transfer(0x00);		/*控制字：表示以下传输的N个字节是指令*/
		transfer(0x38);		/*功能设置：MX=0,MY=0,PD=0,H1=0,H0=0,*/
		transfer(0x40+j); 	/*设置页地址*/
		transfer(0xe0);		/*设置页地址低位*/
		transfer(0xf0);		/*设置页地址高位*/
		stop_flag();
		start_flag();
		transfer(0x7e);
		transfer(0x40);		/*控制字：表示以下传输的N个字节都是显示数据*/
		for(i=0;i<132;i++)
		{
			transfer(0x00);
		}
		stop_flag();
	}
}

void get_n_bytes_data_from_ROM(uchar addrHigh,uchar addrMid,uchar addrLow,uchar *pBuff,uchar DataLen )
{
	uchar i;
	IC_CS_LOW;
	SCK_LOW;
	send_command_to_ROM(0x03);
	send_command_to_ROM(addrHigh);
	send_command_to_ROM(addrMid);
	send_command_to_ROM(addrLow);
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();
	IC_CS_HIGH;
}


void display_graphic_128x64(uint page,uint column,uchar *dp)
{
	int i,j;

	uchar page_address;
	uchar column_address_L,column_address_H;
	page=page-1;
	page_address = 0x40+page;
	column=column+3;
	column_address_L =0xe0+(column&0x0f);
	column_address_H =0xf0+((column>>4)&0x0f);

	for(j=0;j<8;j++)
	{
		start_flag();				//开始标志
		transfer(0x7e);	        	//选择SLAVE ADDRESS
		transfer(0x00);				//控制字：表示以下传输的N个字节是指令
		transfer(0x38);				//功能设置：MX=0,MY=0,PD=0,H1=0,H0=0
		transfer(page_address+j); 	//设置页地址
		transfer(column_address_L);	//设置页地址低位
		transfer(column_address_H);	//设置页地址高位
		stop_flag();				//停止标志
		start_flag();				//开始标志
		transfer(0x7e);				//选择SLAVE ADDRESS
		transfer(0x40);				//控制字：表示以下传输的N个字节都是显示数据
		for(i=0;i<128;i++)
		{
			transfer(*dp);
			dp++;
		}
		transfer(0x00);
		stop_flag();
	}
}//

void display_graphic_32x32(uint page,uint column,uchar *dp)
{
	int i,j;

	uchar page_address;
	uchar column_address_L,column_address_H;
	page=page-1;
	page_address = 0x40+page;
	column=column+3;
	column_address_L =0xe0+(column&0x0f);
	column_address_H =0xf0+((column>>4)&0x0f);


	for(j=0;j<4;j++)
	{
		start_flag();			/*开始标志*/
		transfer(0x7e);	        	/*选择SLAVE ADDRESS*/
		transfer(0x00);			/*控制字：表示以下传输的N个字节是指令*/
		transfer(0x38);			/*功能设置：MX=0,MY=0,PD=0,H1=0,H0=0,*/
		transfer(page_address+j); 	/*设置页地址*/
		transfer(column_address_L);	/*设置页地址低位*/
		transfer(column_address_H);	/*设置页地址高位*/
		stop_flag();			/*停止标志*/
		start_flag();			/*开始标志*/
		transfer(0x7e);			/*选择SLAVE ADDRESS*/
		transfer(0x40);			/*控制字：表示以下传输的N个字节都是显示数据*/
		for(i=0;i<31;i++)
		{
			transfer(*dp);
			dp++;
		}
		transfer(0x00);
		stop_flag();
	}
}//


void display_graphic_16x16(uint page,uint column,uchar *dp)
{
	int i,j;

	uchar page_address;
	uchar column_address_L,column_address_H;
	page=page-1;
	page_address = 0x40+page;
	column=column+3;
	column_address_L =0xe0+(column&0x0f);
	column_address_H =0xf0+((column>>4)&0x0f);


	for(j=0;j<2;j++)
	{
		start_flag();			/*开始标志*/
		transfer(0x7e);			/*选择SLAVE ADDRESS*/
		transfer(0x00);			/*控制字：表示以下传输的N个字节是指令*/
		transfer(0x38);			/*功能设置：MX=0,MY=0,PD=0,H1=0,H0=0,*/
		transfer(page_address+j); 	/*设置页地址*/
		transfer(column_address_L);	/*设置页地址低位*/
		transfer(column_address_H);	/*设置页地址高位*/
		stop_flag();
		start_flag();
		transfer(0x7e);
		transfer(0x40);	/*控制字：表示以下传输的N个字节都是显示数据*/
		for(i=0;i<16;i++)
		{
			transfer(*dp);
			dp++;
		}
		stop_flag();
	}
}//

void display_graphic_8x16(uint page,uint column,uchar *dp)
{
	int i,j;

	uchar page_address;
	uchar column_address_L,column_address_H;
	page=page-1;
	page_address = 0x40+page;
	column=column+3;
	column_address_L =0xe0+(column&0x0f);
	column_address_H =0xf0+((column>>4)&0x0f);


	for(j=0;j<2;j++)
	{
		start_flag();		    	/*开始标志*/
		transfer(0x7e);	            	/*选择SLAVE ADDRESS*/
		transfer(0x00);	            	/*控制字：表示以下传输的N个字节是指令,直到STOP_FLAG(停止标志为止)*/
		transfer(0x38);	            	/*功能设置：MX=0,MY=0,PD=0,H1=0,H0=0,*/
		transfer(page_address+j);   	/*设置页地址*/
		transfer(column_address_L); 	/*设置页地址低位*/
		transfer(column_address_H); 	/*设置页地址高位*/
		stop_flag();				    
		start_flag();
		transfer(0x7e);
		transfer(0x40);	           	/*控制字：表示以下传输的N个字节都是显示数据*/
		for(i=0;i<8;i++)
		{
			transfer(*dp);
			dp++;
		}
		stop_flag();
	}
}//


/*显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标*/
void display_graphic_5x7(uint page,uchar column,uchar *dp)
{
	uint col_cnt;
	uchar page_address;
	uchar column_address_L,column_address_H;
	page=page-1;
	page_address = 0x40+page;
	column=column+3;
	column_address_L =0xe0+(column&0x0f);
	column_address_H =0xf0+((column>>4)&0x0f);
	
	start_flag();		    	/*开始标志*/
	transfer(0x7e);	            	/*选择SLAVE ADDRESS*/
	transfer(0x00);	            	/*控制字：表示以下传输的N个字节是指令,直到STOP_FLAG(停止标志为止)*/
	transfer(0x38);	            	/*功能设置：MX=0,MY=0,PD=0,H1=0,H0=0,*/
	transfer(page_address); 		/*Set Page Address*/
	transfer(column_address_H);	/*Set MSB of column Address*/
	transfer(column_address_L);	/*Set LSB of column Address*/

	stop_flag();				    
	start_flag();
	transfer(0x7e);
	transfer(0x40);
	
	for (col_cnt=0;col_cnt<6;col_cnt++)
	{	
		transfer(*dp);		/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
		dp++;
	}
	stop_flag();
}

/******************************************************************/
ulong  fontaddr=0;
void display_GB2312_string(uchar y,uchar x,uchar *text)
{
	uchar i= 0;
	uchar addrHigh,addrMid,addrLow ;
	uchar fontbuf[32];			
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{						
			/*国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (ulong)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2; 
			x+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			/*国标简体（GB2312）15x16点的字符在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (ulong)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[16];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );/*取16个字节的数据，存到"fontbuf[32]"*/
			
			display_graphic_8x16(y,x,fontbuf);/*显示8x16的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=1;
			x+=8;
		}
		else
			i++;	
	}
	
}

void display_string_5x7(uchar y,uchar x,uchar *text)
{
	unsigned char i= 0;
	unsigned char addrHigh,addrMid,addrLow ;
	while((text[i]>0x00))
	{
		
		if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[8];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*8);
			fontaddr = (unsigned long)(fontaddr+0x3bfc0);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,8);/*取8个字节的数据，存到"fontbuf[32]"*/
			
			display_graphic_5x7(y,x,fontbuf);/*显示5x7的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=1;
			x+=6;
		}
		else
		i++;	
	}	
}

void initial_lcd()
{
	
	start_flag();       //开始标志

	transfer(0x7e);		//选择SLAVE ADDRESS
	transfer(0x00);		//表示以下传输的字节是指令*/
	transfer(0x3b);		//功能设置：MX=1,MY=1,PD=0,H1=1,H0=1,
	transfer(0x03);		//软件复位*/
	transfer(0x38);		//功能设置：MX=1,MY=1,PD=0,H1=0,H0=0,
	transfer(0x05);		//设置VLCD的范围：0X05表示高电压 0X04表示低电压
	transfer(0x0c);		//打开显示
	transfer(0x39);		//功能设置：MX=1,MY=1,PD=0,H1=0,H0=1,
	transfer(0x08);		//显示配置：DO=0,V=0,(Top/bottom row mode set data order)
	transfer(0x10);		//BIAS设置为：0x10:1/11,0x11:1/10,0x12:1/9
	transfer(0x8c);		//对比度设置：最低是0x80，最高是0xff，数值越大对比度就越高

	stop_flag();		//结束标志
}

void LCD_Start()
{
	LCD_RST_LOW;
	delay(100);
	LCD_RST_HIGH;
	initial_lcd();
	//LCD_SetContrast(0xD0);  // 推荐值：CF / D0 / DF
}
// 对比度设置函数（ST7785/SSD1306 通用）
void LCD_SetContrast(uint8_t contrast)
{
    start_flag();
    transfer(0x7E);    // I2C 地址
    transfer(0x00);    // 命令标志
    transfer(0x81);    // 对比度指令
    transfer(contrast);// 对比度值 0~255
    stop_flag();
}
void LCD_demo(void)
{
	 	clear_screen();    							//clear all dots
		display_graphic_128x64(1,1,bmp_12864_3); 	//在第1页，第1列显示128x64点阵的图片		
		waitkey();
		display_graphic_128x64(1,1,bmp_12864_2); 	//在第1页，第1列显示128x64点阵的图片		
		waitkey();
		display_graphic_128x64(1,1,bmp_12864_1); 	//在第1页，第1列显示128x64点阵的图片		
		waitkey();
										
		clear_screen();    							//clear all dots
		display_graphic_32x32(1,1,yun1); 			//在第1页，第1列显示单个汉字"运"
		display_graphic_32x32(1,(1+32+16),xing1); 	//在第1页，第49列显示单个汉字"行"
		display_graphic_32x32(1,(1+32*3),zhong1); 	//在第1页，第97列显示单个汉字"中"
		
		display_graphic_16x16(5,1,ge1);          	//"歌"
		display_graphic_16x16(5,17,qu1);     		//"曲"
		display_graphic_16x16(5,33,ming1); 			//"名"
		display_graphic_8x16(5,49,mao_hao);      	//":"
		display_graphic_16x16(5,57,chuan1);      	//传
		display_graphic_16x16(5,73,qi1);         	//"奇"
		
		
		display_graphic_16x16(7,1,yan1);          	//"演"
		display_graphic_16x16(7,17,chang1);   		//"唱"
		display_graphic_16x16(7,33,zhe1);   		//"者"
		display_graphic_8x16(7,49,mao_hao);       	//":"
		display_graphic_16x16(7,57,wang1);        	//"王"
		display_graphic_16x16(7,73,fei1);         	//"菲"		
		waitkey();

		clear_screen(); 
		display_GB2312_string(1,1,(unsigned char *)"JLX12864G-200-PC");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
		//display_GB2312_string(3,1,(unsigned char *)"16X16简体汉字库,");
		//display_GB2312_string(5,1,(unsigned char *)"或8X16点阵ASCII,");  /*显示一串16x16点阵汉字或8x16的ASCII字.以下雷同*/
		//display_GB2312_string(7,1,(unsigned char *)"或5X7点阵ASCII码");
		waitkey();

		clear_screen();			
		display_GB2312_string(1,1,(unsigned char *)"abcdefghijklmnoprstuvwxyz");	/*在第1页，第1列，显示一串16x16点阵汉字或8*16的ASCII字*/
		display_string_5x7(3,1,(unsigned char *)"abcdefghijklmnopqrstu");/*在第3页，第1列，显示一串5x7点阵的ASCII字*/
		display_string_5x7(4,1,(unsigned char *)"JLX electronics Co., ");/*显示一串5x7点阵的ASCII字*/
		display_string_5x7(5,1,(unsigned char *)"Ltd. established at  ");/*显示一串5x7点阵的ASCII字*/	
		display_string_5x7(6,1,(unsigned char *)"year 2004.Focus LCM. ");/*显示一串5x7点阵的ASCII字*/
		display_string_5x7(7,1,(unsigned char *)"TEL:TEL:0755-29784961    ");/*显示一串5x7点阵的ASCII字*/
		display_string_5x7(8,1,(unsigned char *)"FAX:0755-29784964    ");/*显示一串5x7点阵的ASCII字*/
		waitkey();

		display_graphic_128x64(1,1,bmp_12864_4); 	//在第1页，第1列显示128x64点阵的图片		
		waitkey();
		display_graphic_128x64(1,1,bmp_12864_5); 	//在第1页，第1列显示128x64点阵的图片		
		waitkey();
		display_graphic_128x64(1,1,bmp_12864_6); 	//在第1页，第1列显示128x64点阵的图片		
		waitkey();
}


void LCD_FlushBuffer(uint8_t *pBuf, uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end) {
    uint8_t page, col;
    for (page = page_start; page <= page_end; page++) {
        // 复用display_graphic_128x64的页/列寻址逻辑
        uchar page_address = 0x40 + (page-1);
        uchar column_address_L = 0xe0 + (col_start & 0x0f);
        uchar column_address_H = 0xf0 + ((col_start >>4) & 0x0f);

        start_flag();
        transfer(0x7e);
        transfer(0x00);
        transfer(0x38);
        transfer(page_address);
        transfer(column_address_L);
        transfer(column_address_H);
        stop_flag();

        start_flag();
        transfer(0x7e);
        transfer(0x40);
        for (col = col_start; col <= col_end; col++) {
            transfer(pBuf[(page-1)*128 + col]); // 按页取缓冲区数据
        }
        stop_flag();
    }
}
