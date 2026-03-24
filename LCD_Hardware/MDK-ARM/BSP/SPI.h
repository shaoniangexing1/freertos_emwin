#ifndef __SPI_H__
#define __SPI_H__

#include "gpio.h"
#include "I2C.h"

#define SCK_HIGH HAL_GPIO_WritePin(IC_SCK_GPIO_Port, IC_SCK_Pin, GPIO_PIN_SET)
#define SCK_LOW  HAL_GPIO_WritePin(IC_SCK_GPIO_Port, IC_SCK_Pin, GPIO_PIN_RESET)

#define MOSI_HIGH HAL_GPIO_WritePin(IC_MOSI_GPIO_Port, IC_MOSI_Pin, GPIO_PIN_SET)
#define MOSI_LOW  HAL_GPIO_WritePin(IC_MOSI_GPIO_Port, IC_MOSI_Pin, GPIO_PIN_RESET)

#define CS_HIGH HAL_GPIO_WritePin(IC_CS_GPIO_Port, IC_CS_Pin, GPIO_PIN_SET)
#define CS_LOW  HAL_GPIO_WritePin(IC_CS_GPIO_Port, IC_CS_Pin, GPIO_PIN_RESET)

#define MISO_READ HAL_GPIO_ReadPin(IC_MISO_GPIO_Port, IC_MISO_Pin)

void SPI_Init(void);

void SPI_WriteByte(uint8_t data);

uint8_t SPI_ReadByte(void);

#endif /* __SPI_H__ */
