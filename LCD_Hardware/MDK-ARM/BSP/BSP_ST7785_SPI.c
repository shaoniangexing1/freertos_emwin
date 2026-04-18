#include "BSP_ST7785_SPI.h"

void SPI_Init(void)
{
}

void SPI_WriteByte(uint8_t data)
{
    uchar i;
    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
            MOSI_HIGH;
        else
            MOSI_LOW;
        data = data << 1;
        SCK_LOW;
        SCK_HIGH;
    }
}

uint8_t SPI_ReadByte(void)
{
    uchar i;
    uchar ret_data = 0;
    SCK_HIGH;
    for (i = 0; i < 8; i++)
    {
        // Rom_OUT=1;
        SCK_LOW;
        ret_data = ret_data << 1;
        if (MISO_READ==1)
            ret_data = ret_data + 1;
        else
            ret_data = ret_data + 0;
        SCK_HIGH;
    }
    return (ret_data);
}
