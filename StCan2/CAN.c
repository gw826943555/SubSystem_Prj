   
#include "CAN.h"
#include "led.h"
#include "SYSTICK.h"
#include "key.h"
extern   u8 test;

//#define CAN_BAUDRATE  1000      /* 1MBps   */
 #define CAN_BAUDRATE  500  /* 500kBps */
/* #define CAN_BAUDRATE  250*/  /* 250kBps */
/* #define CAN_BAUDRATE  125*/  /* 125kBps */
/* #define CAN_BAUDRATE  100*/  /* 100kBps */ 
/* #define CAN_BAUDRATE  50*/   /* 50kBps  */ 
/* #define CAN_BAUDRATE  20*/   /* 20kBps  */ 
/* #define CAN_BAUDRATE  10*/   /* 10kBps  */ 


/**************************************************************
 ** 函数名 :CAN_GPIO_Config
 ** 功能   :打开AFIO，GPIOA，CAN1时钟，初始化CAN的GPIO接口
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void CAN_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	  //PA11-RX，上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	  //PA12-TX,推挽复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/**************************************************************
 ** 函数名 :CAN_NVIC_Config
 ** 功能   :CAN中断配置
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 响应优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**************************************************************
 ** 函数名 :CAN_Interrupt_Config
 ** 功能   :CAN初始化配置和过滤器配置
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void CAN_Interrupt_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;      // CAN 配置结构体
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;// CAN 配置过滤器

	CAN_DeInit(CAN1);//将外设CAN的全部寄存器重设为缺省值
	CAN_StructInit(&CAN_InitStructure);//每个参数按缺省值写入
	/* CAN 初始化设置 */
	CAN_InitStructure.CAN_TTCM=DISABLE;// 时间触发通讯失能
	CAN_InitStructure.CAN_ABOM=DISABLE;// 自动离线管理失能
	CAN_InitStructure.CAN_AWUM=DISABLE;// 自动唤醒模式失能
	CAN_InitStructure.CAN_NART=DISABLE;// 非自动重传模式失能
	CAN_InitStructure.CAN_RFLM=DISABLE;// 接收FIFO锁定失能
	CAN_InitStructure.CAN_TXFP=DISABLE;// 发送FIFO优先级失能
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	//正常传输模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		//重新同步跳跃宽度1个时间单位（1-4）
	CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;		//时间段1为3个时间单位（1-16）
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		//时间段2为2个时间单位（1-8）
//CAN_InitStructure.CAN_Prescaler=6;			  //分频系数为5（1-1024）
	
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
	
	
	CAN_Init(CAN1,&CAN_InitStructure);		    //初始化CAN1，波特率根据需要设置
	
	/* CAN 过滤器设置 */
	CAN_FilterInitStructure.CAN_FilterNumber=0;	 //初始化过滤器0（0-13）
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  //过滤器的模式：标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //过滤器位宽：1个32位过滤器
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x0001<<21)&0xFFFF0000)>>16;               //过滤器标识符高段位
	CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x0001<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;//过滤器标识符低段位
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;//屏蔽/过滤标识符高段位（1屏蔽，0忽略）
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;	//屏蔽/过滤标识符低段位
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);	
	 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);//FIFO0消息挂号中断使能
}
/**************************************************************
 ** 函数名 :CAN_Config
 ** 功能   :CAN初始化， 由于用到了CAN接收中断，要初始化NVIC 
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void CAN_Config(void)
{
	CAN_GPIO_Config();
	CAN_NVIC_Config();
	CAN_Interrupt_Config();
}
/**************************************************************
 ** 函数名 :CAN_send
 ** 功能   :发送报文
 ** 输入   :无
 ** 输出   :无
 ** 返回   :无
 ** 注意   :无
***************************************************************/
void CAN_send(void)
{
	if(!KEY1())
	{
		CanTxMsg TxMessage;		
		TxMessage.StdId=0x0001;     // 设定标准标识符（11位）
		TxMessage.IDE=CAN_ID_STD;   // 消息标志符实验标准标识符	
		TxMessage.RTR=CAN_RTR_DATA;	// 传输消息的帧类型为数据帧	
		TxMessage.DLC=1;			      // 发送两帧，一帧8位
		TxMessage.Data[0]=0x32;		  // 第一帧数据
		CAN_Transmit(CAN1,&TxMessage);	//开始发送消息
		LED1_ON;                    //按下S1后，LED灯DS1会亮一下
	}
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	if((RxMessage.StdId==0x0001) && (RxMessage.IDE==CAN_ID_STD)
     && (RxMessage.DLC==1) && (RxMessage.Data[0]==0x32))
  {
    test = 1; //接收成功  
  }
  else
  {
    test =0;  //接收失败
  }
  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
}
