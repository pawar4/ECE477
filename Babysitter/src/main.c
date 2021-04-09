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
#include "stm32l1xx_usart.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SparkFunBQ27441.h"
			
#define FG_ADDRESS 0xAA
I2C_TypeDef * I2CPERIPHSEL;
int16_t value;
int16_t value2;
int16_t value3;
int8_t value4;

void FG_Config(void);
//TIM6_Config(void);
//void I2C_start(I2C_TypeDef *, uint8_t, uint8_t);
//void I2C_write(I2C_TypeDef *, uint8_t);
//void I2C_stop(I2C_TypeDef *);
//int8_t I2C_read_ack(I2C_TypeDef *);
//int8_t I2C_read_nack(I2C_TypeDef *);

GPIO_InitTypeDef GPIO_InitStructure;
I2C_InitTypeDef I2C_InitStructure;

void FG_Config(void)
{


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	//GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2, &I2C_InitStructure);
	I2C_StretchClockCmd(I2C2, ENABLE);
	I2CPERIPHSEL = I2C2;
	I2C_Cmd(I2C2, ENABLE);
}

//void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
//{
// while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
// I2C_GenerateSTART(I2Cx, ENABLE);
// while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
// I2C_Send7bitAddress(I2Cx, address, direction);
// if (direction== I2C_Direction_Transmitter) {
// while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
// }
// else if(direction == I2C_Direction_Receiver) {
// while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
// }
//}
//
//void I2C_write(I2C_TypeDef* I2Cx, uint8_t data)
//{
// I2C_SendData(I2Cx, data);
// while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//}
//
//void I2C_stop(I2C_TypeDef* I2Cx)
//{
// I2C_GenerateSTOP(I2Cx, ENABLE);
//}
//
//int8_t I2C_read_ack(I2C_TypeDef* I2Cx)
//{
// int8_t data;
//
// I2C_AcknowledgeConfig(I2Cx, ENABLE);
// while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
// data=I2C_ReceiveData(I2Cx);
//
// return data;
//}
//
//int8_t I2C_read_nack(I2C_TypeDef* I2Cx)
//{
// uint8_t data;
//
// I2C_AcknowledgeConfig(I2Cx, DISABLE);
// while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
// data=I2C_ReceiveData(I2Cx);
// I2C_GenerateSTOP(I2Cx, ENABLE);
//
// return data;
//}

int main(void)
{

	FG_Config();
//	/* READ CONTROL COMMANDS
//	// UNseal
//	/*
//	I2C_start(I2C2, FG_ADDRESS, I2C_Direction_Transmitter);
//	//
//	I2C_write(I2C2,0x00);
//	I2C_write(I2C2,0x00);
//	I2C_write(I2C2,0x80);
//	I2C_stop(I2C2);
//	I2C_start(I2C2, FG_ADDRESS, I2C_Direction_Transmitter);
//	//
//	I2C_write(I2C2,0x00);
//	I2C_write(I2C2,0x00);
//	I2C_write(I2C2,0x80);
//	I2C_stop(I2C2);

//	I2C_start(I2CPERIPHSEL, FG_ADDRESS, I2C_Direction_Transmitter);
//	I2C_write(I2CPERIPHSEL,0x00);
//	I2C_write(I2CPERIPHSEL,0x02);
//	I2C_write(I2CPERIPHSEL,0x00);
//	I2C_stop(I2CPERIPHSEL);
//	I2C_start(I2CPERIPHSEL, FG_ADDRESS, I2C_Direction_Transmitter);
//	I2C_write(I2CPERIPHSEL, 0x00);
//	I2C_GenerateSTART(I2CPERIPHSEL, ENABLE);
//	while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_MODE_SELECT)) ;
//	I2C_Send7bitAddress(I2CPERIPHSEL, FG_ADDRESS, I2C_Direction_Receiver);
//	while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
//	value = I2C_read_ack(I2CPERIPHSEL);
//	value2 = I2C_read_nack(I2CPERIPHSEL);

////
//	I2C_start(I2C2, FG_ADDRESS, I2C_Direction_Transmitter);
//	I2C_write(I2C2,0x1C);
//	I2C_GenerateSTART(I2C2, ENABLE);
//	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) ;
//	I2C_Send7bitAddress(I2C2, FG_ADDRESS, I2C_Direction_Receiver);
//	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
//	value3 = I2C_read_ack(I2C2);
//	value4 = I2C_read_nack(I2C2);
//
	//I2C_start(I2CPERIPH, FG_ADDRESS, I2C_Direction_Transmitter);
//	I2C_write(I2C2,0x0E);
//	I2C_GenerateSTART(I2C2, ENABLE);
//	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) ;
//	I2C_Send7bitAddress(I2C2, FG_ADDRESS, I2C_Direction_Receiver);
//	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
//	value = I2C_read_ack(I2C2);
//	value2 = I2C_read_nack(I2C2);
	value = deviceType();
	value2 = readControlWord(BQ27441_CONTROL_FW_VERSION);
	value3 = soh(PERCENT);
	while(1);

}
