#ifndef __CAN_H
#define __CAN_H

#include "stm32f10x.h"
void CAN_GPIO_Config(void);
void CAN_NVIC_Config(void);
void CAN_Interrupt_Config(void);
void CAN_Config(void);
void CAN_send(void);
#endif
