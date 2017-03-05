#include "stm32f10x.h"
#include "CommonConfig.h"
#include <stdint.h>
//#include "Timer.h"
#include "CCan.h"
#include "CPWM.h"
//#include "CUartCanProbe.h"
//#include "CUartConsole.h"
#include "sys.h"
#include "dht11.h"
#include "delay.h"
#include "common.h"
#include "ColorArray.hpp"
#include "ADC.h"
#include "delay.h"

enum subsystemMode
{
	SUBSYSTEM_NONE = 0,
	SUBSYSTEM_LED,
	SUBSYSTEM_DRIVER, 
	SUBSYSTEM_IO,
	SUBSYSTEM_DHT,
	SUBSYSTEM_ADC
};

/////////////////////DRIVER param///////////////////
union
{
u16 speed;
u8 byte[2];
}SPEED;	
u16 setspeed =0;
u16 unit =10; //单位0.1counts/s
u16 series =24;//电机级数
u16 stomin =60;//转速单位转换
u16 period =3;//转速与占空比之比
CanTxMsg tempTxMsg;
CanRxMsg tempRxMsg;  
CanTxMsg okTxMsg;
CanTxMsg ADCMsg;
CanTxMsg DhtTxMsg;
CanTxMsg InputIOMsg;
u8 Dhttemp = 0;
u8 Dhthumi = 0;
u16 Hall_Input_Data_left = 0;
u16 Hall_Input_Data_left_Before = 0;
u16 Hall_Input_Data_right = 0;
u16 Hall_Input_Data_right_Before = 0;
s32 Wheel_Circle_Point_Right = 0;
s32 Wheel_Circle_Right= 0; 	
s32 Wheel_Circle_Point_Left = 0;
s32 Wheel_Circle_Left= 0; 	
u16 CorrectHall[]={5,1,3,2,6,4};
bool left_first_in =true;
bool right_first_in= true;
bool error_hall_left=false;
bool error_hall_right=false;
/////////////////////DHT11 param////////////////////

/////////////////////StCan2 param///////////////////
u8 DMAFlag = 1;
u8 CANFlag = 0;
CanRxMsg RxMessage;
volatile emode CurrentLBandState = specified;
volatile u8 BatteryQut = 100;
volatile u8 Intensity =  0;
volatile u8 SpecifiedR = 0;
volatile u8 SpecifiedG = 0;
volatile u8 SpecifiedB = 0;
unsigned char DMA_color_array[NUM_LED*COLOR_BASE];
unsigned char DMA_bin_array[NUM_LED*COLOR_BASE * 8+2];
/////////////////////ADC param///////////////////////
u16 AD_VAL = 0;
/////////////////////MAIN param//////////////////////
subsystemMode _nowType = SUBSYSTEM_NONE;
bool is_driver_open = false;
bool is_led_open = false;
/////////////////////CANID///////////////////////////
#define CAN_Config 21
#define CAN_Config_Back 22
#define CAN_Config_LEFTDRIVER_BACK 23
#define CAN_Config_LEFTDRIVER 24
#define CAN_Config_RIGHTDRIVER_BACK 25
#define CAN_Config_RIGHTDRIVER 26
#define CAN_Config_LED 27
#define CAN_Config_LED_Back 28
#define CAN_DEMAND_DHT 29
#define CAN_DEMAND_DHT_BACK 30
#define CAN_DEMAND_VOL 31
#define CAN_DEMAND_VOL_BACK 32
#define CAN_Config_IO 33
#define CAN_Config_IO_BACK 34
//////////////////////FUNC///////////////////////////
int16_t SetMotorSpeed(CanRxMsg tempRxMsg);
int16_t GetMotorPos(CanRxMsg tempRxMsg);
void IsNewHallDataIn();
void Clear();
void TransimitPos();
void SubsystemLed();
void StopMotor();
void SubsystemIOConfig();
void SubsystemDHTRead();
void SetSubsystemType();
void SubsystemADC();
//////////////////////MAIN///////////////////////////

