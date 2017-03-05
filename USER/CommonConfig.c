#include "stm32f10x.h"
#include "CommonConfig.h"

void Contral_PAR_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_DeInit(GPIOA);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_9|GPIO_Pin_4|GPIO_Pin_5;  //控制继电器输出高电平24V
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_8;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_8);
}
                                                                                                 
void CAN_NvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
 
void CommonConfig(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	Contral_PAR_GPIO_Config();
	CAN_NvicConfig();

	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);     //电源输出默认开启
	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);   
	GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9);
}

