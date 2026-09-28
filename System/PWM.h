#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

#define PWM_PERIOD			1000	/* 1 kHz, 1000 duty steps */
#define PWM_PERCENT_TO_DUTY(p)		((uint16_t)((p) * (PWM_PERIOD / 100)))

void PWM_Init(void);
void PWM_SetDuty(uint16_t duty);

#endif
