#include "CPWM.h"
u16 PWM_L = 0;
u16 PWM_R = 0;

static void TIM4_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_8; //PWM���  ͨ��1��3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // ���ÿ�©
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * ��������SPEED_GPIO_Config
 * ����  �������ٶȿ�����Ҫ�õ���GPIO�˿�
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */

static void SPEED_GPIO_Config(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2| GPIO_Pin_3; //GPIO_Pin_2 GPIO_Pin_3 ���ܻỵ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    //PA2 ��������FR����   PA3 �����ҵ��FR����
}


/*
 * ��������TIM4_Mode_Config
 * ����  ������TIM4�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void TIM4_Mode_Config(void)
{
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
	TIM_OCInitStructure.TIM_Pulse = PWM_L;	   
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=PWM_R;
	TIM_OC3Init(TIM4,&TIM_OCInitStructure); 
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);	
	
	TIM_ARRPreloadConfig(TIM4,ENABLE);		
	TIM_Cmd(TIM4,ENABLE);       								
}

void Right_HALL_IN_GPIO_Config(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef  GPIO_Initure;
	//GPIO_DeInit(GPIOB);                                     
	GPIO_Initure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //�����ҵ������HALL��ƽ�ź�
	GPIO_Initure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Initure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initure);
}
void Left_HALL_IN_GPIO_Config(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef  GPIO_Initure;
	//GPIO_DeInit(GPIOB);                                        
	GPIO_Initure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//������������HALL��ƽ�ź�
	GPIO_Initure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Initure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initure);
} 

void TIM4_PWM_Init(void)
{
	TIM4_GPIO_Config();
  SPEED_GPIO_Config();
	TIM4_Mode_Config();	
	Right_HALL_IN_GPIO_Config();
	Left_HALL_IN_GPIO_Config();
	
  GPIO_SetBits(GPIOA,GPIO_Pin_2);  //��ʼ���ߵ�ƽPA2��ת ����
	GPIO_SetBits(GPIOA,GPIO_Pin_3); //��ʼ���ߵ�ƽPA3��ת �ҵ��
}


void Hall_IRQConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line11;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
	EXTI_Init(&EXTI_InitStructure);
}
//end of file