int main()
{
	//SCB->VTOR=0x08008000;
	CommonConfig();
	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_8);
	CanRouter250k.InitCan();
	CanRouter250k.InitCanGpio(CCanRouter::GROUP_A11);
	delay_init();
	DHT_Init();  //DHT_Init
	DHT_TimerInit();
	BatAD_Init();//ADC_Init
	
	while(1)
	{
		DHT_Start();
		Clear();
		AD_VAL = Get_BatVol();
		delay_ms(500);
		DhtTxMsg.Data[1]=DHT_Decode(&Dhttemp,&Dhthumi);			//温湿度解析结果
	}
}

//////////////////////Set type//////////////////////////
void SetSubsystemType()
{
	if(tempRxMsg.StdId == CAN_Config)
	{
			if(tempRxMsg.Data[0]&0x01)
			{
				is_led_open = true;
				//is_driver_open = false;
				okTxMsg.Data[0]|=0x01;
			  BSP_CONFIG();//StCan_Init
        TIM_Cmd(TIM2, ENABLE);
        GPIOA->BRR |= 0x0001; 
				//StopMotor();          //关闭驱动器
				//TIM_Cmd(TIM4, DISABLE);  
				//EXTI_DeInit();
			}else{
				is_led_open = false;
				TIM_Cmd(TIM2, DISABLE);    //关闭LED
				DMA_Cmd(DMA1_Channel2, DISABLE);
				okTxMsg.Data[0]&=0xFE;
			}
			
			if(tempRxMsg.Data[0]&0x02)
			{
				//is_led_open = false;
				is_driver_open = true;
				okTxMsg.Data[0]|=0x02;
				TIM4_PWM_Init(); //PWM Init
				Hall_IRQConfig();
				GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9);     
			}else{
				is_driver_open=false;
				okTxMsg.Data[0]&=0xFD;
				StopMotor();          //关闭驱动器
				TIM_Cmd(TIM4, DISABLE);  
				EXTI_DeInit();
			}
			okTxMsg.DLC = 2;
			okTxMsg.StdId = CAN_Config_Back;		
			CanRouter250k.putMsg(okTxMsg);
			CanRouter250k.runTransmitter();
	}
	
	else if((tempRxMsg.StdId == CAN_Config_LEFTDRIVER)||(tempRxMsg.StdId == CAN_Config_RIGHTDRIVER))
	{
		_nowType = SUBSYSTEM_DRIVER;
	}
	else if(tempRxMsg.StdId == CAN_Config_LED)
	{
		_nowType = SUBSYSTEM_LED;
	}
	else if(tempRxMsg.StdId == CAN_Config_IO)
	{
		_nowType = SUBSYSTEM_IO;		 
	}
	else if(tempRxMsg.StdId == CAN_DEMAND_DHT)
	{
		_nowType = SUBSYSTEM_DHT;	
	}
	else if(tempRxMsg.StdId == CAN_DEMAND_VOL)
	{
		_nowType = SUBSYSTEM_ADC;	
	}
	else
	  _nowType = SUBSYSTEM_NONE;
}

//////////////////////do run ///////////////////////////
void SubsystemDorun()
{
	 switch(_nowType)
   {
 			case SUBSYSTEM_NONE:
			     break;
			case SUBSYSTEM_LED:
		   { if(is_led_open)
				{SubsystemLed();}
	       break;}	
			case SUBSYSTEM_DRIVER:
			 { if(is_driver_open){
				 SetMotorSpeed(tempRxMsg);
				 TransimitPos();}
			   break;}
      case SUBSYSTEM_IO:
       { SubsystemIOConfig();
			   break;}			
      case SUBSYSTEM_DHT:
			 { SubsystemDHTRead();
			   break;}
      case SUBSYSTEM_ADC:
			{  SubsystemADC();
			   break;}
			default: 
				   break;
	}
}

