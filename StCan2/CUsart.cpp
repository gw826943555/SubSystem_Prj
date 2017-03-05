#include "stm32f10x.h"
#include "CUsart.hpp"
#include "delay.h"
#include <string.h>
const uint32_t CUsart::BaudrateTable_[3] = {4800,9600,115200};

CUsart::CUsart(USART_TypeDef* USARTx)
{
	USARTx_ = USARTx;
}
void CUsart::Init(USART_BaudrateType Baudrate_Enum)
{
	Baudrate_ = BaudrateTable_[Baudrate_Enum];
	CLKConfig();
	PinConfig();
	UsartConfig();
}
void CUsart::Init(USART_BaudrateType Baudrate_Enum, 
									DMA_EnableType DMAStatus)
{
	Init(Baudrate_Enum);
	if(DMA_Enable == DMAStatus)
	{
		if(USARTx_ == USART1) DMA_USART_CHx_ = DMA1_Channel4;
		else if(USARTx_ == USART2) DMA_USART_CHx_ = DMA1_Channel7;
		else if(USARTx_ == USART2) DMA_USART_CHx_ = DMA1_Channel2;
		
		DMASendConfig();
	}
}
void CUsart::CLKConfig()
{
//			USART1	APB2;	GPIOA	APB2;
//			USART2	APB1;	GPIOA	APB2;
//			USART3	APB1;	GPIOB	APB2;
	if (USARTx_ == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if(USARTx_ == USART2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if(USARTx_ == USART3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	}
}

void CUsart::PinConfig()
{
	GPIO_InitTypeDef GPIO_InitStructure;  

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (USARTx_ == USART1)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
	}
	else if(USARTx_ == USART2)
	{}
	else if(USARTx_ == USART3)
	{}
}

void CUsart::UsartConfig()
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = Baudrate_; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USARTx_, &USART_InitStructure); 

	USART_Cmd(USARTx_, ENABLE); 
}

void CUsart::USART_Test(USART_TestType TestType)
{
	u8 temp;
	while(TestType == LoopBackTest)
	{
		while (USART_GetFlagStatus(USARTx_ , USART_FLAG_RXNE) == RESET);
		temp = USART1->DR;
		while (USART_GetFlagStatus(USARTx_, USART_FLAG_TXE) == RESET);
		USARTx_->DR = temp;
	}
	while(TestType == ContinueSend)
	{
		while (USART_GetFlagStatus(USARTx_, USART_FLAG_TXE) == RESET);
		USARTx_->DR = 0x55;
		delay_ms(500);
	}
}

//#define DMATEST
void CUsart::DMASendConfig()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	USART_DMACmd(USARTx_, USART_DMAReq_Tx, ENABLE);
	
	DMA_InitStructure_.DMA_PeripheralBaseAddr = (uint32_t)(&USARTx_->DR);
	DMA_InitStructure_.DMA_MemoryBaseAddr = (uint32_t)"a";
	DMA_InitStructure_.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure_.DMA_BufferSize = 1;
	DMA_InitStructure_.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure_.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure_.DMA_PeripheralDataSize =
	DMA_PeripheralDataSize_Byte;
	DMA_InitStructure_.DMA_MemoryDataSize =
	DMA_PeripheralDataSize_Byte;
	DMA_InitStructure_.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure_.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure_.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Cmd(DMA_USART_CHx_, DISABLE);//---
	DMA_Init(DMA_USART_CHx_, &DMA_InitStructure_);//----
	
}

void CUsart::DMASendStr(const char* str)
{
	DMA_InitStructure_.DMA_MemoryBaseAddr = (uint32_t)str;
	DMA_InitStructure_.DMA_BufferSize = strlen(str);
	
	DMA_Cmd(DMA_USART_CHx_, DISABLE);
	DMA_Init(DMA_USART_CHx_, &DMA_InitStructure_);
	
	DMA_Cmd(DMA_USART_CHx_, ENABLE);
}

void CUsart::Printf_USART1_Test(const char* str)
{
	DMA_InitStructure_.DMA_MemoryBaseAddr = (uint32_t)str;
	DMA_InitStructure_.DMA_BufferSize = strlen(str);
	
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_Init(DMA1_Channel4, &DMA_InitStructure_);
	
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

void CUsart::DMA_USART1_TEST(void)
{
	DMA_InitTypeDef DMA_InitStructure; 
	
	USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 
	
	DMA_Cmd(DMA1_Channel4, DISABLE); 
 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART1->DR); 
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)"hello"; 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 
	DMA_InitStructure.DMA_BufferSize = strlen("hello"); 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel4, ENABLE); 
}
CUsart COM1(USART1);

/*End of File*/
