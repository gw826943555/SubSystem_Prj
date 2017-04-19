#include "io.h"

void SFIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);//开C口时钟，复用时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = EMG_FBPinx |Brake_FBPinx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //设为输出　注意：2MHZ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(EMG_FBPort, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = EMG_ENPinx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //设为输出　注意：2MHZ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(EMG_ENPort, &GPIO_InitStructure);
//
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSEConfig(RCC_LSE_OFF);//关闭外部低速外部时钟信号功能 后，PC13 PC14 PC15 才可以当普通IO用。
	BKP_TamperPinCmd(DISABLE);//关闭入侵检测功能，也就是 PC13，也可以当普通IO 使用
	PWR_BackupAccessCmd(DISABLE);//禁止修改后备寄存器
	BKP_ITConfig(DISABLE);  
}

void EMG_ENCmd(FunctionalState state)
{
	if(state==ENABLE)
	{
		GPIO_WriteBit(EMG_ENPort,EMG_ENPinx,Bit_SET);
	}else{
		GPIO_WriteBit(EMG_ENPort,EMG_ENPinx,Bit_RESET);
	}
}

uint8_t SFIO_GetStatus(void)
{
	uint8_t temp=0;
	if(GPIO_ReadInputDataBit(EMG_FBPort,EMG_FBPinx)==Bit_RESET)
	{
		temp&=(~0x01);
	}else{
		temp|=0x01;
	}
	
	if(GPIO_ReadInputDataBit(Brake_FBPort,Brake_FBPinx)==Bit_RESET)
	{
		temp&=(~0x02);
	}else{
		temp|=0x02;
	}
	return temp;
}


