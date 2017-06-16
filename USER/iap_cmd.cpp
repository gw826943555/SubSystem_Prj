#include "iap_cmd.h"
#include "ccan.h"
#include "timer.h"

#define IAP_RST_ID						0x5005

namespace{
	const uint8_t IAP_CMD[8]={0x09,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
	const uint8_t RX_MAILBOX_QUE_SIZE  = 5;
	CanRxMsg rxMailboxBuf[RX_MAILBOX_QUE_SIZE];
	CCanRxMailbox _rxMailbox(rxMailboxBuf,RX_MAILBOX_QUE_SIZE);
}

void iap_do_init()
{
//	SCB->VTOR=0x08008000;																						//中断地址偏移量
//	__set_PRIMASK(0);																								//允许中断

	_rxMailbox.setExtId(IAP_RST_ID);
	_rxMailbox.attachToRouter(CanRouter1);
}

void iap_do_run()
{
	static bool _is_rst = true;
	static Timer _rst_delay(0,10);
	
	if(_rxMailbox.msgsInQue() > 0)
	{
		CanRxMsg RxMsg;
		_rxMailbox.getMsg(&RxMsg);
		for(uint8_t i=0;i<8;++i)							//
		{
			if(RxMsg.Data[i]!=IAP_CMD[i])							//检查RST命令
				return ;																//不是RST命令，直接返回
		}
		CanTxMsg TxMsg;
		TxMsg.ExtId = 0x5004;
		TxMsg.Data[0] = 0x09;
		TxMsg.Data[1] = 0x00;
		TxMsg.Data[2] = 0x00;
		TxMsg.Data[3] = 0x00;
		TxMsg.IDE = CAN_Id_Extended;
		TxMsg.RTR = CAN_RTR_Data;
		TxMsg.DLC = 4;
		CanRouter1.putMsg(TxMsg);
		_is_rst = false;														//等待复位
		_rst_delay.reset();
	}
	if(false == _is_rst)
	{
		if(_rst_delay.isAbsoluteTimeUp())						//1s复位延时
		{
			NVIC_SystemReset();
		}
	}
}

