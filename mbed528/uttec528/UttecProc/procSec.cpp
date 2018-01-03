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
//eRpt, eSRx, eTx, eRx, eMst, eGw
#define testCount 7

void procSec::testFrame(rfFrame_t* pFrame){
	static bool bSender = true;
	static bool bToggle = false;
	static uint32_t tgCount = 0;
	tgCount++;
	if(!(tgCount%testCount)) bToggle = !bToggle;
	if(bSender){
		pFrame->MyAddr.RxTx.iRxTx = eMst;
		pFrame->MyAddr.SensorType.iSensor = ePir;	
		pFrame->MyAddr.PrivateAddr = 	11; //org 10
		pFrame->Cmd.Command = edVolume;
		if(bToggle)
			pFrame->Cmd.SubCmd = edsControl;
		else
			pFrame->Cmd.SubCmd = edsCmd_Alternative;
			
		pFrame->Ctr.High = 100;
		pFrame->Ctr.Low = 0;
		pFrame->Ctr.Level = 50;
	}
	else{
		pFrame->MyAddr.SensorType.iSensor = ePir;	
		pFrame->MyAddr.RxTx.iRxTx = eRpt;
	}	
}

void procSec::testSxFrame(){
	
}

void procSec::secTask(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;
	UttecUtil myUtil;
	ulCount++;	
	
	testFrame(pFrame);
	if(pFrame->MyAddr.RxTx.Bit.Mst){
		printf("My Role is eMst\n\r");
		if(!(ulCount%testCount)){
			myUtil.testProc(pFrame->Cmd.Command, 0);
			pMy485->send485(pFrame, eRsUp);
//			pMyRf->sendRf(pFrame);
		}
	}
	else{
		printf("My Role is Rx\n\r");
	}
	myUtil.dispSec(pFrame);
}
