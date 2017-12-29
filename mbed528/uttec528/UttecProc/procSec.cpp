#include <stdio.h>
#include <string.h>

#include "procSec.h"
#include "UttecUtil.h"

Flash* procSec::mpFlash = NULL;
Flash_t* procSec::mpFlashFrame = NULL;
rfFrame_t* procSec::mp_rfFrame = NULL;
DimmerRf* procSec::pMyRf = NULL;
rs485* procSec::pMy485 = NULL;
sx1276Exe* procSec::pMySx1276 = NULL;
UttecBle* procSec::pMyBle = NULL;
mSecExe* procSec::pMy_mSec = NULL;
procServer* procSec::pMyServer = NULL;

procSec::procSec(uttecLib_t pLib, procServer* pServer){
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

void procSec::secTask(rfFrame_t* pFrame){
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
