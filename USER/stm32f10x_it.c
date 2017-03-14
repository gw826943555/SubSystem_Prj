/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "CCAN.h"
#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32f10x_it.h"
#include "common.h"	 

void SetSubsystemType();
void SubsystemDorun();
void SubsystemLedDo();
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	//CPUTIMER0_ISR();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
void TIM2_IRQHandler(void)
{
	
	TIM_ClearFlag(TIM2, TIM_FLAG_CC1); 

}

extern u8 DMAFlag;
void DMA1_Channel2_IRQHandler()
{
	
//  if(DMA_GetITStatus(DMA1_IT_TC2))
	if(DMA1->ISR&0x00000020)
  {
		GPIOA->CRL &= 0x3333FFF3;	//change the GPIO mode from AFIO to OUTPP
    DMA_ClearITPendingBit(DMA1_IT_GL2);
		
		//DMAFlag = 1;
		SubsystemLedDo();
  }
	else
	{
		//BSP_LED_Toggle();
	}

}

extern CanRxMsg tempRxMsg; 
uint8_t IAP_CMD[8]={0x09,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	memset((void*)&tempRxMsg, 0, sizeof(tempRxMsg));
  CAN_Receive(CAN1, CAN_FIFO0, &tempRxMsg);
	if(tempRxMsg.ExtId==0x5005)
	{
		for(uint8_t i=0;i<8;++i)
		{
			if(tempRxMsg.Data[i]!=IAP_CMD[i])
				return ;
		}
		CanTxMsg rstReplyMsg;
		rstReplyMsg.ExtId = 0x5004;
		rstReplyMsg.Data[0] = 0x09;
		rstReplyMsg.Data[1] = 0x00;
		rstReplyMsg.Data[2] = 0x00;
		rstReplyMsg.Data[3] = 0x00;
		rstReplyMsg.IDE = CAN_Id_Extended;
		rstReplyMsg.RTR = CAN_RTR_Data;
		rstReplyMsg.DLC = 4;
		CanRouter250k.putMsg(rstReplyMsg);
		CanRouter250k.runTransmitter();
		for(uint32_t m=0;m<0xffff;++m)
			;
		NVIC_SystemReset();
	}
	SetSubsystemType();
	SubsystemDorun();
	
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
}
/**
  * @}
  */ 

#ifdef __cplusplus
 }
#endif 
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
