
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "test.h"

Flash* test::mpFlash = NULL;
Flash_t* test::mpFlashFrame = NULL;
rfFrame_t* test::mp_rfFrame = NULL;
DimmerRf* test::pMyRf = NULL;
rs485* test::pMy485 = NULL;
UttecBle* test::pMyBle = NULL;
mSecExe* test::pMy_mSec = NULL;
procServer* test::pMyServer = NULL;

test::test(){
}
void test::setTest(uttecLib_t pLib, procServer* pServer){
	mpFlash = pLib.pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pLib.pDimmerRf;
	pMy485 = pLib.pRs485;
	pMyBle = pLib.pBle;
	pMy_mSec = pLib.pMsec;
	pMyServer = pServer;
}

void test::setTestSxRxData(){
}

void test::setTestApproveReq(){
}

void test::setTestApproveAck(){
}

void test::setFindRfData(){
}


