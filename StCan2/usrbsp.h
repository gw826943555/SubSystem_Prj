#ifndef __USRBSP_H
#define __USRBSP_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f10x.h"


/****************************************************************
*	BSP_USRCONFIG
****************************************************************/
void BSP_CONFIG(void);

/****************************************************************
*	BSP_LED
****************************************************************/
#define RCC_APB2_BSP_LED	RCC_APB2Periph_GPIOA<<((BSP_LED_GPIO-GPIOA)/0x400)
#define BSP_LED_GPIO			GPIOA

#define BSP_LED_START_BIT   (4-1) //0-1

#define BSP_LED		GPIO_Pin_10


void LED_CONFIG(void);
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle (void);
/****************************************************************
*	BSP_SPI
****************************************************************/
#define Dummy_Byte		0xFF

void SPI2_CONFIG(void);
u8 SPI_ReadByte(void);
u8 SPI_SendByte(u8 byte);
u16 SPI_SendHalfWord(u16 HalfWord);
void SPI2_LoopBackTest(void);

/****************************************************************
*	BSP_TIMBASE
****************************************************************/
typedef enum
{
	nTimeUSARTSend = 0,
	nTimeLEDToggle = 1
}TIM_TestType;
void TIM2_CONFIG(void);
void TIM2_DMAConfig(void);
void TIM_Test(TIM_TypeDef* TIMx, 
							TIM_TestType TestType,
							u32 CntNum);
inline void UsrDMACh2_En(void)
{
	
//	while(0);
//	while(0);
	DMA1_Channel2->CCR |= DMA_CCR1_EN;
	GPIOA->CRL |=0x0008;
}

/****************************************************************
*	BSP_CAN
****************************************************************/
//#define CAN_BAUDRATE  1000      /* 1MBps   */
// #define CAN_BAUDRATE  500  /* 500kBps */
#define CAN_BAUDRATE  250  /* 250kBps */
/* #define CAN_BAUDRATE  125*/  /* 125kBps */
/* #define CAN_BAUDRATE  100*/  /* 100kBps */ 
/* #define CAN_BAUDRATE  50*/   /* 50kBps  */ 
/* #define CAN_BAUDRATE  20*/   /* 20kBps  */ 
/* #define CAN_BAUDRATE  10*/   /* 10kBps  */ 
#define CAN_RX_ID	21

void CAN1_CONFIG(void);
void CAN_GpioConfig(void);
void CAN_NvicConfig(void);
void CAN_InterruptConfig(void);
void CAN_Send(u8 data);

#ifdef __cplusplus
 }
#endif 
#endif 
/***********************End Of File************************/


