#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

#define SERIAL_RX_BUFFER_SIZE	64
#define SERIAL_LINE_SIZE		48

/* Ring buffer written by the interrupt, read by the main loop. */
static volatile uint8_t  s_rxBuffer[SERIAL_RX_BUFFER_SIZE];
static volatile uint16_t s_rxHead = 0;
static volatile uint16_t s_rxTail = 0;

static char    s_lineBuffer[SERIAL_LINE_SIZE + 1];
static uint8_t s_lineLength = 0;

/*
 * USART1 on PA9 (TX) and PA10 (RX), 115200 8-N-1.
 * Receive is interrupt driven so no byte is lost while the main loop is busy.
 */
void Serial_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

static void Serial_RxPush(uint8_t byte)
{
	uint16_t next = (uint16_t)((s_rxHead + 1) % SERIAL_RX_BUFFER_SIZE);

	if (next != s_rxTail)		/* silently drop the byte when full */
	{
		s_rxBuffer[s_rxHead] = byte;
		s_rxHead = next;
	}
}

static uint8_t Serial_RxPop(uint8_t *pByte)
{
	if (s_rxHead == s_rxTail)
	{
		return 0;
	}

	*pByte = s_rxBuffer[s_rxTail];
	s_rxTail = (uint16_t)((s_rxTail + 1) % SERIAL_RX_BUFFER_SIZE);
	return 1;
}

/*
 * Assemble one command line from the ring buffer.
 * Call it from the main loop. Returns 1 when a complete line is ready,
 * then Serial_GetLine() gives you the text. Lines end with '\n', a '\r' is ignored.
 */
uint8_t Serial_Poll(void)
{
	uint8_t byte;

	while (Serial_RxPop(&byte))
	{
		if (byte == '\r')
		{
			continue;
		}

		if (byte == '\n')
		{
			if (s_lineLength > 0)
			{
				s_lineBuffer[s_lineLength] = '\0';
				s_lineLength = 0;
				return 1;
			}
			continue;
		}

		if (s_lineLength < SERIAL_LINE_SIZE)
		{
			s_lineBuffer[s_lineLength] = (char)byte;
			s_lineLength++;
		}
		else
		{
			s_lineLength = 0;	/* line too long, drop it entirely */
		}
	}

	return 0;
}

char *Serial_GetLine(void)
{
	return s_lineBuffer;
}

void Serial_SendByte(uint8_t byte)
{
	USART_SendData(USART1, byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(const uint8_t *pArray, uint16_t length)
{
	uint16_t i;

	for (i = 0; i < length; i++)
	{
		Serial_SendByte(pArray[i]);
	}
}

void Serial_SendString(const char *pString)
{
	while (*pString != '\0')
	{
		Serial_SendByte((uint8_t)(*pString));
		pString++;
	}
}

void Serial_Printf(const char *pFormat, ...)
{
	char buffer[128];
	va_list arg;

	va_start(arg, pFormat);
	vsnprintf(buffer, sizeof(buffer), pFormat, arg);
	va_end(arg);

	Serial_SendString(buffer);
}

/* retarget printf to USART1 */
int fputc(int ch, FILE *f)
{
	Serial_SendByte((uint8_t)ch);
	return ch;
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_RxPush((uint8_t)USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	/* Overrun error: read DR to clear it, otherwise the interrupt keeps firing. */
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)
	{
		(void)USART_ReceiveData(USART1);
	}
}
