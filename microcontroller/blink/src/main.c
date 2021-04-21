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
#include "nmea.h"
#include "SparkFunBQ27441.h"
#include "stm32l1xx_i2c.h"

static int cnt1 = 0;
static int cnt3 = 0;
static int ble_cnt = 0;
static int ph_index = 0;
int state1 = 0;
int state0 = 0;
int state2 = 0;
int valid1 = 0;
int valid3 = 0;
int test_cmd = 0;
static int sms_cnt = 0;
char delimiter[2] = "\r\n";
char received_message1[200] = "";
char received_message3[200] = "";
char phone_number[15] = "";
char phone_index[5] = "";
char sms_request[100] = "";
char weight[50] = "20.5 Kg\r\n";
int* weightOffset = 420;
char battery[10];
I2C_TypeDef * I2CPERIPHSEL;


uint8_t ctrl_z = 0x1A;                              // for sending ctrl+z

char receive_success[30] = "Message successfully received\r";

volatile int UART1_received = 0;
volatile int UART3_received = 0;
volatile int ble = 0;

void GSM_default();
void GSM_powersave();
void save_phone_number();
void UART1_init();
void UART3_init();
void USART1_IRQHandler();
void USART3_IRQHandler();
void EXTI0_IRQHandler();
void receivesuccessmsg();
void pushbuttonmsg();
void setup_ring_indicator_pin();
void sms_sendweight();
void sms_sendlocation(char * msg);
void sms_sendbattery();
void bluetooth_sendweight();
void bluetooth_sendlocation(char * msg);
void bluetooth_sendbattery();
void FG_Config(void);

int main(void)
{
	UART1_init();
	UART3_init();
	GSM_default();
	FG_Config();
	pushbuttonmsg();
	initParsedMessage();
	initUART2();
	initTimer2();
	//GSM_powersave();
	while(1)
	{
		if((UART1_received == 1))
		{
			if(received_message1[0] == 'W') //send weight info via bluetooth
			{
				bluetooth_sendweight();
			}
			else if (received_message1[0] == 'T')
			{
				bluetooth_tareweight();
			}
			else if(received_message1[0] == 'B')   //send battery info via bluetooth
			{
				bluetooth_sendbattery();
			}
			else if(received_message1[0] == 'L') //send location info via bluetooth
			{
				bluetooth_sendlocation(createGPSmsg());
			}
			UART1_received = 0;
//			valid1 = 0;
		}

		if((UART3_received == 1))
		{
			if(sms_request[0] == 'W')  //send weight info via sms
			{
				sms_sendweight();
			}
			else if(sms_request[0] == 'B') //send battery info via sms
			{
				sms_sendbattery();
			}
			else if(sms_request[0] == 'L') //send GPS location info via sms
			{
				sms_sendlocation(createGPSmsg());
			}
			UART3_received = 0;
//			valid3 = 0;

		}
	}
}

