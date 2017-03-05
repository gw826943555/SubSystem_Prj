#include "stm32f10x.h"
#include "usrbsp.h"
void LED_CONFIG(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2_BSP_LED, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = BSP_LED; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(BSP_LED_GPIO, &GPIO_InitStructure);
}
/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns ON ALL the LEDs
*                       1    turns ON user LED1  on the board
*                       2    turns ON user LED2  on the board
*                       3    turns ON user LED3  on the board
*                       4    turns ON user LED3  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_LED_On(void)
{
	GPIO_SetBits(BSP_LED_GPIO,BSP_LED);
}
/*
*********************************************************************************************************
*                                              BSP_LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns OFF ALL the LEDs
*                       1    turns OFF user LED1  on the board
*                       2    turns OFF user LED2  on the board
*                       3    turns OFF user LED3  on the board
*                       4    turns OFF user LED3  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Off (void)
{
	GPIO_ResetBits(BSP_LED_GPIO,BSP_LED);
}


/*
*********************************************************************************************************
*                                            BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    TOGGLE ALL the LEDs
*                       1    TOGGLE user LED1  on the board
*                       2    TOGGLE user LED2  on the board
*                       3    TOGGLE user LED3  on the board
*                       4    TOGGLE user LED3  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (void)
{
	if(GPIO_ReadInputDataBit(BSP_LED_GPIO,BSP_LED)==SET)
		GPIO_ResetBits(BSP_LED_GPIO,BSP_LED);
	else
		GPIO_SetBits(BSP_LED_GPIO,BSP_LED);
}
