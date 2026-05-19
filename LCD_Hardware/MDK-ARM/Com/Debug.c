#include "Debug.h"

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) { __NOP();}
}
void delay_ms(uint32_t ms)
{
    while (ms--) delay_us(1000);
}

    