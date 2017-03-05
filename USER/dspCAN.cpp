#include "dspCAN.h"
#include "bsp_CAN.h"
#include "CUartConsole.h"
#include "CSysTick.h"
void task_CAN_run()
{
	CanTxMsg TxMessage;
	TxMessage.IDE = CAN_ID_STD;   //Set ID type as standard
	TxMessage.RTR = CAN_RTR_DATA;	//Set the frame as data 
	TxMessage.DLC = 6;			      // data length 1 byte
	
	if (CAN_MessagePending(CAN1, CAN_FIFO_L) != 0) 
	{
		Console::Instance()->printf("CAN_L received...\r\n");
		CAN_FIFORelease(CAN1, CAN_FIFO_L);
				
		TxMessage.StdId = 0x0001;     //Set the standard ID (11 bits)

		TxMessage.Data[5] = 0;		  // the 1st byte data
		TxMessage.Data[4] = 1;		  // the 2st byte data
		TxMessage.Data[3] = 81;		  // the 3st byte data
		TxMessage.Data[2] = 1;		  // the 4st byte data
		TxMessage.Data[1] = 0;		  // the 5st byte data
		TxMessage.Data[0] = 0;		  // the 6st byte data
		CAN_Transmit(CAN1,&TxMessage);	//start to transmit
	}
	
	if (CAN_MessagePending(CAN1, CAN_FIFO_R) != 0)
	{
		Console::Instance()->printf("CAN_R received...\r\n");
		CAN_FIFORelease(CAN1, CAN_FIFO_R);
		
		TxMessage.StdId = 0x0003;     //Set the standard ID (11 bits)

		TxMessage.Data[5] = 0;		  // the 1st byte data
		TxMessage.Data[4] = 1;		  // the 2st byte data
		TxMessage.Data[3] = 61;		  // the 3st byte data
		TxMessage.Data[2] = 0;		  // the 4st byte data
		TxMessage.Data[1] = 0;		  // the 5st byte data
		TxMessage.Data[0] = 0;		  // the 6st byte data
		CAN_Transmit(CAN1,&TxMessage);	//start to transmit
	}
}
// end of file
