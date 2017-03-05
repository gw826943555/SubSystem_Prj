#include "stm32f10x.h"
#include <stdio.h>
#include "usrbsp.h"
#include "delay.h"
/**
  * @brief  Initializes the USART1 peripheral. Including CLOCK, GPIO
	*					UART and Enable the USART1.
  * @param  None
  * @retval None
  */

void USART1_CONFIG(void)
{
	USART_InitTypeDef USART_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate = 9600; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USART1, &USART_InitStructure); 
	
	USART_Cmd(USART1, ENABLE); 
}
/**
  * @brief  Test the base function of the USARTx
  * @param  USARTx: Select the USART or the UART peripheral. 
  *   			This parameter can be one of the following values:
  *   			USART1, USART2, USART3, UART4 or UART5.
  * @param  TestType: enum type that figure out which function
	*					you want to test.
	*					This parameter can be one of the following values:
  *   			LoopBackTest,ContinueSend.
  * @retval None
  */

void USART_TEST(USART_TypeDef* USARTx, USART_TestType TestType)
{
	u8 temp;
	while(TestType == LoopBackTest)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		temp = USART1->DR;
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART1->DR = temp;
	}
	while(TestType == ContinueSend)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART1->DR = 0x55;
		delay_ms(500);
	}
}

#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};
FILE __stdout;

void _sys_exit(int x)
{
	x = x;
	//return 0;
}
///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}
///重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}
