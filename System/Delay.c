#include "stm32f10x.h"

/*
 * SysTick based blocking delays.
 *
 * WARNING: these functions stall the CPU completely.
 * They are only acceptable during initialisation and inside the key debounce.
 * Never call them from the main loop of this project.
 */

/* xus range: 0 .. 233015 */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				/* reload value */
	SysTick->VAL = 0x00;					/* clear current counter */
	SysTick->CTRL = 0x00000005;				/* HCLK source, enable */
	while (!(SysTick->CTRL & 0x00010000));	/* wait for the count flag */
	SysTick->CTRL = 0x00000004;				/* stop */
}

void Delay_ms(uint32_t xms)
{
	while (xms--)
	{
		Delay_us(1000);
	}
}

void Delay_s(uint32_t xs)
{
	while (xs--)
	{
		Delay_ms(1000);
	}
}
