#include <stdio.h>
#include <string.h>

#include "proc485.h"
#include "UttecUtil.h"

Flash* proc485::mpFlash = NULL;
Flash_t* proc485::mpFlashFrame = NULL;
rfFrame_t* proc485::mp_rfFrame = NULL;
DimmerRf* proc485::pMyRf = NULL;
rs485* proc485::pMy485 = NULL;
sx1276Exe* proc485::pMySx1276 = NULL;
UttecBle* proc485::pMyBle = NULL;
mSecExe* proc485::pMy_mSec = NULL;
procServer* proc485::pMyServer = NULL;

proc485::proc485(uttecLib_t pLib, procServer* pServer){
	mpFlash = pLib.pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pLib.pDimmerRf;
	pMy485 = pLib.pRs485;
	pMySx1276 = pLib.pSx1276;
	pMyBle = pLib.pBle;
	pMy_mSec = pLib.pMsec;
	pMyServer = pServer;
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
