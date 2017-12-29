#include <stdio.h>
#include <string.h>

#include "procSx1276.h"
#include "UttecUtil.h"

Flash* procSx1276::mpFlash = NULL;
Flash_t* procSx1276::mpFlashFrame = NULL;
rfFrame_t* procSx1276::mp_rfFrame = NULL;
DimmerRf* procSx1276::pMyRf = NULL;
rs485* procSx1276::pMy485 = NULL;
sx1276Exe* procSx1276::pMySx1276 = NULL;
UttecBle* procSx1276::pMyBle = NULL;
mSecExe* procSx1276::pMy_mSec = NULL;
procServer* procSx1276::pMyServer = NULL;


procSx1276::procSx1276(uttecLib_t pLib, procServer* pServer){
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

void procSx1276::sx1276Task(rfFrame_t* pFrame){
}
