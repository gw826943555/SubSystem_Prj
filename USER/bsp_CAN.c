/**
******************************************************************************
* @file    bsp_CAN.c
* @author  Chenxx
* @version V1.0
* @date    2015-09-05
* @brief   CAN 
******************************************************************************
*/ 
#include "bsp_CAN.h"
void CAN1_CONFIG(void)
{
	CAN_GpioConfig();
	CAN_NvicConfig();
	CAN_InterruptConfig();
}
/**************************************************************
CAN_GpioConfig
***************************************************************/
void CAN_GpioConfig(void)
{		
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	  //PA11-RX
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	  //PA12-TX
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure; 
	uint32_t RCC_APB2Periph_GPIOx;
	uint8_t GPIO_PinSource_BASE;
	uint8_t GPIO_AF_USARTx;
	GPIO_TypeDef *GPIOx;
	
	#if CAN_IOGROUP_B8
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
		GPIOx = GPIOB;
		GPIO_PinSource_BASE = GPIO_PinSource8;
		GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	#elif CAN_IOGROUP_A11
		RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource11;
	#else 
		#error
	#endif
	
	/* open clock of gpio */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 << GPIO_PinSource_BASE; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOx, &GPIO_InitStructure);	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 << GPIO_PinSource_BASE; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; 
	GPIO_Init(GPIOx, &GPIO_InitStructure);
	
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
/**************************************************************
CAN_NvicConfig
***************************************************************/
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
/**************************************************************
CAN_InterruptConfig
***************************************************************/
void CAN_InterruptConfig(void)
{
	CAN_InitTypeDef        CAN_InitStructure;      
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	/* CAN  */
	CAN_InitStructure.CAN_TTCM=DISABLE; //Disable the time trig communication
	CAN_InitStructure.CAN_ABOM=DISABLE; //Disable the auto bus-off management
	CAN_InitStructure.CAN_AWUM=DISABLE; //Disable the auto wakeup management
	CAN_InitStructure.CAN_NART=DISABLE; //Disable the none auto retransmit
	CAN_InitStructure.CAN_RFLM=DISABLE; //Disable the receive FIFO lock management
	CAN_InitStructure.CAN_TXFP=DISABLE; //Disalbe the transmit FIFO priority
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	//CAN work on normal mode
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;			//Resynchronization jump width
	CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;			//
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;			//
//CAN_InitStructure.CAN_Prescaler=6;			  
	
#if CAN_BAUDRATE == 1000 /* 1MBps */
  CAN_InitStructure.CAN_Prescaler =6;
#elif CAN_BAUDRATE == 500 /* 500KBps */
  CAN_InitStructure.CAN_Prescaler =12;
#elif CAN_BAUDRATE == 250 /* 250KBps */
  CAN_InitStructure.CAN_Prescaler =24;
#elif CAN_BAUDRATE == 125 /* 125KBps */
  CAN_InitStructure.CAN_Prescaler =48;
#elif  CAN_BAUDRATE == 100 /* 100KBps */
  CAN_InitStructure.CAN_Prescaler =60;
#elif  CAN_BAUDRATE == 50 /* 50KBps */
  CAN_InitStructure.CAN_Prescaler =120;
#elif  CAN_BAUDRATE == 20 /* 20KBps */
  CAN_InitStructure.CAN_Prescaler =300;
#elif  CAN_BAUDRATE == 10 /* 10KBps */
  CAN_InitStructure.CAN_Prescaler =600;
#else
   #error "Please select first the CAN Baudrate in Private defines in main.c "
#endif  /* CAN_BAUDRATE == 1000 */	
	
	
	CAN_Init(CAN1,&CAN_InitStructure);		   
	
	/* CAN Filter Configuration */
	CAN_FilterInitStructure.CAN_FilterNumber = CAN_FILTER_L;					//set the filter that will be actived 	 
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;  	//Filtmode set as ID mask
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//Filter width, set as 32-bit
	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)CAN_RX_ID_L<<21)&0xFFFF0000)>>16;               
	CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)CAN_RX_ID_L<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF; 						//Mask ID_H
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;							//Mask ID_L
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO_L;			
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);	
	
	/* CAN Filter Configuration */
	CAN_FilterInitStructure.CAN_FilterNumber = CAN_FILTER_R;					//set the filter that will be actived 	 
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;  	//Filtmode set as ID mask
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//Filter width, set as 32-bit
	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)CAN_RX_ID_R<<21)&0xFFFF0000)>>16;               
	CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)CAN_RX_ID_R<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF; 						//Mask ID_H
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;							//Mask ID_L
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO_R;			
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);	
	 
//	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
}
/**************************************************************
CAN_Send
***************************************************************/
void CAN_Send(u8 data)
{
	if(/**/1)
	{
		CanTxMsg TxMessage;		
		TxMessage.StdId=0x0001;     //Set the standard ID (11 bits)
		TxMessage.IDE=CAN_ID_STD;   //Set ID type as standard
		TxMessage.RTR=CAN_RTR_DATA;	//Set the frame as data 
		TxMessage.DLC=1;			      // data length 1 byte
		TxMessage.Data[0]=data;		  // the 1st byte data
		CAN_Transmit(CAN1,&TxMessage);	//start to transmit

	}
}
