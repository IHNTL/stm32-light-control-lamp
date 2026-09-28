#include "stm32f10x.h"
#include "LightSensor.h"
#include "AD.h"

/*
 * Calibration endpoints, in raw ADC counts.
 * LIGHT_ADC_DARK  : reading with the sensor fully covered
 * LIGHT_ADC_BRIGHT: reading under strong light
 *
 * To find them: open the serial assistant, cover the sensor and watch the
 * "adc=" field of the periodic report, then do the same under a lamp.
 */
#define LIGHT_ADC_DARK		3500
#define LIGHT_ADC_BRIGHT	200

void LightSensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	AD_Init();

	/* Digital output of the sensor module, kept for reference. */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint16_t LightSensor_GetRaw(void)
{
	return AD_GetValue();
}

uint8_t LightSensor_GetDigital(void)
{
	return (uint8_t)GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
}

/*
 * Map a raw ADC reading to a LED brightness in percent, 0..100.
 *
 * Darker surroundings must produce a brighter lamp, so the value is scaled
 * between the two calibration endpoints. The formula stays correct whether
 * LIGHT_ADC_DARK is above or below LIGHT_ADC_BRIGHT, because both the
 * numerator and the denominator change sign together.
 */
uint8_t LightSensor_ToDuty(uint16_t adc)
{
	int32_t denominator = (int32_t)LIGHT_ADC_DARK - (int32_t)LIGHT_ADC_BRIGHT;
	int32_t numerator;
	int32_t duty;

	if (denominator == 0)
	{
		return 0;
	}

	numerator = ((int32_t)adc - (int32_t)LIGHT_ADC_BRIGHT) * 100;
	duty = numerator / denominator;		/* truncates towards zero */

	if (duty < 0)
	{
		duty = 0;
	}
	if (duty > 100)
	{
		duty = 100;
	}

	return (uint8_t)duty;
}
