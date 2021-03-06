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

static int cnt = 0;
char received_message[200] = "";
char receive_success[30] = "Message successfully received\r";
volatile int UART1_received = 0;
volatile int UART1_sent = 1;

void UART1_init();
void USART1_IRQHandler();
void EXTI0_IRQHandler();
void receivesuccessmsg();
void pushbuttonmsg();

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

	UART1_init();
	pushbuttonmsg();
	while(1)
	{
		if(UART1_received == 1)
		{
			UART1_received = 0;
			receivesuccessmsg();
		}
	}
}



void UART1_init()
{
	//**************************Setting up USART*******************//
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //Enabling GPIOA ports
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //Enabling USART1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

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

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 // we want to configure the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff

    USART_Cmd(USART1, ENABLE);    //Enable UART


//    for (int i = 0; i < 16; ++i)
//    {
////    	USART_SendData(USART1,send_message[i]);
////    	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
//
//    	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
//    	received_message[i] = USART_ReceiveData(USART1);
//
//    }



}


void receivesuccessmsg()
{
	int i;
	for(i = 0; i < 30; ++i)
	{
		USART_SendData(USART1,receive_success[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
}

void pushbuttonmsg()
{
	// Function to set push button interrupt to send message to phone via bluetooth

	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //Enabling GPIOA ports
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    //setting the pin to logic low when button not pressed

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);  //telling system to use PA0 for EXTI_Line0

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;   //setting high priority but less priority than receiving
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler()
{
	// check if the USART1 receive interrupt flag was set
	  if( USART_GetITStatus(USART1, USART_IT_RXNE) ){

	    char temp = USART_ReceiveData(USART1);

	    if( (temp != '\n') && (cnt < 200) ){
	      received_message[cnt] = temp;
	      cnt++;
	    }
	    else{
	      UART1_received = 1;
	      cnt = 0;
	    }
	  }

	  // check if the USART1 transmit interrupt flag was set
	  if( USART_GetITStatus(USART1, USART_IT_TXE) ){


	  }

	  return;
}
void EXTI0_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		char send_message[] = "Hello\r\n";
		for(int i = 0; i < strlen(send_message); ++i)
		{
			USART_SendData(USART1,send_message[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}

}

