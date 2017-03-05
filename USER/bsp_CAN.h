#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f10x.h"
	
/****************************************************************
*	BSP_CAN
****************************************************************/
	
#define CAN_IOGROUP_B8	1//change here
#define CAN_IOGROUP_A11	0//change here
	
//#define CAN_BAUDRATE  1000      /* 1MBps   */
// #define CAN_BAUDRATE  500  /* 500kBps */
#define CAN_BAUDRATE  250  /* 250kBps */
/* #define CAN_BAUDRATE  125*/  /* 125kBps */
/* #define CAN_BAUDRATE  100*/  /* 100kBps */ 
/* #define CAN_BAUDRATE  50*/   /* 50kBps  */ 
/* #define CAN_BAUDRATE  20*/   /* 20kBps  */ 
/* #define CAN_BAUDRATE  10*/   /* 10kBps  */ 
#define CAN_RX_ID_L	0x0002
#define CAN_RX_ID_R	0x0004
#define CAN_FILTER_L 0
#define CAN_FILTER_R 1
#define CAN_FIFO_L	CAN_FIFO0
#define CAN_FIFO_R	CAN_FIFO1


void CAN1_CONFIG(void);
void CAN_GpioConfig(void);
void CAN_NvicConfig(void);
void CAN_InterruptConfig(void);
void CAN_Send(u8 data);
#ifdef __cplusplus
}
#endif 

#endif
