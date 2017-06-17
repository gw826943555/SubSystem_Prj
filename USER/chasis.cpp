#include "chasis.h"
#include "ccan.h"

#define CAN_LEFT_DRIVER_BACK 					0x01
#define CAN_LEFT_DRIVER 									0x02
#define CAN_RIGHT_DRIVER_BACK 				0x03
#define CAN_RIGHT_DRIVER 								0x04

#define HALL_LA																GPIO_Pin_2
#define HALL_LB																GPIO_Pin_3
#define HALL_LC																GPIO_Pin_4
#define HALL_RA															GPIO_Pin_10
#define HALL_RB															GPIO_Pin_11
#define HALL_RC															GPIO_Pin_12
#define HALL_PORT														GPIOA

#define DIR_LCTRL														GPIO_Pin_2
#define DIR_RCTRL														GPIO_Pin_3
#define DIR_CTRL_PORT											GPIOA

#define SPEED_LCTRL													GPIO_Pin_6
#define SPEED_RCTRL												GPIO_Pin_8
#define SPEED_CTRL_PORT									GPIOB

namespace{
	const uint8_t MaxBufSize=8;
	CanRxMsg RCanRxBuf[MaxBufSize];
	CCanRxMailbox _RcanRxMailBox(RCanRxBuf,MaxBufSize);
	
	CanRxMsg LCanRxBuf[MaxBufSize];
	CCanRxMailbox _LcanRxMailBox(LCanRxBuf,MaxBufSize);
}

void chasis_init_ctrl()
{
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  GPIO_InitStructure.GPIO_Pin = DIR_LCTRL | DIR_RCTRL; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DIR_CTRL_PORT, &GPIO_InitStructure);    
}

void chasis_hall_irq_config(FunctionalState state)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = state;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_Init(&EXTI_InitStructure);
}

void chasis_init_hall()
{
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  GPIO_InitStructure.GPIO_Pin =  HALL_LA | HALL_LB | HALL_LC | HALL_RA | HALL_RB | HALL_RC; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(HALL_PORT, &GPIO_InitStructure);    
}

void chasis_init_pwm()
{
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  GPIO_InitStructure.GPIO_Pin = SPEED_LCTRL | SPEED_RCTRL; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPEED_CTRL_PORT, &GPIO_InitStructure);    
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Time base configuration */		 
	TIM_TimeBaseStructure.TIM_Period = 999;      
	TIM_TimeBaseStructure.TIM_Prescaler =35;	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;	   
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM4,&TIM_OCInitStructure); 
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);	
	
	TIM_ARRPreloadConfig(TIM4,ENABLE);		
	TIM_Cmd(TIM4,ENABLE); 
	
	DoStatus.is_protected.DO_DataBit.Do04 = 1;
	DoStatus.is_protected.DO_DataBit.Do06 = 1;
}

void chasis_do_init()
{
	chasis_init_pwm();
	chasis_init_ctrl();
	chasis_init_hall();
	chasis_hall_irq_config(ENABLE);
	
	_RcanRxMailBox.setStdId(CAN_RIGHT_DRIVER);
	_RcanRxMailBox.attachToRouter(CanRouter1);
	
	_LcanRxMailBox.setStdId(CAN_LEFT_DRIVER);
	_LcanRxMailBox.attachToRouter(CanRouter1);
}

void chasis_do_run()
{
	if(_RcanRxMailBox.msgsInQue())
	{
		
	}
	
	if(_LcanRxMailBox.msgsInQue())
	{
		
	}
}


