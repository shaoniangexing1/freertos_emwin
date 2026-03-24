#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include "gpio.h"


#define uchar uint8_t 
#define uint uint16_t
#define ulong uint32_t

#define IIC_SCL_HIGH HAL_GPIO_WritePin(IIC_SCK_GPIO_Port, IIC_SCK_Pin, GPIO_PIN_SET)
#define IIC_SCL_LOW  HAL_GPIO_WritePin(IIC_SCK_GPIO_Port, IIC_SCK_Pin, GPIO_PIN_RESET)

#define IIC_SDA_HIGH HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET)
#define IIC_SDA_LOW  HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_RESET)

void I2C_Init(void);

void I2C_Start(void);

void I2C_Stop(void);

void I2C_WriteByte(uint8_t byte);

uint8_t I2C_ReadByte(uint8_t ack);


#endif /* __I2C_H__ */
