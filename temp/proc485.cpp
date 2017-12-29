#include <stdio.h>
#include <string.h>

#include "proc485.h"
#include "UttecUtil.h"

DimmerRf* proc485::m_pRf = NULL;

proc485::proc485(DimmerRf* pRf){
	m_pRf = pRf;
}

void proc485::rs485Task(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;
	UttecUtil myUtil;
	ulCount++;	
	
	if(pFrame->MyAddr.RxTx.Bit.Tx){
//		printf("My Role is Tx\n\r");
//		m_pRf->sendRf(pFrame);
	}
	else{
//		printf("My Role is Rx\n\r");
	}
	myUtil.dispSec(pFrame);
}
