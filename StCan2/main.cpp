#include "stm32f10x.h"
#include "delay.h"
#include "common.h"
#include "ColorArray.hpp"

u8 DMAFlag = 1;
u8 CANFlag = 0;
CanRxMsg RxMessage;
volatile emode CurrentLBandState = specified;
//volatile emode CurrentLBandState = specified;
volatile u8 BatteryQut = 100;
volatile u8 Intensity = 80;
volatile u8 SpecifiedR = 255;
volatile u8 SpecifiedG = 255;
volatile u8 SpecifiedB = 255;
unsigned char DMA_color_array[NUM_LED*COLOR_BASE];
unsigned char DMA_bin_array[NUM_LED*COLOR_BASE * 8+2];
int main()
{
	BSP_CONFIG();
	TIM_Cmd(TIM2, ENABLE);
	GPIOA->BRR |= 0x0001;
	while(1)
	{
		//CSingleton::GetInstance()->Test();
		if(DMAFlag)
		{
			DMAFlag = 0;
			ColorArray::Instance()
				->set_normal_command( CurrentLBandState, 
															Intensity, 
															BatteryQut,
															SpecifiedR,
															SpecifiedG,
															SpecifiedB);
			delay_ms(5);
			TIM2->CNT = 0;
			DMA_Cmd(DMA1_Channel2, DISABLE);
			DMA_SetCurrDataCounter(DMA1_Channel2,DMA_SIZE);
			UsrDMACh2_En();
		}
		if(CANFlag == 1)
		{
			CANFlag = 0;

			if((RxMessage.StdId==CAN_RX_ID) && (RxMessage.IDE==CAN_ID_STD)
				 && (RxMessage.DLC==5))
			{
				CAN_Send((u8)CurrentLBandState); 
				if(RxMessage.Data[0]<=3)
				{
					CurrentLBandState = (emode)RxMessage.Data[0];
					Intensity = RxMessage.Data[1];
					if(RxMessage.Data[0] == 1)
						{
							BatteryQut = RxMessage.Data[2];
							if(BatteryQut > 100)BatteryQut = 100;
						}						
					else if(RxMessage.Data[0] == 3)
						{
							SpecifiedR = RxMessage.Data[2];
							SpecifiedG = RxMessage.Data[3];
							SpecifiedB = RxMessage.Data[4];
						}
				}
			}
			else
			{
				CAN_Send(0xAA);
			}
		}
		BSP_LED_Toggle();
	}
	//return 0;
}
