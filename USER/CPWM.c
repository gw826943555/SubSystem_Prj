#include "CPWM.h"
u16 PWM_L = 0;
u16 PWM_R = 0;

static void TIM4_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_8; //PWM输出  通道1、3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用开漏
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * 函数名：SPEED_GPIO_Config
 * 描述  ：配置速度控制需要用到的GPIO端口
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */

static void SPEED_GPIO_Config(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2| GPIO_Pin_3; //GPIO_Pin_2 GPIO_Pin_3 可能会坏
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    //PA2 控制左电机FR方向   PA3 控制右电机FR方向
}


/*
 * 函数名：TIM4_Mode_Config
 * 描述  ：配置TIM4输出的PWM信号的模式，如周期、极性、占空比
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
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
	GPIO_Initure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //接收右电机输入HALL电平信号
	GPIO_Initure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Initure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initure);
}
void Left_HALL_IN_GPIO_Config(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef  GPIO_Initure;
	//GPIO_DeInit(GPIOB);                                        
	GPIO_Initure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//接收左电机输入HALL电平信号
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
	
  GPIO_SetBits(GPIOA,GPIO_Pin_2);  //初始化高电平PA2正转 左电机
	GPIO_SetBits(GPIOA,GPIO_Pin_3); //初始化高电平PA3正转 右电机
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
