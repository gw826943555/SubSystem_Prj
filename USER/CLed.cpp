/**
  ******************************************************************************
  * @file    CLed.cpp
  * @author  Chenxx
  * @version V1.0
  * @date    2015-09-05
  * @brief   This file provides all the LED firmware functions.
  ******************************************************************************/
#include "stm32f10x.h"
#include "CLed.h"

CLed::CLed(void):
	Is_On_Set(0)
{
	CLKConfig();
	PinConfig();
}
void CLed::Set(uint8_t led)
{
	switch (led)
	{
		case 0:
			 GPIO_SetBits(BSP_LED_GPIO, BSP_LEDS);
			 break;

		case 1:
			 GPIO_SetBits(BSP_LED_GPIO, BSP_LED1);
			 break;

		case 2:
			 GPIO_SetBits(BSP_LED_GPIO, BSP_LED2);
			 break;

		case 3:
			 GPIO_SetBits(BSP_LED_GPIO, BSP_LED3);
			 break;
		
		case 4:
			 GPIO_SetBits(BSP_LED_GPIO, BSP_LED4);
			 break;

		default:
			 break;
	}
}

void CLed::On(u8 led)
{
	if(Is_On_Set)
	{
		Set(led);
	}
	else
	{
		Reset(led);
	}
}

void CLed::Reset(u8 led)
{
	switch (led) {
		case 0:
				 GPIO_ResetBits(BSP_LED_GPIO, BSP_LEDS);
				 break;
		case 1:
				 GPIO_ResetBits(BSP_LED_GPIO, BSP_LED1);
				 break;
		case 2:
				 GPIO_ResetBits(BSP_LED_GPIO, BSP_LED2);
				 break;
		case 3:
				 GPIO_ResetBits(BSP_LED_GPIO, BSP_LED3);
				 break;
		case 4:
				 GPIO_ResetBits(BSP_LED_GPIO, BSP_LED4);
				 break;
		default:
				 break;
	}
}
void CLed::Off (u8 led)
{
	if(Is_On_Set)
	{
		Reset(led);
	}
	else
	{
		Set(led);
	}
}
void  CLed::Toggle (u8 led)
{
    uint16_t  pins;

    switch (led) {
			case 0:
					 pins =  GPIO_ReadOutputData(BSP_LED_GPIO);
					 pins ^= BSP_LEDS;
					 GPIO_SetBits(  BSP_LED_GPIO,   pins  & BSP_LEDS);
					 GPIO_ResetBits(BSP_LED_GPIO, (~pins) & BSP_LEDS);
					 break;

			case 1:
			case 2:
			case 3:
			case 4:
					pins = GPIO_ReadOutputData(BSP_LED_GPIO);
					if ((pins & (1 << (led + BSP_LED_START_BIT))) == 0) {
							 GPIO_SetBits(  BSP_LED_GPIO, (1 << (led + BSP_LED_START_BIT)));
					 } else {
							 GPIO_ResetBits(BSP_LED_GPIO, (1 << (led + BSP_LED_START_BIT)));
					 }
					break;

			default:
					 break;
    }
}
CLed BspLed;

/*End of File*/


