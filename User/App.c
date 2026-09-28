#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include "App.h"
#include "CtLED.h"
#include "LED.h"
#include "LightSensor.h"
#include "OLED.h"
#include "Serial.h"
#include "Timer.h"

#define APP_PC_TIMEOUT_MS		3000	/* PC mode watchdog */
#define APP_GRADIENT_PERIOD_MS	10
#define APP_REPORT_PERIOD_MS	200
#define APP_DISPLAY_PERIOD_MS	500
#define APP_LED_PERIOD_MS		500

static uint8_t  s_mode = APP_MODE_AUTO;
static uint16_t s_adcValue = 0;
static uint32_t s_lastCommandTick = 0;

static uint32_t s_lastGradient = 0;
static uint32_t s_lastReport = 0;
static uint32_t s_lastDisplay = 0;
static uint32_t s_lastLed = 0;

static void App_UpdateDisplay(void);
static void App_HandleCommand(char *pLine);

void App_Init(void)
{
	OLED_Init();
	LED_Init();
	CtLED_Init();
	LightSensor_Init();
	Serial_Init();
	Timer_Init();

	s_mode = APP_MODE_AUTO;
	s_lastCommandTick = Timer_GetTick();

	Serial_SendString("\r\nLight Control Lamp ready\r\n");
	Serial_SendString("Type HELP for the command list\r\n");
	Serial_SendString("MODE,AUTO\r\n");
}

uint8_t App_GetMode(void)
{
	return s_mode;
}

uint16_t App_GetAdc(void)
{
	return s_adcValue;
}

static void App_ToUpperCase(char *pText)
{
	while (*pText != '\0')
	{
		if ((*pText >= 'a') && (*pText <= 'z'))
		{
			*pText = (char)(*pText - 'a' + 'A');
		}
		pText++;
	}
}

static void App_SetMode(uint8_t mode)
{
	if (mode == s_mode)
	{
		return;
	}

	s_mode = mode;

	if (mode == APP_MODE_AUTO)
	{
		Serial_SendString("MODE,AUTO\r\n");
	}
	else
	{
		/* Hold the current brightness, the PC takes over from here. */
		CtLED_SetImmediate(CtLED_GetCurrent());
		Serial_SendString("MODE,PC\r\n");
	}
}

static void App_SendStatus(void)
{
	Serial_Printf("DATA adc=%u duty=%u target=%u mode=%s\r\n",
		(unsigned int)s_adcValue,
		(unsigned int)CtLED_GetCurrent(),
		(unsigned int)CtLED_GetTarget(),
		(s_mode == APP_MODE_AUTO) ? "AUTO" : "PC");
}

static void App_UpdateDisplay(void)
{
	OLED_ShowString(1, 1, "LIGHT CTL");

	OLED_ShowString(2, 1, "ADC:");
	OLED_ShowNum(2, 5, s_adcValue, 4);

	OLED_ShowString(3, 1, "DUTY:");
	OLED_ShowNum(3, 6, CtLED_GetCurrent(), 3);
	OLED_ShowChar(3, 9, '%');

	OLED_ShowString(4, 1, "MODE:");
	OLED_ShowString(4, 6, (s_mode == APP_MODE_AUTO) ? "AUTO " : "PC   ");
}

static void App_HandleCommand(char *pLine)
{
	char *pCommand;
	char *pArgument;

	App_ToUpperCase(pLine);

	pCommand = strtok(pLine, " ,");
	if (pCommand == NULL)
	{
		return;
	}
	pArgument = strtok(NULL, " ,");

	s_lastCommandTick = Timer_GetTick();

	if (strcmp(pCommand, "LIGHT") == 0)
	{
		int value;

		if (pArgument == NULL)
		{
			Serial_SendString("ERR,LIGHT needs a value 0-100\r\n");
			return;
		}

		value = atoi(pArgument);
		if (value < 0)
		{
			value = 0;
		}
		if (value > 100)
		{
			value = 100;
		}

		App_SetMode(APP_MODE_PC);
		CtLED_SetTarget((uint16_t)value);
		Serial_Printf("OK,LIGHT=%d\r\n", value);
	}
	else if (strcmp(pCommand, "MODE") == 0)
	{
		if (pArgument == NULL)
		{
			Serial_SendString("ERR,MODE needs AUTO or PC\r\n");
			return;
		}

		if (strcmp(pArgument, "AUTO") == 0)
		{
			App_SetMode(APP_MODE_AUTO);
			Serial_SendString("OK,MODE=AUTO\r\n");
		}
		else if (strcmp(pArgument, "PC") == 0)
		{
			App_SetMode(APP_MODE_PC);
			Serial_SendString("OK,MODE=PC\r\n");
		}
		else
		{
			Serial_SendString("ERR,MODE needs AUTO or PC\r\n");
		}
	}
	else if (strcmp(pCommand, "QUERY") == 0)
	{
		App_SendStatus();
	}
	else if (strcmp(pCommand, "HELP") == 0)
	{
		Serial_SendString("LIGHT <0-100>  set brightness, switches to PC mode\r\n");
		Serial_SendString("MODE AUTO|PC   select the working mode\r\n");
		Serial_SendString("QUERY          report the current status\r\n");
		Serial_SendString("HELP           show this list\r\n");
	}
	else
	{
		Serial_Printf("ERR,unknown command %s\r\n", pCommand);
	}
}

/*
 * Main application task. Call it as fast as possible from the while(1) loop.
 * Every job here is non-blocking, so no task can stall the others.
 */
void App_Process(void)
{
	/* 1. serial commands have the highest priority */
	if (Serial_Poll())
	{
		App_HandleCommand(Serial_GetLine());
	}

	/* 2. read the latest ADC result, DMA keeps the buffer fresh in the background */
	s_adcValue = LightSensor_GetRaw();

	/* 3. mode logic */
	if (s_mode == APP_MODE_AUTO)
	{
		CtLED_SetTarget(LightSensor_ToDuty(s_adcValue));
	}
	else if ((Timer_GetTick() - s_lastCommandTick) > APP_PC_TIMEOUT_MS)
	{
		/* watchdog: the PC stopped talking, fall back to automatic control */
		App_SetMode(APP_MODE_AUTO);
		Serial_SendString("EVT,pc timeout\r\n");
	}

	/* 4. LED gradient, every 10 ms */
	if (Timer_IsElapsed(&s_lastGradient, APP_GRADIENT_PERIOD_MS))
	{
		CtLED_Update();
	}

	/* 5. periodic status report */
	if (Timer_IsElapsed(&s_lastReport, APP_REPORT_PERIOD_MS))
	{
		App_SendStatus();
	}

	/* 6. OLED refresh, every 500 ms */
	if (Timer_IsElapsed(&s_lastDisplay, APP_DISPLAY_PERIOD_MS))
	{
		App_UpdateDisplay();
	}

	/* 7. heartbeat */
	if (Timer_IsElapsed(&s_lastLed, APP_LED_PERIOD_MS))
	{
		LED_Toggle();
	}
}