//////////////////////////DHT///////////////////////////////
void SubsystemDHTRead()
{
	if((tempRxMsg.Data[1]&0x01) == 1) 
	{  DhtTxMsg.DLC =4;
     DhtTxMsg.StdId = 30;
		 DhtTxMsg.Data[0]=0x00;
		// DhtTxMsg.Data[1]=0x00;			//DHT_Decode() 解析结果
		 DhtTxMsg.Data[2]=Dhttemp;
		 DhtTxMsg.Data[3]=Dhthumi;
		 CanRouter250k.putMsg(DhtTxMsg);
		 CanRouter250k.runTransmitter();}
}
//////////////////////////IO/////////////////////////////
void SubsystemIOConfig()
{
	if((tempRxMsg.Data[0]&0x01) == 1)  
	 {
			u16 ReadInputIO;

			ReadInputIO = GPIO_ReadInputData(GPIOB)&0xFC3C;
			InputIOMsg.DLC = 4;
			InputIOMsg.StdId = CAN_Config_IO_BACK;
			InputIOMsg.Data[0] =0x01;
			InputIOMsg.Data[1] = (((ReadInputIO>>2)&0x0F)|((ReadInputIO>>6)&0xF0));
			InputIOMsg.Data[2] = (ReadInputIO>>14)&0x03;		
			InputIOMsg.Data[3] = 0;
			CanRouter250k.putMsg(InputIOMsg);
			CanRouter250k.runTransmitter();
	 }
	 else if((tempRxMsg.Data[0]&0x02) == 2)
	 {
		//控IO		
		 if(false==is_driver_open)
		 {
				if((tempRxMsg.Data[2])&0x04) 
				{
						GPIOB->CRL&=0xF0FFFFFF;								//IO模式修改为推挽输出
						GPIOB->CRL|=0x03000000;
						GPIO_SetBits(GPIOB, GPIO_Pin_6);
				}
				else
				{
						GPIOB->CRL&=0xF0FFFFFF;								//IO模式修改为推挽输出
						GPIOB->CRL|=0x03000000;
						GPIO_ResetBits(GPIOB, GPIO_Pin_6);
				}
				
				if((tempRxMsg.Data[2])&0x10) 
				{
					GPIOB->CRH&=0xFFFFFFF0;								//IO模式修改为推挽输出
					GPIOB->CRH|=0x00000003;
					GPIO_SetBits(GPIOB, GPIO_Pin_8);
				}
				else
				{
					GPIOB->CRH&=0xFFFFFFF0;								//IO模式修改为推挽输出
					GPIOB->CRH|=0x00000003;
					GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				}
				
				if((tempRxMsg.Data[1]&0x10)==0x10)			//PWR_EN4
				{
					GPIO_SetBits(GPIOA,GPIO_Pin_9);
				}else
				{
					GPIO_ResetBits(GPIOA,GPIO_Pin_9);
				}
				
				if((tempRxMsg.Data[1]&0x20)==0x20)			//PWR_EN5
				{
					GPIO_SetBits(GPIOA,GPIO_Pin_8);
				}else
				{
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);
				}
		 }
		 else
		 {
			 GPIO_SetBits(GPIOA,GPIO_Pin_8);				//打开驱动器电源
			 GPIO_SetBits(GPIOA,GPIO_Pin_9);
		 }
		 
		 if(tempRxMsg.Data[1]&0x01)								//PD0
		 {
			 GPIO_SetBits(GPIOA,GPIO_Pin_5);
		 }else
		 {
			 GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		 }
		 
		 if(tempRxMsg.Data[1]&0x02)								//PD1
		 {
			 GPIO_SetBits(GPIOA,GPIO_Pin_4);
		 }else
		 {
			 GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		 }
		 
		 if(tempRxMsg.Data[1]&0x04)								//PD2
		 {
			 GPIO_SetBits(GPIOA,GPIO_Pin_7);
		 }else
		 {
			 GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		 }
		 
		 if(tempRxMsg.Data[1]&0x08)								//PD3
		 {
			 GPIO_SetBits(GPIOA,GPIO_Pin_6);
		 }else
		 {
			 GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		 }
		 
		if(((tempRxMsg.Data[1]>>4)&0x08) == 8)
		{
			GPIOA->CRL&=0xFFFFFF0F;								//IO模式修改为推挽输出
			GPIOA->CRL|=0x00000030;
			GPIO_SetBits(GPIOA, GPIO_Pin_1);
		}
		else
		{
			GPIOA->CRL&=0xFFFFFF0F;
			GPIOA->CRL|=0x00000030;
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		}
		
		if((tempRxMsg.Data[2])&0x08)
		{
			GPIOB->CRL&=0x0FFFFFFF;
			GPIOB->CRL|=0x30000000;
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
		}
		else
		{
			GPIOB->CRL&=0x0FFFFFFF;
			GPIOB->CRL|=0x30000000;
			GPIO_ResetBits(GPIOB, GPIO_Pin_7);
		}
		
		if((tempRxMsg.Data[2])&0x20)
		{
			GPIOB->CRH&=0xFFFFFF0F;
			GPIOB->CRH|=0x00000030;
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
		}
		else
		{
			GPIOB->CRH&=0xFFFFFF0F;
			GPIOB->CRH|=0x00000030;
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
		}
		
		if(((tempRxMsg.Data[2])&0x01) == 1)			//PWMA2
		{
			GPIOA->CRL&=0xFFFFF0FF;								//IO模式修改为推挽输出
			GPIOA->CRL|=0x00000300;
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
		}
		else{
			GPIOA->CRL&=0xFFFFF0FF;								//IO模式修改为推挽输出
			GPIOA->CRL|=0x00000300;
			GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		}
		
		if(((tempRxMsg.Data[2])&0x02) == 2)  		//PWMA3
		{
			GPIOA->CRL&=0xFFFF0FFF;								//IO模式修改为推挽输出
			GPIOA->CRL|=0x00003000;
			GPIO_SetBits(GPIOA, GPIO_Pin_3);
		}
		else
		{
			GPIOA->CRL&=0xFFFF0FFF;								//IO模式修改为推挽输出
			GPIOA->CRL|=0x00003000;
			GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		}

		if(false==is_led_open)
		{
			if(((tempRxMsg.Data[1]>>4)&0x04) == 4)  
			{
				GPIOA->CRL&=0xFFFFFFF0;								//IO模式修改为推挽输出
				GPIOA->CRL|=0x00000003;
				GPIO_SetBits(GPIOA, GPIO_Pin_0);
			}
		  else
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_0);
				GPIOA->CRL&=0xFFFFFFF0;								//IO模式修改为推挽输出
				GPIOA->CRL|=0x00000003;
			}
		}
		okTxMsg.DLC = 4;
		okTxMsg.StdId = CAN_Config_IO_BACK;
		okTxMsg.Data[0]=0x02;
		CanRouter250k.putMsg(okTxMsg);
		CanRouter250k.runTransmitter();
	}
}
///////////////////////LED////////////////////////////
void SubsystemLed()
{
		 if(tempRxMsg.StdId == CAN_Config_LED)
			{
				if(tempRxMsg.Data[0]<=3){
				CurrentLBandState = (emode)tempRxMsg.Data[0];}
				Intensity = tempRxMsg.Data[1];
				if(tempRxMsg.Data[0] == 1)
						{
							BatteryQut = tempRxMsg.Data[2];
							if(BatteryQut > 100)BatteryQut = 100;
						}			
				else if(tempRxMsg.Data[0] == 3)
						{
							SpecifiedR = tempRxMsg.Data[2];
							SpecifiedG = tempRxMsg.Data[3];
							SpecifiedB = tempRxMsg.Data[4];
						}		
				okTxMsg.DLC = 2;
        okTxMsg.StdId = CAN_Config_LED_Back;
		    okTxMsg.Data[0]=0x02;
			  okTxMsg.Data[1]=0xAA;
		    CanRouter250k.putMsg(okTxMsg);
		    CanRouter250k.runTransmitter();
			}	 
			
}
void SubsystemLedDo()
{
//		if(DMAFlag)
//			 {
//			  DMAFlag = 0;
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
//		   }
}
////////////////////////ADC////////////////////////////////
void SubsystemADC()
{
	if((tempRxMsg.Data[1]&0x01)== 1)
	{
		
		ADCMsg.DLC = 2;
		ADCMsg.StdId = 32;
		ADCMsg.Data[0] =AD_VAL&0xFF;
		ADCMsg.Data[1] =(AD_VAL>>8)&0xFF;
		CanRouter250k.putMsg(ADCMsg);
		CanRouter250k.runTransmitter();
	}
}

