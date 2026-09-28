#include "stm32f10x.h"

static volatile uint32_t s_tickMs = 0;

/*
 * TIM3 provides the 1 ms system tick.
 * TIM2 is reserved for the PWM output on PA1, so it must not be used here.
 */
void Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;		/* 1 ms */
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;		/* 72 MHz / 72 = 1 MHz */
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

	TIM_ClearFlag(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE);
}

uint32_t Timer_GetTick(void)
{
	return s_tickMs;
}

/*
 * Non-blocking periodic timer helper.
 * Returns 1 when the interval has elapsed, and moves the reference point forward.
 * Wrap-around safe because unsigned subtraction is used.
 */
uint8_t Timer_IsElapsed(uint32_t *pLast, uint32_t interval)
{
	uint32_t now = s_tickMs;

	if ((now - *pLast) >= interval)
	{
		*pLast = now;
		return 1;
	}
	return 0;
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		s_tickMs++;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
