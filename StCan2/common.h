#ifndef __COMMON_H
#define __COMMON_H	

#include "ColorArray.hpp"

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f10x.h"
#include "usrbsp.h"

#include "delay.h"
void SetSubsystemType();
void SubsystemDorun();
void SubsystemLedDo();
	 

#ifdef __cplusplus
 }
#endif 
	 
#endif