////////////////////////Motor Drivers//////////////////////////
int16_t SetMotorSpeed(CanRxMsg tempRxMsg)
{	

 if((tempRxMsg.StdId == CAN_Config_LEFTDRIVER) && (tempRxMsg.Data[5]==0x10)) //左电机
 {
	 if(tempRxMsg.Data[0] == 0xFF) //负速度
	 {
		SPEED.byte[0]=tempRxMsg.Data[3];
	  SPEED.byte[1]=tempRxMsg.Data[2];
		setspeed=(((u16)(~(SPEED.speed-1)))*stomin)/(unit*series*period);     
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);  // set left wheel direction
		TIM_SetCompare1(TIM4,setspeed);   
		setspeed =0;
		 
		//send ok msg
    okTxMsg.DLC = 6;
    okTxMsg.StdId = CAN_Config_LEFTDRIVER_BACK;
		okTxMsg.Data[5]=0x10;
    okTxMsg.Data[4] =	0;
		CanRouter250k.putMsg(okTxMsg);
		CanRouter250k.runTransmitter();
    		 
		return 1;
	 }
	 
	 else//正速度
	 {
		SPEED.byte[0]=tempRxMsg.Data[3];
	  SPEED.byte[1]=tempRxMsg.Data[2];
		setspeed=(SPEED.speed*stomin)/(unit*series*period);
		GPIO_SetBits(GPIOA,GPIO_Pin_2); // set left wheel direction
		TIM_SetCompare1(TIM4,setspeed);
		setspeed =0;
		 
		 //send ok msg
		okTxMsg.DLC = 6;
    okTxMsg.StdId = CAN_Config_LEFTDRIVER_BACK;
		okTxMsg.Data[5]=0x10;
    okTxMsg.Data[4] =	0;
		CanRouter250k.putMsg(okTxMsg);
		CanRouter250k.runTransmitter();
		return 1;
	 }
 }
 
 else if((tempRxMsg.StdId == CAN_Config_RIGHTDRIVER) && (tempRxMsg.Data[5]==0x10)) //右电机
 {
	  if(tempRxMsg.Data[0] == 0xFF) //负速度
	 {
		SPEED.byte[0]=tempRxMsg.Data[3];
	  SPEED.byte[1]=tempRxMsg.Data[2];
		setspeed=(((u16)(~(SPEED.speed-1)))*stomin)/(unit*series*period);
		GPIO_ResetBits(GPIOA,GPIO_Pin_3); // set right wheel direction
		TIM_SetCompare3(TIM4,setspeed); 
		setspeed =0;
		
		//send ok msg
		okTxMsg.DLC = 6;
    okTxMsg.StdId = CAN_Config_RIGHTDRIVER_BACK;
		okTxMsg.Data[5]=0x10;
    okTxMsg.Data[4] =	0;
		CanRouter250k.putMsg(okTxMsg);
		CanRouter250k.runTransmitter();
		return 1;
	 }
	 else//正速度
	 {
		SPEED.byte[0]=tempRxMsg.Data[3];
	  SPEED.byte[1]=tempRxMsg.Data[2];
		setspeed=(SPEED.speed*stomin)/(unit*series*period);
		GPIO_SetBits(GPIOA,GPIO_Pin_3); // set right wheel direction
		TIM_SetCompare3(TIM4,setspeed);
	  setspeed =0;
 		 
		//send ok msg 
		okTxMsg.DLC = 6;
    okTxMsg.StdId = CAN_Config_RIGHTDRIVER_BACK;
		okTxMsg.Data[5]=0x10;
    okTxMsg.Data[4] =	0;
		CanRouter250k.putMsg(okTxMsg);
		CanRouter250k.runTransmitter();
		return 1;
	 }
 }
 
 else{ return 0;}
} 