void GSM_default()
{
	char send_message1[] = "AT+UPSV=0\r\n";
	char send_message2[] = "AT+CNMI=2,2\r\n";
	for(int i = 0; i < strlen(send_message1); ++i)
	{
		USART_SendData(USART3,send_message1[i]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
	}

	for(int i= 0; i < 10000; ++i);     //when using while(UART3_received != 1) it doesnt work

	for(int i = 0; i < strlen(send_message2); ++i)
	{
		USART_SendData(USART3,send_message2[i]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
	}
}

void GSM_powersave()
{
	char send_message1[] = "AT+UPSV=1\r\n";

	for(int i = 0; i < strlen(send_message1); ++i)
	{
		USART_SendData(USART3,send_message1[i]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
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

	    if(ble == 1)
	    {
	    	if(ble_cnt < 12)
	    	{
	    		phone_number[ble_cnt] = temp;
	    		received_message1[cnt1] = temp;
	    		UART1_received = 0;
	    	}

    		ble_cnt += 1;
    		cnt1 += 1;

	    	if(temp == '\n')
	    	{
	    		UART1_received = 1;
	    		cnt1 = 0;
	    		valid1 = 1;
	    		ble = 0;
	    		ble_cnt = 0;
	    	}

	    }

	    else
	    {
			if((temp == '\n')){
			  UART1_received = 1;
			  cnt1 = 0;
			  valid1 = 1;
			}
			else{
			  if(cnt1 < 200)
			  {
				  received_message1[cnt1] = temp;
				  cnt1++;
			  }
			}
		}

	    if(temp == 'N')
	    {
	    	ble = 1;
	    }
	  }


	  return;
}

void EXTI0_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
//		char send_message1[] = "AT+CMGL=\"ALL\"\r\n";
//		char send_message[] = "AT+CMGF=1\r\n";
//		char send_message1[] = "AT+CMGD=3\r\n";
//		char send_message1[] = "AT+CNMI=2,2\r\n";
//		char send_message1[] = "AT+UPSV=1\r\n";
//		char send_message1[] = "AT+CMGL=\"REC READ\"\r\n";
//		char send_message1[] = "AT+CMGS=\"+17657018549\"\r";
//		char send_message2[] = "Hello";
//		char send_message1[] = "AT&V\r";
//		for(int i = 0; i < strlen(send_message1); ++i)
//		{
//			USART_SendData(USART3,send_message1[i]);
//			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
//		}

//		for(int i= 0; i < 4000; ++i);     //when using while(UART3_received != 1) it doesnt work
//
//		for(int i = 0; i < strlen(send_message2); ++i)
//		{
//			USART_SendData(USART3,send_message2[i]);
//			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
//		}
//		USART_SendData(USART3,ctrl_z);
//		char * msg = createGPSmsg();
//		bluetooth_sendlocation(msg);
// 		bluetooth_sendbattery();
		//sms_sendweight();
		bluetooth_sendweight();
		//save_phone_number();

		EXTI_ClearITPendingBit(EXTI_Line0);
	}

}

//Function to initialize UART Channel 3 for GSM
void UART3_init()
{
	//**************************Setting up USART*******************//
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); //Enabling GPIOB ports
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //Enabling USART3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig (GPIOB, GPIO_PinSource10, GPIO_AF_USART3);   //Setting PB10 to Alternate function for USART
    GPIO_PinAFConfig (GPIOB, GPIO_PinSource11, GPIO_AF_USART3);


    //USART settings
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);     //Initialize UART settings

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		 // we want to configure the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;// this sets the priority group of the USART3 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;		 // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff

    USART_Cmd(USART3, ENABLE);    //Enable UART
}


void save_phone_number()
{
	char message1[40] = "AT+CMGW=\"091137880\"\r";

	for(int i = 0; message1[i] != '\0'; ++i)
	{
		USART_SendData(USART3,message1[i]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
	}

	for(int i = 0; i < 5000; ++i);

	for(int i = 0; phone_number[i] != '\0'; ++i)
	{
		USART_SendData(USART3,phone_number[i]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
	}
	USART_SendData(USART3,ctrl_z);
}
//Function called when UART channel 3 receive register contains unread values for GSM
void USART3_IRQHandler()
{
    if(test_cmd == 0)     //We know that the only AT command we receiving without test is for SMS notification
    {
        // check if the USART3 receive interrupt flag was set
          if( USART_GetITStatus(USART3, USART_IT_RXNE) ){

            char temp = USART_ReceiveData(USART3);

            if((temp == 'T') && (received_message3[cnt3-1] == 'M') && (received_message3[cnt3-2] == 'C'))  //denotes CMT message
            {
                state0 = 1;
                cnt3 = 0;
                sms_cnt = 0;
            }

            if(state0 == 2){        //finished reading
                cnt3 = 0;
                UART3_received = 1;
                sms_cnt = 0;
                valid3 = 1;
                state0 = 0;
            }
            else
            {
                received_message3[cnt3] = temp;
                cnt3++;

                if((state0 == 1) && ((temp == 'W') || (temp == 'B') || (temp == 'L')))
                {
                    sms_request[sms_cnt] = temp;
                    sms_cnt = 0;
                    state0 = 2;
                }
            }
          }
    }

    else  //test_cmd = 1
    {
            sms_cnt = 0;
          if( USART_GetITStatus(USART3, USART_IT_RXNE) ){
            char temp = USART_ReceiveData(USART3);

            if(temp == 'K' && received_message3[cnt3-1] == 'O')
            {
                received_message3[cnt3] = temp;
                state1 = 1;
            }
            else if(state1 == 1 && temp == '\n'){
                cnt3 = 0;
                UART3_received = 1;
                state1 = 0;
                valid3 = 1;
            }
            else if(state1 == 0 && cnt3 < 200)
            {
                received_message3[cnt3] = temp;
                cnt3++;
            }
          }
    }

    return;
}

void FG_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	//GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C2);

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

void setup_ring_indicator_pin()
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); //Enabling GPIOC ports
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;    //setting the pin to logic low when button not pressed

    GPIO_Init(GPIOC, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);  //telling system to use PC4 for EXTI_Line4

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

}


