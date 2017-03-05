#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"
uint8_t BatAD_Init(void);
u16 Get_ADVal(void);
u16 Get_BatVol(void);
#endif



