/**
  ******************************************************************************
  * @file    bsp_TimBase.c
  * @author  Chenxx
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   TIM2 1ms 定时应用bsp
  ******************************************************************************

  */ 

#include "stm32f10x.h"
#include "usrbsp.h"
#include "ColorArray.hpp"

#define LCode  ((u8)30)
#define HCode  ((u8)60)
//#define NUM_LED 	1

//const u8 RGBArray[DMA_Size] = 
//{	
//	30,
//	LCode,HCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,HCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,HCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	0
//};
//const u8 RGBArray2[24*6+2] = 
//{	
//	30,
//	LCode,LCode,LCode,LCode,HCode,HCode,HCode,HCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,LCode,LCode,LCode,HCode,HCode,HCode,HCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,LCode,LCode,LCode,HCode,HCode,HCode,HCode,
//	
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,HCode,HCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,HCode,HCode,
//	
//	LCode,LCode,LCode,LCode,LCode,LCode,HCode,HCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,HCode,HCode,
//	
//	LCode,LCode,LCode,LCode,LCode,LCode,HCode,HCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,HCode,HCode,
//	LCode,LCode,LCode,LCode,LCode,LCode,LCode,LCode,
//	
//	0
//};

extern unsigned char DMA_bin_array[NUM_LED*COLOR_BASE * 8+2];

static void TIM2_BaseConfig(void);
static void TIM2_NVICConfig(void);
static void TIM2_PWMGpioConfig(void);
static void TIM2_PWMConfig(void);


/*
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * 中断周期为 = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 */
//inline void UsrDMACh2_En(void)
//{
//	GPIOA->CRL |=0x0008;
//	DMA1_Channel2->CCR |= DMA_CCR1_EN;
//}
void TIM2_CONFIG(void)
{
	TIM2_BaseConfig();
	TIM2_NVICConfig();
	TIM2_PWMGpioConfig();
	TIM2_PWMConfig();
	TIM2_DMAConfig();
	
	TIM_Cmd(TIM2, DISABLE);		/*先关闭等待使用*/    
}
static void TIM2_BaseConfig(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
			/* 设置TIM2CLK 为 72MHZ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    //TIM_DeInit(TIM2);
	
	/* 自动重装载寄存器周期的值(计数值) */
	TIM_TimeBaseStructure.TIM_Period=90;//----------
	
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
	  /* 时钟预分频数为72 */
	TIM_TimeBaseStructure.TIM_Prescaler= 0;// 72M--------------
	
		/* 对外部时钟进行采样的时钟分频,这里没有用到 */
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//---
}
static void TIM2_NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
		/* interrupt priority configuration */													
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
static void TIM2_PWMGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOA0 Set as AF_PP */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);	
 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
static void TIM2_PWMConfig(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	//TIM_OCInitStructure.TIM_Channel = TIM_Channel_1; 
	TIM_OCInitStructure.TIM_Pulse = 0 ; //------------
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM2, & TIM_OCInitStructure); 
}
void TIM2_DMAConfig(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	/* leaved not set */
	//while(1);
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1 , ENABLE);
	DMA_Cmd(DMA1_Channel2, DISABLE);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM2->CCR1;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_bin_array;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = DMA_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize =
	DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize =
	DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
	
	TIM_DMAConfig(TIM2, TIM_DMABase_CCR1, TIM_DMABurstLength_1Byte);
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
//	TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
	
	DMA_Cmd(DMA1_Channel2, ENABLE);
//	UsrDMACh2_En();
}
/**
  * @brief  Test the base function of the TIMx
  * @param  TIMx: Select the USART or the TIMER peripheral. 
  *   			This parameter can be one of the following values:
  *   			TIM2, TIM3, TIM4, TIM5 or TIM6.
  * @param  TestType: enum type that figure out which function
	*					you want to test.
	*					This parameter can be one of the following values:
  *   			nTimeUSARTSend,nTimeLEDToggle.
	* @param  CntNum: Times to count. When reached, call a function
  * @retval None
  */
void TIM_Test(TIM_TypeDef* TIMx, 
							TIM_TestType TestType,
							u32 CntNum)
{
	TIM_Cmd(TIMx, ENABLE);
	while(TestType == nTimeUSARTSend)
	{
	}
}


/*****************************END OF FILE******************************/