void sms_sendweight()
{
    //char send_message1[] = "AT+CMGS=\"+18455311048\"\r";
	char send_message1[] = "AT+CMGS=\"";
    char send_message2[] = "\"\r";
    char category[2] = "W,";
    if(phone_number[0] != '\0')
    {
		for(int i = 0; i < strlen(send_message1); ++i)
		{
			USART_SendData(USART3,send_message1[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; phone_number[i] != '\0'; ++i)
		{
			USART_SendData(USART3,phone_number[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; i < strlen(send_message2); ++i)
		{
			USART_SendData(USART3,send_message2[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i= 0; i < 5000; ++i);     //when using while(UART3_received != 1) it doesnt work

		for(int i=0; i < 2; ++i)
		{
			USART_SendData(USART3,category[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; weight[i] != '\0'; ++i)
		{
			USART_SendData(USART3,weight[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}
		USART_SendData(USART3,ctrl_z);
    }
}

void sms_sendlocation(char * msg)
{
	char send_message1[] = "AT+CMGS=\"";
    char send_message2[] = "\"\r";
    char category[2] = "L,";

    if(phone_number[0] != '\0')
    {
		for(int i = 0; i < strlen(send_message1); ++i)
		{
			USART_SendData(USART3,send_message1[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; phone_number[i] != '\0'; ++i)
		{
			USART_SendData(USART3,phone_number[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; i < strlen(send_message2); ++i)
		{
			USART_SendData(USART3,send_message2[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i= 0; i < 5000; ++i);     //when using while(UART3_received != 1) it doesnt work

		for(int i=0; i < 2; ++i)
		{
			USART_SendData(USART3,category[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; msg[i] != '\0'; ++i)
		{
			USART_SendData(USART3,msg[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}
		USART_SendData(USART3,ctrl_z);
    }
}

void sms_sendbattery()
{
	char send_message1[] = "AT+CMGS=\"";
    char send_message2[] = "\"\r";
    char category[2] = "B,";

	uint16_t sob = soc(FILTERED);
	itoa (sob, battery, 10);

	if(phone_number[0] != '\0')
	{
		for(int i = 0; i < strlen(send_message1); ++i)
		{
			USART_SendData(USART3,send_message1[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; phone_number[i] != '\0'; ++i)
		{
			USART_SendData(USART3,phone_number[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; i < strlen(send_message2); ++i)
		{
			USART_SendData(USART3,send_message2[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i= 0; i < 5000; ++i);     //when using while(UART3_received != 1) it doesnt work

		for(int i=0; i < 2; ++i)
		{
			USART_SendData(USART3,category[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}

		for(int i = 0; battery[i] != '\0'; ++i)
		{
			USART_SendData(USART3,battery[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
		}
		USART_SendData(USART3,ctrl_z);
	}
}

void bluetooth_sendweight()
{
	char category[2] = "W,";
	int temp = HX711_GetWeight(*weightOffset);
	itoa(temp, weight, 10);
	for (int i = 0; i < 2; ++i)
	{
		USART_SendData(USART1, category[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
	for (int i = 0; weight[i] != '\0'; ++i)
	{
		USART_SendData(USART1, weight[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
	for (int i = 0; i < strlen(delimiter); ++i)
	{
		USART_SendData(USART1, delimiter[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

void bluetooth_tareweight()
{
	char category[2] = "W,";
	int temp = HX711_Tare(weightOffset);
	itoa(temp, weight, 10);
	for (int i = 0; i < 2; ++i)
	{
		USART_SendData(USART1, category[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
	for (int i = 0; weight[i] != '\0'; ++i)
	{
		USART_SendData(USART1, weight[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
	for (int i = 0; i < strlen(delimiter); ++i)
	{
		USART_SendData(USART1, delimiter[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

void bluetooth_sendbattery()
{
	uint16_t sob = soc(FILTERED);
	itoa (sob, battery, 10);
	char category[2] = "B,";

	for(int i = 0; i < 2; ++i)
	{
		USART_SendData(USART1,category[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
	for(int i = 0; battery[i] != '\0'; ++i)
	{
		USART_SendData(USART1,battery[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
	for(int i=0; i < strlen(delimiter); ++i)
	{
		USART_SendData(USART1,delimiter[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
}

void bluetooth_sendlocation(char * msg)
{
	char category[2] = "L,";
	for(int i = 0; i < 2; ++i)
	{
		USART_SendData(USART1,category[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
	for(int i = 0; msg[i] != '\0'; ++i)
	{
		USART_SendData(USART1,msg[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
	for(int i=0; i < strlen(delimiter); ++i)
	{
		USART_SendData(USART1,delimiter[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
}
