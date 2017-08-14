#include "dht11.h" 

uint16_t DHT_CapBuf[45];
uint8_t i=0;

//初始化DHT11的IO口 DQ 同时检测DHT11的存在 
//返回1:不存在 
//返回0:存在    	  
u8 DHT_Init(void) 
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIOB->CRL&=0XFFFFFF0F;//PORTB.1 推挽输出 
	GPIOB->CRL|=0X00000070; 
	GPIOB->ODR|=1<<1;      //输出1	   
	return 0;  
}

void DHT_TimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Prescaler=71;
	TIM_TimeBaseInitStructure.TIM_Period=25000;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,DISABLE);
	
	TIM_ICInitTypeDef TIM3_ICInitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM3_ICInitStructure.TIM_Channel=TIM_Channel_4;
	TIM3_ICInitStructure.TIM_ICFilter=0x00;
	TIM3_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM3_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
	TIM3_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	TIM_ICInit(TIM3,&TIM3_ICInitStructure);
}

void DHT_StartTimer(uint16_t delayus)
{
	TIM_SetCounter(TIM3,0);
	TIM_Cmd(TIM3,ENABLE);
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);					//清除TIM3中断挂起标志位，等待延时结束
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_CC4,DISABLE);
}

void DHT_Start(void)
{
//	if(DHT_CapBuf[43]==0xaa)
//		return ;																				//上一帧没有解码
	DHT11_DQ_OUT=1;
	DHT11_IO_OUT();
	DHT11_DQ_OUT=0;
	DHT_StartTimer(50000);
}

u8 DHT_Decode(uint8_t *temp,uint8_t *humi)
{
	uint64_t dat=0;
	uint8_t humi_point_value = 0;
	uint8_t temp_point_value = 0;
//	if(DHT_CapBuf[43]!=0xaa)
//		return 1;																			//数据未接收完成
	if(DHT_CapBuf[1]>130&DHT_CapBuf[1]<190)
	{
		for(uint8_t i=2;i<42;++i)
		{
			dat<<=1;
			if(DHT_CapBuf[i]>110&DHT_CapBuf[i]<130)
			{
				dat+=1;
			}else if(DHT_CapBuf[i]>70&DHT_CapBuf[i]<90)
			{
			}else
			{
				return 1;																	//数据错误
			}
		}
		*humi=dat>>32;
		humi_point_value = (dat >> 24) & 0xFF;
		*temp=(dat>>16)&0xFF;
		temp_point_value = (dat >> 8) & 0xFF;
		if((*temp + *humi + humi_point_value + temp_point_value)==(dat&0xFF))
		{
			DHT_CapBuf[43]=0;
			return 0;																	//数据解析完成
		}
		else
			return 1;																	//数据校验错误
	}
	return 1;																			//起始帧长度错误
}
#ifdef __cplusplus
 extern "C" {
#endif
	 
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		i=0;
		DHT11_DQ_OUT=1;
		DHT11_IO_IN();
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
		TIM_ITConfig(TIM3,TIM_IT_CC4,ENABLE);
		TIM_SetCounter(TIM3,0);
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);					//清除输入捕获通道4中断挂起标志位
		DHT_CapBuf[43]=0;
	}
	if(TIM_GetITStatus(TIM3,TIM_IT_CC4)==SET)
	{
		DHT_CapBuf[i]=TIM_GetCapture4(TIM3);
		TIM_SetCounter(TIM3,0);
		++i;
		if(i>42)
		{
			TIM_ITConfig(TIM3,TIM_IT_CC4,DISABLE);
			DHT11_IO_OUT();
			DHT11_DQ_OUT=1;
			//DHT_CapBuf[i]=0xaa;
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);
	}
}

#ifdef __cplusplus
 }
 
#endif 

