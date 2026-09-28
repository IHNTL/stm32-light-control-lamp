#ifndef __CTLED_H
#define __CTLED_H

#include "stm32f10x.h"

#define CTLED_DUTY_MAX	100

void CtLED_Init(void);
void CtLED_SetTarget(uint16_t dutyPercent);
void CtLED_SetImmediate(uint16_t dutyPercent);
uint16_t CtLED_GetTarget(void);
uint16_t CtLED_GetCurrent(void);
void CtLED_Update(void);

#endif
