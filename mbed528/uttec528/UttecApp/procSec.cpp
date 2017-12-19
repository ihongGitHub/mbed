#include <stdio.h>
#include <string.h>

#include "procSec.h"
#include "UttecUtil.h"

DigitalOut aliveLed(p17, 0);
DimmerRf* procSec::m_pRf = NULL;

procSec::procSec(DimmerRf* pRf){
	m_pRf = pRf;
}

void procSec::secTask(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;
	UttecUtil myUtil;
	ulCount++;	
	aliveLed = !aliveLed;
	
	if(pFrame->MyAddr.RxTx.Bit.Tx){
//		printf("My Role is Tx\n\r");
		m_pRf->sendRf(pFrame);
	}
	else{
//		printf("My Role is Rx\n\r");
	}
	myUtil.dispSec(pFrame);
}
