#ifndef CUART_CAN_PROB_H
#define CUART_CAN_PROB_H
#include "CCan.h"
#include "Singleton.h"

class CUartCanProbe
{
public:		
	CUartCanProbe();
	~CUartCanProbe();
	void run();
	uint16_t getsFromCan();

private:
	CCanRouter& baseCanRouter_;
	CCanRxMailbox rxMailbox_;
};

typedef NormalSingleton<CUartCanProbe> uartCanProbe;

#endif 
//end of file
