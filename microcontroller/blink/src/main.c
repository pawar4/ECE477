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
#include "stm32l1xx_gpio.h"
#include <stdio.h>
#include <stdlib.h>
			

int main(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

    GPIO_InitTypeDef Init_Periph;
    Init_Periph.GPIO_Pin = GPIO_Pin_7
    
}
