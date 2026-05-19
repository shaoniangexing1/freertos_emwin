#include "Inf_FT6236.h"
/****************************************************
* 函数名称 ：
* 功    能 ：单片机发送起始信号
* 入口参数 ：无
* 返回参数 ：无
* 注意事项 ：
*****************************************************/

void FT6236_Start(void)					
{
	//SDA_OUT();     		//sda线输出
	// delay_us(3);
	 FT6236_SDA_HIGH;									
	 FT6236_SCL_HIGH;		//SCL最小高电平脉宽:0.6us
	 delay_us(4);		//起始信号的最小建立时间:0.6us
	 FT6236_SDA_LOW;		//SCL高电平期间，SDA的一个下降沿表示起始信号
	 delay_us(4);		//起始信号的最小保持时间:0.6us
	FT6236_SCL_LOW;		//箝住总线,为发送器件地址做准备;
	 delay_us(2);		//SCL最小低电平脉宽:1.2us,由RET实现
	//I2C_Start();
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机发送停止信号
* 入口参数 ：无
* 返回参数 ：无
* 注意事项 ：
*****************************************************/
void FT6236_Stop(void)							
{
	//SDA_OUT();     		//sda线输出	
	// delay_us(3);	
	 FT6236_SCL_HIGH;		//SCL最小高电平脉宽:0.6us		
	 delay_us(4);		//停止信号的最小建立时间:0.6us
	FT6236_SDA_LOW;	
	 delay_us(4);
	 FT6236_SDA_HIGH;		//SCL高电平期间，SDA的一个上升沿表示停止信号
	 delay_us(2);	
	//I2C_Stop();					
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机发送应答信号
* 入口参数 ：无
* 返回参数 ：无
* 注意事项 ：单片机读1B数据后发送一个应答信号
*****************************************************/
void FT6236_McuACK(void)							
{
	FT6236_SCL_LOW;	
	//SDA_OUT();     		//sda线输出	
	delay_us(3);
	FT6236_SDA_LOW;	
	delay_us(2);																	
	FT6236_SCL_HIGH;		//SCL最小高电平脉宽:0.6us
	delay_us(2);
	FT6236_SCL_LOW;		//SCL最小低电平脉宽:1.2us
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机发送非应答信号
* 入口参数 ：无
* 返回参数 ：无
* 注意事项 ：单片机读数据停止前发送一个非应答信号
*****************************************************/
void FT6236_McuNACK(void)
{
	FT6236_SCL_LOW;	
	//SDA_OUT();     				//sda线输出	
	delay_us(3);
	FT6236_SDA_HIGH;	
	delay_us(2);																	
	FT6236_SCL_HIGH;				//SCL最小高电平脉宽:0.6us
	delay_us(2);
	FT6236_SCL_LOW;				//SCL最小低电平脉宽:1.2us
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机检查FT6236送来的应答信号
* 入口参数 ：无
* 返回参数 ：1，接收应答失败
			 0，接收应答成功
* 注意事项 ：单片机写1个地址/数据后检查
			 全局变量RevAckF:收到FT6236应答信号的标志位,为0表示收到
*****************************************************/
uint8_t FT6236_CheckAck(void)							
{
	uint8_t ucErrTime=0;
	//SDA_IN();      				//SDA设置为输入
	FT6236_SDA_HIGH;
	FT6236_SCL_HIGH;				//使SDA上数据有效;SCL最小高电平脉宽:0.6us
	delay_us(3);
	while(FT6236_SDA_READ == GPIO_PIN_SET)
	{	
		ucErrTime++;
		if(ucErrTime>250)		//无应答
		{
			FT6236_Stop();	
			return 1;
		}
		delay_us(2);
	}
	FT6236_SCL_LOW;
	return 0;
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机向IIC总线发送1B的地址/数据
* 入口参数 ：待发送的1B地址/数据
* 返回参数 ：无
* 注意事项 ：不是一个完整的数据发送过程;送数的顺序是从高到低
*****************************************************/
void FT6236_WrOneByte(uint8_t dat)						
{
	uint8_t i;						
	//SDA_OUT();     				//sda线输出	
	FT6236_SCL_LOW;				//拉低时钟开始数据传输
	delay_us(3);
	for(i = 8; i > 0; i--)		//8位1B地址/数据的长度
	{
		if(dat & 0x80) 		
			FT6236_SDA_HIGH;		//发送"1"		
		else
			FT6236_SDA_LOW;		//发送"0"
		FT6236_SCL_HIGH;			//使SDA上的数据有效
		delay_us(2);			//SCL最小高电平脉宽:0.6us							
		FT6236_SCL_LOW;			//SCL最小低电平脉宽:1.2us
		delay_us(2);
		dat <<= 1;				//发送数据左移1位,为下位发送准备	
	}	
	//I2C_WriteByte(dat);
}

/****************************************************
* 函数名称 ：
* 功    能 ：单片机从IIC总线接收1B的数据
* 入口参数 ：无
* 返回参数 ：收到的1B数据
* 注意事项 ：不是一个完整的数据接收过程;从高到低的顺序接收数据
*****************************************************/
uint8_t FT6236_RdOneByte(void)						
{
	uint8_t i,dat = 0;				//接收数据位数和内容暂存单元
	//SDA_IN();						//SDA设置为输入
	delay_us(2);	
	FT6236_SDA_HIGH;			//使能上拉,准备读取数据
	delay_us(2);
	for(i = 8;i > 0;i--)
	{
		FT6236_SCL_LOW;
		delay_us(2);
		FT6236_SCL_HIGH;
		dat <<= 1;
		if(FT6236_SDA_READ==GPIO_PIN_SET)
			dat++;
		delay_us(2);			//SCL最小低电平脉宽:1.2us
	}
	FT6236_SDA_HIGH;		
	return(dat);				//返回1B的数据
}

//向FT6236写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
uint8_t FT6236_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	uint8_t ret=0;
	FT6236_Start();	 
	FT6236_WrOneByte(FT_CMD_WR);	//发送写命令 	 
	FT6236_CheckAck(); 	 										  		   
	FT6236_WrOneByte(reg&0XFF);   	//发送低8位地址
	FT6236_CheckAck();  
	for(i=0;i<len;i++)
	{	   
    	FT6236_WrOneByte(buf[i]);  	//发数据
		ret=FT6236_CheckAck();
		if(ret)break;  
	}
    FT6236_Stop();					//产生一个停止条件	    
	return ret; 
}
//从FT6236读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void FT6236_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i; 
 	FT6236_Start();	
 	FT6236_WrOneByte(FT_CMD_WR);   	//发送写命令 	 
	FT6236_CheckAck(); 	 										  		   
 	FT6236_WrOneByte(reg&0XFF);   	//发送低8位地址
	FT6236_CheckAck();  
 	FT6236_Start();  	 	   
	FT6236_WrOneByte(FT_CMD_RD);   	//发送读命令		   
	FT6236_CheckAck();	  
	for(i=0;i<len;i++)
	{	   
		*buf++ = FT6236_RdOneByte();		//读入1B数据到接收数据缓冲区中
		FT6236_McuACK();					//发送应答位	  
	} 
	FT6236_McuNACK();						//n个字节读完,发送非应答位
    FT6236_Stop();					//产生一个停止条件	  
} 
/* 
**函数名：FT6236_Init
**传入参数：无
**返回值：无
**功能：初始化FT6236引脚
*/  
void FT6236_Init(void)
{
	uint8_t temp; 
	// GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO_InitTypeDef类型的结构体
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	//使能PG端口时钟

	// /******  PG8:时钟SCL-----PG7:数据SDA  ***********************/	
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;	//PG7,PG8	
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   	//推挽输出
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//最高输出速率50MHz	
	// GPIO_Init(GPIOG,&GPIO_InitStructure);					//初始化对应GPIOG	
	// GPIO_SetBits(GPIOG,GPIO_Pin_7 | GPIO_Pin_8);			//PG.7.8 输出高	
	// /******  PC7:片选RST  ***************************************/	
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//PC7	
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz	
	// GPIO_Init(GPIOC,&GPIO_InitStructure);					//初始化对应GPIOC		
	// GPIO_SetBits(GPIOC,GPIO_Pin_7);							//PC.7 输出高
	// /**********************PC6，中断INT*********************/
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//PC6	
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	//设置成浮空输入	
	// GPIO_Init(GPIOC,&GPIO_InitStructure);					//初始化对应GPIOC	
	// GPIO_SetBits(GPIOC,GPIO_Pin_6);	
	//HAL_GPIO_WritePin(FT6236U_INT_GPIO_Port, FT6236U_INT_Pin, GPIO_PIN_RESET);
	FT6236_RST_LOW;
	HAL_Delay(50);
	//HAL_GPIO_WritePin(FT6236U_INT_GPIO_Port, FT6236U_INT_Pin, GPIO_PIN_SET);
	FT6236_RST_HIGH;
	HAL_Delay(100);
	FT6236_SDA_HIGH;
	FT6236_SCL_HIGH;
	HAL_Delay(10);
	temp=0;
	FT6236_WR_Reg(FT_DEVIDE_MODE,&temp,1);	//进入正常操作模式 
 	temp=22;								//触摸有效值，22，越小越灵敏	
 	FT6236_WR_Reg(FT_ID_G_THGROUP,&temp,1);	//设置触摸有效值
 	temp=12;								//激活周期，不能小于12，最大14
 	FT6236_WR_Reg(FT_ID_G_PERIODACTIVE,&temp,1); 
/******************************************************/
}
const uint16_t FT6236_TPX_TBL[5]=
{
	FT_TP1_REG,
	FT_TP2_REG,
	FT_TP3_REG,
	FT_TP4_REG,
	FT_TP5_REG
};
TouchPointRefTypeDef TPR_Structure; 
void FT6236_Scan(void)
{
	uint8_t i=0;
	uint8_t sta = 0;
	uint8_t buf[4] = {0};    
	FT6236_RD_Reg(0x02,&sta,1);//读取触摸点的状态  	   
 	if(sta & 0x0f)	//判断是否有触摸点按下，0x02寄存器的低4位表示有效触点个数
 	{
 		TPR_Structure.TouchSta = ~(0xFF << (sta & 0x0F));	//~(0xFF << (sta & 0x0F))将点的个数转换为触摸点按下有效标志
 		for(i=0;i<5;i++)	                                //分别判断触摸点1-5是否被按下
 		{
 			if(TPR_Structure.TouchSta & (1<<i))			    //读取触摸点坐标
 			{											    //被按下则读取对应触摸点坐标数据
 				FT6236_RD_Reg(FT6236_TPX_TBL[i],buf,4);	//读取XY坐标值
				TPR_Structure.x[i]=((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
				TPR_Structure.y[i]=((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
 				if((buf[0]&0XC0)!=0X80)
 				{
					TPR_Structure.x[i]=TPR_Structure.y[i]=0;//必须是contact事件，才认为有效	
					return;
				}
 			}
 		}
 		TPR_Structure.TouchSta |= TP_PRES_DOWN;     //触摸按下标记
 	}
 	else
 	{
 		if(TPR_Structure.TouchSta &TP_PRES_DOWN) 	//之前是被按下的
 			TPR_Structure.TouchSta &= ~0x80;        //触摸松开标记	
 		else
 		{
 			TPR_Structure.x[0] = 0;
 			TPR_Structure.y[0] = 0;
 			TPR_Structure.TouchSta &=0xe0;	//清楚触摸点有效标记
 		}
 	}
}
