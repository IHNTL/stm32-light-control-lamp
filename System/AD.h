#ifndef __AD_H
#define __AD_H

#include "stm32f10x.h"

#define AD_BUFFER_SIZE		8
#define AD_MAX_VALUE		4095

void AD_Init(void);
uint16_t AD_GetValue(void);

#endif
