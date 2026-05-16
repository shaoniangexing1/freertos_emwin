#ifndef __FREERTOS_H__
#define __FREERTOS_H__

#include "Inf_ST7785_LCD.h"
#include "ComGlobal.h"
#include "GUI.h"
#include "IWDG.h"
#include "stdio.h"
#include "WM.h"

extern void SPI2_Init_Set_Prescaler(uint32_t prescaler);

void app_start(void);



#endif /* __FREERTOS_H__ */
