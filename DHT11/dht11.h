#ifndef __DHT11_H 
#define __DHT11_H  

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	  
//本程序只供学习使用，未经作者许可，不得用于其它任何用途 
//Mini STM32开发板 
//DHT11 驱动代码	    
//正点原子@ALIENTEK 
//技术论坛:www.openedv.com 
//修改日期:2012/7/24  
//版本：V1.0 
//版权所有，盗版必究。 
//Copyright(C) 正点原子 2009-2019 
//All rights reserved 
//////////////////////////////////////////////////////////////////////////////////	  
  
//IO方向设置 
#define DHT11_IO_IN()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=8<<4;} 
#define DHT11_IO_OUT() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=3<<4;} 
////IO操作函数	    
#define	DHT11_DQ_OUT PBout(1) //数据端口	PD3  
#define	DHT11_DQ_IN  PBin(1)  //数据端口	PD3

u8 DHT_Init(void);//初始化DHT11 
void DHT_Start(void);
void DHT_TimerInit(void);
void DHT_StartTimer(uint16_t delayus);
u8 DHT_Decode(uint8_t *temp,uint8_t *humi);

#ifdef __cplusplus
}
#endif

#endif 

