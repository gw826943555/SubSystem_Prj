#include "CUartCanProbe.h"
#include "stm32f10x.h"
#include "CUartConsole.h"

const uint8_t RX_MAILBOX_QUE_SIZE  = 20;
CanRxMsg rxMailboxBuf[RX_MAILBOX_QUE_SIZE];


/**
  * @brief  Constructor
	* @param  None
  * @retval None
  */
CUartCanProbe::CUartCanProbe()
	:baseCanRouter_(CanRouter250k),
	rxMailbox_(rxMailboxBuf, RX_MAILBOX_QUE_SIZE)
{
	rxMailbox_.setExtId(0x19204);
	rxMailbox_.attachToRouter(baseCanRouter_);
}

/**
  * @brief  getsFromCan
	* @param  None
  * @retval bytes it reads
  */
uint16_t CUartCanProbe::getsFromCan()
{
	uint16_t bytesCnt = 0;
	CanRxMsg rxMsg;
	while(rxMailbox_.msgsInQue() > 0)
	{
		rxMailbox_.getMsg(&rxMsg);
		Console::Instance()->send_array((char*)rxMsg.Data, rxMsg.DLC);
		bytesCnt += rxMsg.DLC;
	}
	return bytesCnt;
}

/**
  * @brief  run
	* @param  None
  * @retval None
  */
void CUartCanProbe::run()
{
	getsFromCan();
}
//end of file
