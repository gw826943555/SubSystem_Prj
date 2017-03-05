   
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
 ** ������ :CAN_GPIO_Config
 ** ����   :��AFIO��GPIOA��CAN1ʱ�ӣ���ʼ��CAN��GPIO�ӿ�
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void CAN_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	  //PA11-RX����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	  //PA12-TX,���츴�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/**************************************************************
 ** ������ :CAN_NVIC_Config
 ** ����   :CAN�ж�����
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // ��ռ���ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // ��Ӧ���ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**************************************************************
 ** ������ :CAN_Interrupt_Config
 ** ����   :CAN��ʼ�����ú͹���������
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void CAN_Interrupt_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;      // CAN ���ýṹ��
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;// CAN ���ù�����

	CAN_DeInit(CAN1);//������CAN��ȫ���Ĵ�������Ϊȱʡֵ
	CAN_StructInit(&CAN_InitStructure);//ÿ��������ȱʡֵд��
	/* CAN ��ʼ������ */
	CAN_InitStructure.CAN_TTCM=DISABLE;// ʱ�䴥��ͨѶʧ��
	CAN_InitStructure.CAN_ABOM=DISABLE;// �Զ����߹���ʧ��
	CAN_InitStructure.CAN_AWUM=DISABLE;// �Զ�����ģʽʧ��
	CAN_InitStructure.CAN_NART=DISABLE;// ���Զ��ش�ģʽʧ��
	CAN_InitStructure.CAN_RFLM=DISABLE;// ����FIFO����ʧ��
	CAN_InitStructure.CAN_TXFP=DISABLE;// ����FIFO���ȼ�ʧ��
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	//��������ģʽ
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		//����ͬ����Ծ���1��ʱ�䵥λ��1-4��
	CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;		//ʱ���1Ϊ3��ʱ�䵥λ��1-16��
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		//ʱ���2Ϊ2��ʱ�䵥λ��1-8��
//CAN_InitStructure.CAN_Prescaler=6;			  //��Ƶϵ��Ϊ5��1-1024��
	
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
	
	
	CAN_Init(CAN1,&CAN_InitStructure);		    //��ʼ��CAN1�������ʸ�����Ҫ����
	
	/* CAN ���������� */
	CAN_FilterInitStructure.CAN_FilterNumber=0;	 //��ʼ��������0��0-13��
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  //��������ģʽ����ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //������λ��1��32λ������
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x0001<<21)&0xFFFF0000)>>16;               //��������ʶ���߶�λ
	CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x0001<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;//��������ʶ���Ͷ�λ
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;//����/���˱�ʶ���߶�λ��1���Σ�0���ԣ�
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;	//����/���˱�ʶ���Ͷ�λ
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);	
	 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);//FIFO0��Ϣ�Һ��ж�ʹ��
}
/**************************************************************
 ** ������ :CAN_Config
 ** ����   :CAN��ʼ���� �����õ���CAN�����жϣ�Ҫ��ʼ��NVIC 
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void CAN_Config(void)
{
	CAN_GPIO_Config();
	CAN_NVIC_Config();
	CAN_Interrupt_Config();
}
/**************************************************************
 ** ������ :CAN_send
 ** ����   :���ͱ���
 ** ����   :��
 ** ���   :��
 ** ����   :��
 ** ע��   :��
***************************************************************/
void CAN_send(void)
{
	if(!KEY1())
	{
		CanTxMsg TxMessage;		
		TxMessage.StdId=0x0001;     // �趨��׼��ʶ����11λ��
		TxMessage.IDE=CAN_ID_STD;   // ��Ϣ��־��ʵ���׼��ʶ��	
		TxMessage.RTR=CAN_RTR_DATA;	// ������Ϣ��֡����Ϊ����֡	
		TxMessage.DLC=1;			      // ������֡��һ֡8λ
		TxMessage.Data[0]=0x32;		  // ��һ֡����
		CAN_Transmit(CAN1,&TxMessage);	//��ʼ������Ϣ
		LED1_ON;                    //����S1��LED��DS1����һ��
	}
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	if((RxMessage.StdId==0x0001) && (RxMessage.IDE==CAN_ID_STD)
     && (RxMessage.DLC==1) && (RxMessage.Data[0]==0x32))
  {
    test = 1; //���ճɹ�  
  }
  else
  {
    test =0;  //����ʧ��
  }
  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
}
