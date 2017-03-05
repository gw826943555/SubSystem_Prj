#include "stm32f10x.h"
#include "usrbsp.h"
#include "delay.h"

void NVIC_CONFIG(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  													
}
void AFIO_CLK_En(void)
{
	
}
void BSP_CONFIG(void)
{
	NVIC_CONFIG();
	LED_CONFIG();
//	COM1.Init(Baudrate115200,DMA_Enable);
	TIM2_CONFIG();
	//CAN1_CONFIG();
	delay_init();
}
