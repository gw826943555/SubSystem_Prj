/**
  ******************************************************************************
  * @file    CLed.h
  * @author  Chenxx
  * @version V1.0
  * @date    2015-09-05
  * @brief   This file the CLed class.
  ******************************************************************************/
#ifndef __CLED_HPP
#define __CLED_HPP

#include "stm32f10x.h"

#define RCC_APB2_BSP_LED	RCC_APB2Periph_GPIOA<< \
				(((uint32_t)BSP_LED_GPIO-(uint32_t)GPIOA)/0x400)
#define BSP_LED_GPIO			GPIOA

#define BSP_LED_START_BIT   (8-1) //0-1

#define BSP_LED1		GPIO_Pin_8
#define BSP_LED2		GPIO_Pin_8
#define BSP_LED3		GPIO_Pin_8
#define BSP_LED4		GPIO_Pin_8
#define BSP_LEDS		(BSP_LED1 | BSP_LED2 | BSP_LED3 | BSP_LED4)  

class CLed
{
	public:
		CLed();
	void On(u8 Led);
	void Off(u8 Led);
	void Toggle(u8 Led);
	private:
		uint8_t Is_On_Set;
		void Set(u8 Led);
		void Reset(u8 Led);
		void CLKConfig(void)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2_BSP_LED, ENABLE); 
		}
		void PinConfig(void)
		{
			GPIO_InitTypeDef GPIO_InitStructure;  

			GPIO_InitStructure.GPIO_Pin = BSP_LED1|BSP_LED2|BSP_LED3|BSP_LED4; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
			GPIO_Init(BSP_LED_GPIO, &GPIO_InitStructure);	
		}
	
};
extern CLed BspLed;

#endif
