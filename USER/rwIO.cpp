#include "ccan.h"

#define RWIO_RECV_STD_ID							0x621
#define RWIO_SEND_STD_ID							0x622
#define RWIO_READ_CMD									0x01
#define RWIO_WRITE_CMD								0x02

#define DI00													GPIO_Pin_2
#define DI01													GPIO_Pin_3
#define DI02													GPIO_Pin_4
#define DI03													GPIO_Pin_5
#define DI04													GPIO_Pin_10
#define DI05													GPIO_Pin_11
#define DI06													GPIO_Pin_12
#define DI07													GPIO_Pin_13
#define DI08													GPIO_Pin_14
#define DI09													GPIO_Pin_15
#define DI_PORT												GPIOB

#define DO00													GPIO_Pin_0
#define DO01													GPIO_Pin_1
#define DO02													GPIO_Pin_2
#define DO03													GPIO_Pin_3
#define DO_PORT0											GPIOA

#define DO04													GPIO_Pin_6
#define DO05													GPIO_Pin_7
#define DO06													GPIO_Pin_8
#define DO07													GPIO_Pin_9
#define DO_PORT1											GPIOB

#define DO10													GPIO_Pin_4
#define DO11													GPIO_Pin_5
#define DO12													GPIO_Pin_6
#define DO13													GPIO_Pin_7
#define DO14													GPIO_Pin_8
#define DO15													GPIO_Pin_9
#define DO_PORT2											GPIOA


namespace{
	const uint8_t rxQeueSize = 8;
	CanRxMsg rxQeueBuf[rxQeueSize];
	CCanRxMailbox _rxMailbox(rxQeueBuf,8);
	uint32_t DO_status = 0x00;
}

struct di_data
{
	unsigned Di00 : 1;
	unsigned Di01 : 1;
	unsigned Di02 : 1;
	unsigned Di03 : 1;
	unsigned Di04 : 1;
	unsigned Di05 : 1;
	unsigned Di06 : 1;
	unsigned Di07 : 1;
	unsigned Di08 : 1;
	unsigned Di09 : 1;
};

union 
{
	struct di_data DiBit;
	uint32_t Di;
}DiData;



void rwIO_io_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DI00|DI01|DI02|DI03|DI04|DI05|DI06|DI07|DI08|DI09;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DI_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DO00|DO01|DO02|DO03;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DO_PORT0, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DO04|DO05|DO06|DO07;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DO_PORT1, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DO10|DO11|DO11|DO13|DO14|DO15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DO_PORT2, &GPIO_InitStructure);
}

void rwIO_do_init(void)
{
	rwIO_io_init();
	_rxMailbox.setStdId(RWIO_RECV_STD_ID);
	_rxMailbox.attachToRouter(CanRouter1);
}

void read_di()
{
	uint32_t tmp = 0;
	tmp = GPIO_ReadInputData(GPIOA);
	DiData.Di = ((tmp >> 2) & 0x0F) + ((tmp >> 10) & 0x3F);
}

void write_do()
{
	
}

void rwIO_do_run()
{
	read_di();
	write_do();
	if(_rxMailbox.msgsInQue() > 0)
	{
		CanRxMsg RxMsg;
		_rxMailbox.getMsg(&RxMsg);
		if(RWIO_READ_CMD == RxMsg.Data[0])								
		{
			CanTxMsg TxMsg;
			TxMsg.StdId = RWIO_SEND_STD_ID;
			TxMsg.ExtId = 0x00;
			TxMsg.DLC = 0x04;
			TxMsg.IDE = CAN_ID_STD;
			TxMsg.RTR = CAN_RTR_DATA;
			TxMsg.Data[0] = 0x01;								//to to
			TxMsg.Data[1] = DiData.Di;
			TxMsg.Data[2] = DiData.Di>>8;
			TxMsg.Data[3] = DiData.Di>>16;
			CanRouter1.putMsg(TxMsg);
		}
		if(RWIO_WRITE_CMD == RxMsg.Data[0])
		{
			DO_status = RxMsg.Data[1];
			DO_status += ((uint32_t)RxMsg.Data[2]<<8);
			DO_status += ((uint32_t)RxMsg.Data[3]<<16);
			
			CanTxMsg TxMsg;
			TxMsg.StdId = RWIO_SEND_STD_ID;
			TxMsg.ExtId = 0x00;
			TxMsg.DLC = 0x04;
			TxMsg.IDE = CAN_ID_STD;
			TxMsg.RTR = CAN_RTR_DATA;
			TxMsg.Data[0] = 0x02;								//to to
			TxMsg.Data[1] = 0x00;
			TxMsg.Data[2] = 0x00;
			TxMsg.Data[3] = 0x00;
			CanRouter1.putMsg(TxMsg);
		}
	}
}