void IsNewHallDataIn()
{
///////////////////left//////////////////////////////////////////
 if(left_first_in)
 left_first_in =false;
 else
 {
	if(Hall_Input_Data_left!=Hall_Input_Data_left_Before)
		{ 
			int i;
			for(i=0;i<6;i++)                     
			{ if(Hall_Input_Data_left==CorrectHall[i])
			  break;}    			//find pos on array
			
			if((i!=0)&&(i!=5))
		{
			if(Hall_Input_Data_left_Before ==CorrectHall[i-1])
			Wheel_Circle_Point_Left++;
      else if(Hall_Input_Data_left_Before ==CorrectHall[i+1])
			Wheel_Circle_Point_Left--;	
			else                                //jump a hall sign
			{
	      error_hall_left =true;//TODO	
			}
		}
		  else if(i==0)
			{
				if(Hall_Input_Data_left_Before ==CorrectHall[5])
				Wheel_Circle_Point_Left++;
			  else if(Hall_Input_Data_left_Before ==CorrectHall[i+1])
				Wheel_Circle_Point_Left--;
				else
				error_hall_left =true; //TODO
			}
			else if(i==5)
			{
				if(Hall_Input_Data_left_Before ==CorrectHall[i-1])
				Wheel_Circle_Point_Left++;
			  else if(Hall_Input_Data_left_Before ==CorrectHall[0])
				Wheel_Circle_Point_Left--;
				else
				error_hall_left =true;//TODO
			}
			else
			{;}
		}
	}
////////////////////right////////////////////////////////////////	
  if(right_first_in)
  right_first_in=false;
  else
	{
	if(Hall_Input_Data_right!=Hall_Input_Data_right_Before)
		{ 
			int i;
			for(i=0;i<6;i++)                    
			{ if(Hall_Input_Data_right== CorrectHall[i])
			  break;
			}                            //find pos on array
			if((i!=0)&&(i!=5))
			{
			 if(Hall_Input_Data_right_Before == CorrectHall[i-1])
			 Wheel_Circle_Point_Right++;
       else if(Hall_Input_Data_right_Before == CorrectHall[i+1])
			 Wheel_Circle_Point_Right--;	
			 else                                //jump a hall sign
		  	{
	       error_hall_right =true;//TODO		  
		 	  }
		  }
		  else if(i==0)
			{
				if(Hall_Input_Data_right_Before ==CorrectHall[5])
				Wheel_Circle_Point_Right++;
			  else if(Hall_Input_Data_right_Before ==CorrectHall[i+1])
				Wheel_Circle_Point_Right--;
        else
        error_hall_right =true;
 			}
			else if(i==5)
			{
				if(Hall_Input_Data_right_Before ==CorrectHall[i-1])
				Wheel_Circle_Point_Right++;
			  else if(Hall_Input_Data_right_Before ==CorrectHall[0])
				Wheel_Circle_Point_Right--;
				else
				error_hall_right =true;
			} 
			else
			{;}
		} 	
	}  
}

