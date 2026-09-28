#include "stm32f10x.h"
#include "App.h"

/*
 * Light Control Lamp - STM32F103C8T6
 *
 * Foreground/background architecture:
 *   background - TIM3 tick, USART1 RX and ADC+DMA run on their own
 *   foreground - App_Process() handles every job without blocking
 */
int main(void)
{
	App_Init();

	while (1)
	{
		App_Process();
	}
}
