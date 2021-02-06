/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"
			

int main(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef Init_Periph;
	Init_Periph.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	Init_Periph.GPIO_Mode = GPIO_Mode_OUT;
	Init_Periph.GPIO_Speed = GPIO_Speed_2MHz;
	Init_Periph.GPIO_OType = GPIO_OType_PP;
	Init_Periph.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &Init_Periph);

	while(1) {
		GPIO_SetBits(GPIOB, GPIO_Pin_7);
		GPIO_SetBits(GPIOB, GPIO_Pin_6);
		for(int i = 0; i <= 500000; i++);
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);
		for(int i = 0; i <= 500000; i++);
	}
}