int16_t GetMotorPos(CanRxMsg tempRxMsg)
{
///////////////////left////////////////////////////////////////// 
	Hall_Input_Data_left=(GPIO_ReadInputData(GPIOB)&0x1C)>>2 ;
	switch(Hall_Input_Data_left)
	{
		case 5: IsNewHallDataIn();break;  //101
		case 1: IsNewHallDataIn();break;  //001
		case 3: IsNewHallDataIn();break;  //011
		case 2: IsNewHallDataIn();break;  //010
		case 6: IsNewHallDataIn();break;  //110
		case 4: IsNewHallDataIn();break;  //100
	}	 
	Hall_Input_Data_left_Before=Hall_Input_Data_left;
	Wheel_Circle_Left=Wheel_Circle_Point_Left; 
	
////////////////////////////////right/////////////////////////////
	Hall_Input_Data_right=(GPIO_ReadInputData(GPIOB)&0x1C00)>>10;
  switch(Hall_Input_Data_right)
	{
		case 5: IsNewHallDataIn();break;  //101
		case 1: IsNewHallDataIn();break;  //001
		case 3: IsNewHallDataIn();break;  //011
		case 2: IsNewHallDataIn();break;  //010
		case 6: IsNewHallDataIn();break;  //110
		case 4: IsNewHallDataIn();break;  //100
	}
	Hall_Input_Data_right_Before=Hall_Input_Data_right;
	Wheel_Circle_Right=Wheel_Circle_Point_Right; 
	return 1;

}

