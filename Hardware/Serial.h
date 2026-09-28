#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

void Serial_Init(void);

/* Call from the main loop. Returns 1 when a complete command line is ready. */
uint8_t Serial_Poll(void);
char *Serial_GetLine(void);

void Serial_SendByte(uint8_t byte);
void Serial_SendArray(const uint8_t *pArray, uint16_t length);
void Serial_SendString(const char *pString);
void Serial_Printf(const char *pFormat, ...);

#endif
