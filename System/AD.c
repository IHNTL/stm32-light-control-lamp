#include "stm32f10x.h"
#include "AD.h"
#include "DMA.h"

static uint16_t s_adcBuffer[AD_BUFFER_SIZE];

/*
 * ADC1_IN0 on PA0, continuous conversion, results moved into s_adcBuffer by DMA.
 * PA0 is used because PA1 is taken by the PWM output.
 */
void AD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	/* 72 MHz / 6 = 12 MHz, must stay below 14 MHz */

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DMA_AdcInit(s_adcBuffer, AD_BUFFER_SIZE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/* Averaged result, less sensitive to single-sample noise. */
uint16_t AD_GetValue(void)
{
	uint32_t sum = 0;
	uint8_t i;

	for (i = 0; i < AD_BUFFER_SIZE; i++)
	{
		sum += s_adcBuffer[i];
	}
	return (uint16_t)(sum / AD_BUFFER_SIZE);
}
