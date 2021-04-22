#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"
#include "stm32l1xx_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOUT_Pin GPIO_Pin_10
#define PD_SCK_Pin GPIO_Pin_11

GPIO_InitTypeDef GPIO_InitStructure;

void Delay(int val)
{
	__IO uint32_t i = val;
	for (; i != 0; i--)
	{
	}
}

void HX711_init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DOUT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = PD_SCK_Pin; // PD_SCK as a Output Pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;

	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//reset hx711
	GPIO_WriteBit(GPIOC, PD_SCK_Pin, Bit_SET);
	Delay(100000);
	GPIO_WriteBit(GPIOC, PD_SCK_Pin, Bit_RESET);
	Delay(1000);
}

int HX711_Read(void)
{
	unsigned long buffer = 0;

	while (GPIO_ReadInputDataBit(GPIOC, DOUT_Pin) == Bit_SET)
		;

	for (int i = 0; i < 24; i++)
	{
		GPIO_SetBits(GPIOC, PD_SCK_Pin);
		buffer = buffer << 1;
		GPIO_ResetBits(GPIOC, PD_SCK_Pin);
		if (GPIO_ReadInputDataBit(GPIOC, DOUT_Pin) == Bit_SET)
			buffer++;
	}
	GPIO_SetBits(GPIOC, PD_SCK_Pin);
	buffer = buffer ^ 0x800000;
	GPIO_ResetBits(GPIOC, PD_SCK_Pin);

	return buffer;
}

int HX711_GetOffset(void)
{

	int offset = 0;
	for (int i = 0; i < 10; i++)
	{
		offset += HX711_Read();
		Delay(1000);
	}

	return offset / 10;
}

int HX711_Tare(int *weightOffset)
{
	int weight = -1;
	*weightOffset = 420;
	int idx = 0;

	for (int i = 0; i <= 15; i++)
	{
		int temp = HX711_GetOffset();
		weight = (((4.555e-4) * temp) - 3478) - *weightOffset;
		if (i % 5 == 0)
		{
			*weightOffset += weight;
		}

		if (weight > -2 && weight < 2)
		{
			if (idx == 1)
				return 0;

			idx++;
		}
		else
		{
			idx = 0;
		}
	}

	return -1;
}

int HX711_GetWeight(int weightOffset)
{
	int temp = HX711_GetOffset();
	int weight = (((4.555e-3) * HX711_GetOffset()) - 34780) - weightOffset;
	return weight;
}
