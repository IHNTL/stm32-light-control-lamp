#include "stm32f10x.h"
#include "CtLED.h"
#include "PWM.h"

#define GRADIENT_STEP	2		/* percent per update, 10 ms per update */

static uint16_t s_targetDuty = 0;
static uint16_t s_currentDuty = 0;

void CtLED_Init(void)
{
	PWM_Init();

	s_targetDuty = 0;
	s_currentDuty = 0;
	PWM_SetDuty(0);
}

/* Set the target brightness in percent. The output ramps towards this value. */
void CtLED_SetTarget(uint16_t dutyPercent)
{
	if (dutyPercent > CTLED_DUTY_MAX)
	{
		dutyPercent = CTLED_DUTY_MAX;
	}
	s_targetDuty = dutyPercent;
}

/* Jump straight to the value. Used when the mode changes. */
void CtLED_SetImmediate(uint16_t dutyPercent)
{
	if (dutyPercent > CTLED_DUTY_MAX)
	{
		dutyPercent = CTLED_DUTY_MAX;
	}
	s_targetDuty = dutyPercent;
	s_currentDuty = dutyPercent;
	PWM_SetDuty(PWM_PERCENT_TO_DUTY(dutyPercent));
}

uint16_t CtLED_GetTarget(void)
{
	return s_targetDuty;
}

uint16_t CtLED_GetCurrent(void)
{
	return s_currentDuty;
}

/*
 * Non-blocking gradient handler. Call it every 10 ms from the main loop.
 * It returns immediately, so the serial port keeps running during a ramp.
 */
void CtLED_Update(void)
{
	if (s_currentDuty == s_targetDuty)
	{
		return;
	}

	if (s_currentDuty < s_targetDuty)
	{
		s_currentDuty += GRADIENT_STEP;
		if (s_currentDuty > s_targetDuty)
		{
			s_currentDuty = s_targetDuty;
		}
	}
	else
	{
		if (s_currentDuty >= GRADIENT_STEP)
		{
			s_currentDuty -= GRADIENT_STEP;
		}
		else
		{
			s_currentDuty = 0;
		}

		if (s_currentDuty < s_targetDuty)
		{
			s_currentDuty = s_targetDuty;
		}
	}

	PWM_SetDuty(PWM_PERCENT_TO_DUTY(s_currentDuty));
}