void TransimitPos()
{
	if((tempRxMsg.StdId == CAN_Config_LEFTDRIVER)&& (tempRxMsg.Data[4]==0x01))
	{
		if(tempRxMsg.Data[5]==0)
		{tempTxMsg.StdId = CAN_Config_LEFTDRIVER_BACK;
		tempTxMsg.DLC =6;
		tempTxMsg.Data[3] = (Wheel_Circle_Left)&0xFF;
		tempTxMsg.Data[2] = (Wheel_Circle_Left>>8)&0xFF;
		tempTxMsg.Data[1] = (Wheel_Circle_Left>>16)&0xFF;
		tempTxMsg.Data[0] = (Wheel_Circle_Left>>24)&0xFF;
		tempTxMsg.Data[4] =0x01;
		tempTxMsg.Data[5] = 0;
		CanRouter250k.putMsg(tempTxMsg);
		CanRouter250k.runTransmitter();}
	}
		
	else if ((tempRxMsg.StdId == CAN_Config_RIGHTDRIVER)&& (tempRxMsg.Data[4]==0x01))
	{
    if(tempRxMsg.Data[5]==0)
		{tempTxMsg.StdId = CAN_Config_RIGHTDRIVER_BACK;
		tempTxMsg.DLC =6;
		tempTxMsg.Data[3] = (Wheel_Circle_Right)&0xFF;
		tempTxMsg.Data[2] = (Wheel_Circle_Right>>8)&0xFF;
		tempTxMsg.Data[1] = (Wheel_Circle_Right>>16)&0xFF;
		tempTxMsg.Data[0] = (Wheel_Circle_Right>>24)&0xFF;
		tempTxMsg.Data[4] =0x01;
		tempTxMsg.Data[5] = 0;
		CanRouter250k.putMsg(tempTxMsg);
		CanRouter250k.runTransmitter();}
	}
	else
	{;}
}

void Clear()
{
	tempRxMsg.StdId = 0;
	tempRxMsg.Data[0] = 0;
	tempRxMsg.Data[1] = 0;
	tempRxMsg.Data[2] = 0;
	tempRxMsg.Data[3] = 0;
	tempRxMsg.Data[4] = 0;
	tempRxMsg.Data[5] = 0;
	tempRxMsg.Data[6] = 0;
  tempRxMsg.Data[7] = 0;
	
	okTxMsg.StdId = 0;
	okTxMsg.Data[0] = 0;
	okTxMsg.Data[1] = 0;
	okTxMsg.Data[2] = 0;
	okTxMsg.Data[3] = 0;
	okTxMsg.Data[4] = 0;
	okTxMsg.Data[5] = 0;
	
	_nowType = SUBSYSTEM_NONE;
}

void StopMotor()
{
  TIM_SetCompare1(TIM4,0);
	TIM_SetCompare3(TIM4,0);
}

#ifdef __cplusplus
 extern "C" {
#endif 
void EXTI2_IRQHandler(void)
{
	GetMotorPos(tempRxMsg);
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{
	GetMotorPos(tempRxMsg);
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	GetMotorPos(tempRxMsg);
	EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI15_10_IRQHandler(void)
{
	GetMotorPos(tempRxMsg);
	if(EXTI_GetITStatus(EXTI_Line10)==SET)
		EXTI_ClearITPendingBit(EXTI_Line10);
	if(EXTI_GetITStatus(EXTI_Line11)==SET)
		EXTI_ClearITPendingBit(EXTI_Line11);
	if(EXTI_GetITStatus(EXTI_Line12)==SET)
		EXTI_ClearITPendingBit(EXTI_Line12);
}
#ifdef __cplusplus
 }
#endif 







