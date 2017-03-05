#ifndef __CUSART_HPP
#define __CUSART_HPP

#include "stm32f10x.h"

typedef enum
{
	LoopBackTest = 0,
	ContinueSend = 1
}USART_TestType;
typedef enum
{
	DMA_Disable = 0,
	DMA_Enable = 1
}DMA_EnableType;
typedef enum
{
	Baudrate4800 = 0,
	Baudrate9600 = 1,
	Baudrate115200 = 2
}USART_BaudrateType;
class CUsart
{
	private:
		USART_TypeDef* USARTx_;
		DMA_Channel_TypeDef* DMA_USART_CHx_;
		DMA_InitTypeDef DMA_InitStructure_;
		static const uint32_t BaudrateTable_[3];
		uint32_t Baudrate_;
	
		void CLKConfig();
		void PinConfig();
		void UsartConfig();
		void DMASendConfig();
		void DMASendStr(const char* str);
	public:
			/*currently don't consider aoubt pin remap£¬*/
			/*only consider about which USART*/
		CUsart(USART_TypeDef* USARTx);
		void Init(USART_BaudrateType Baudrate_Enum);
		void Init(USART_BaudrateType Baudrate_Enum, 
							DMA_EnableType DMAStatus);
		void USART_Test(USART_TestType TestType);
		void printf(const char* str)
	{
		DMASendStr(str);
	}
	void Printf_USART1_Test(const char* str);
	void DMA_USART1_TEST(void);
};


extern CUsart COM1;

#endif
