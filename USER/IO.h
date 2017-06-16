#ifndef __IO__H
#define __IO__H
#include "stm32f10x.h"
#define EMG_FBPinx			GPIO_Pin_13
#define EMG_FBPort			GPIOC
#define EMG_ENPinx			GPIO_Pin_14
#define EMG_ENPort			GPIOC
#define Brake_FBPinx		GPIO_Pin_15
#define Brake_FBPort		GPIOC

#define LEDPIN					GPIO_Pin_10
#define LEDPORT					GPIOA

void SFIO_Init(void);
void EMG_ENCmd(FunctionalState state);
uint8_t SFIO_GetStatus(void);
void led_init(void);
void led_toggle();


#endif

