#include "I2C.h"

void I2C_Delay(void)
{
    volatile int i = 40;
    while (i--)
        __NOP();
}
void I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Ensure clocks for the port already enabled by MX_GPIO_Init.
       Reconfigure SDA/SCL to have internal pull-up if hardware lacks external pull-ups. */

    /* SCL pin */
    GPIO_InitStruct.Pin = IIC_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // use internal pull-up if needed
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SCK_GPIO_Port, &GPIO_InitStruct);

    /* SDA pin */
    GPIO_InitStruct.Pin = IIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // use internal pull-up if needed
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);

    /* Release both lines (high) */
    HAL_GPIO_WritePin(IIC_SCK_GPIO_Port, IIC_SCK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET);

    /* small delay for line to settle */
    for (volatile int i = 0; i < 1000; ++i)
        __NOP();

    /* Bus recovery: if SDA stuck low, pulse SCL up to 9 times to free it */
    if (HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) == GPIO_PIN_RESET)
    {
        for (int k = 0; k < 9 && HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) == GPIO_PIN_RESET; ++k)
        {
            HAL_GPIO_WritePin(IIC_SCK_GPIO_Port, IIC_SCK_Pin, GPIO_PIN_SET);
            for (volatile int d = 0; d < 200; ++d)
                __NOP();
            HAL_GPIO_WritePin(IIC_SCK_GPIO_Port, IIC_SCK_Pin, GPIO_PIN_RESET);
            for (volatile int d = 0; d < 200; ++d)
                __NOP();
        }
    }
}

void I2C_Start(void)
{
    // Code to generate I2C start condition
    IIC_SCL_HIGH;
    IIC_SDA_HIGH;
    I2C_Delay();
    IIC_SDA_LOW;
    I2C_Delay();
}

void I2C_Stop(void)
{
    IIC_SCL_HIGH; /*STOP FLAG*/
    IIC_SDA_LOW;  /*STOP FLAG*/
    I2C_Delay();
    IIC_SDA_HIGH; /*STOP FLAG*/
    I2C_Delay();
}

void I2C_WriteByte(uint8_t byte)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        /* drive data while SCL is low */
        IIC_SCL_LOW;
        I2C_Delay();
        if (byte & 0x80)
            IIC_SDA_HIGH;
        else
            IIC_SDA_LOW;
        I2C_Delay();
        /* clock the bit: high then low */
        IIC_SCL_HIGH;
        I2C_Delay();
        IIC_SCL_LOW;
        byte = byte << 1;
    }
    /* Release SDA so slave can ACK (line pulled up by pull-up resistor if idle) */
    IIC_SDA_HIGH;
    I2C_Delay();
    /* Pulse SCL to let slave drive ACK */
    IIC_SCL_HIGH;
    I2C_Delay();
    /* Optionally sample ACK: HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) */
    IIC_SCL_LOW;
    I2C_Delay();
}

uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t byte = 0;
    int i;

    /* Release SDA so slave can drive the line */
    IIC_SDA_HIGH;
    I2C_Delay();

    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        /* Clock high, then read SDA */
        IIC_SCL_HIGH;
        I2C_Delay();
        if (HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) == GPIO_PIN_SET)
            byte |= 0x01;
        IIC_SCL_LOW;
        I2C_Delay();
    }

    /* Send ACK (ack==1) or NACK (ack==0) back to the slave */
    if (ack)
        IIC_SDA_LOW; /* ACK: pull low */
    else
        IIC_SDA_HIGH; /* NACK: release high */
    I2C_Delay();
    IIC_SCL_HIGH;
    I2C_Delay();
    IIC_SCL_LOW;
    /* Release SDA */
    IIC_SDA_HIGH;

    return byte;
}
