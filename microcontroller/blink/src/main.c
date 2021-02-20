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
#include <string.h>
			

int main(void)
{
	//*************************** Making two LEDs blink ***********************///

	/*RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

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
		//for(int i = 0; i <= 500000; i++);
		for(int i = 0; i <= 1000000; i++);
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);
		//for(int i = 0; i <= 500000; i++);
		for(int i = 0; i <= 1000000; i++);
	}*/

	//*************************************************************************///


	//**************************Setting up USART*******************//

	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	char send_message[20] = "ECE477 IS EASY\r";
	char receive_message[18] = "";

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //Enabling GPIOA ports
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //Enabling USART1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig (GPIOA, GPIO_PinSource9, GPIO_AF_USART1);   //Setting PA9 to Alternate function for USART
    GPIO_PinAFConfig (GPIOA, GPIO_PinSource10, GPIO_AF_USART1);


    //USART settings
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);     //Initialize UART settings

    USART_Cmd(USART1, ENABLE);    //Enable UART


    for (int i = 0; i < 18; ++i)
    {
    	USART_SendData(USART1,send_message[i]);
    	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);

//    	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
//    	receive_message[i] = USART_ReceiveData(USART1);

    }
    while(1)
    {

    }




    //*******************************************************************************************///















}
