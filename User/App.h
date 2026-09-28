#ifndef __APP_H
#define __APP_H

#include "stm32f10x.h"

#define APP_MODE_AUTO	0
#define APP_MODE_PC		1

void App_Init(void);
void App_Process(void);

uint8_t App_GetMode(void);
uint16_t App_GetAdc(void);

#endif
