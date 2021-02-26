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
#include "nmea.h"
#include <string.h>

int main(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	char receive_message[GPS_BUFFER_SIZE] = "";

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //Enabling GPIOA ports
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE); //Enabling USART1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	//USART settings
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);     //Initialize UART settings

	USART_Cmd(USART2, ENABLE);    //Enable UART
	int i = 0;
	char c = '\0';
	nmea_message * parsed_message = calloc(1, sizeof(nmea_message));
	parsed_message -> gll = calloc(1, sizeof(gll_message));
	parsed_message -> gsv = calloc(1, sizeof(gsv_message));
	parsed_message -> rmc = calloc(1, sizeof(rmc_message));
	//char * begin_sentence;
	while(1){
		while(c != '\n'){
			while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
			receive_message[i] = USART_ReceiveData(USART2);
			c = receive_message[i];
			i++;
		}
		//begin_sentence = strstr(receive_message, "$");
		parse_sentence(parsed_message, receive_message);
		printf("%s\n", receive_message);
		//free_nmea_message(parsed_message);
		memset(receive_message, '\0', 82*sizeof(char));
		c = '\0';
		i = 0;
	}
}
