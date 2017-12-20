#include <stdio.h>
#include <string.h>

#include "CmdDefine.h"
#include "procRf.h"

#include "UttecUtil.h"
#include "UttecLed.h"

Flash* procRf::mpFlash=NULL;
Flash_t* procRf::mpFlashFrame=NULL;
rfFrame_t* procRf::mp_rfFrame=NULL;
DimmerRf* procRf::pMyRf=NULL;
proc_mSec* procRf::pMy_mSec=NULL;

procRf::procRf(DimmerRf* pRf){
	pMyRf = pRf;
//	pRf->mpFlash->getFlashFrame->rfFrame;
}
bool procRf::isTx(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.Tx;
}
bool procRf::isSRx(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.SRx;
}
bool procRf::isRx(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.Rx;
}
bool procRf::isRpt(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.Rpt;
}
bool procRf::isGw(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.GW;
}
bool procRf::isMst(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.Mst;
}
bool procRf::isMstOrGw(rfFrame_t* pFrame){
	return pFrame->MyAddr.RxTx.Bit.Mst||pFrame->MyAddr.RxTx.Bit.GW;
}
void procRf::procRepeatCmd(rfFrame_t* pFrame){
	UttecUtil myUtil;
	printf("procRepeatCmd\n\r");
	myUtil.testProc(2,(uint32_t)pFrame->MyAddr.RxTx.iRxTx);
}
void procRf::procSensorCmd(rfFrame_t* pFrame){
	pMy_mSec->m_sPir.dTime = pFrame->Ctr.DTime*1000;
	pMy_mSec->m_sPir.target = (float)pFrame->Ctr.High/100.0;
}
void procRf::set_procmSec(proc_mSec* pmSec){
	pMy_mSec = pmSec;
}

void procRf::taskRf(rfFrame_t* pFrame){
	UttecUtil myUtil;
	UttecLed myLed;
	
	uint8_t ucCmd = pFrame->Cmd.Command;
	switch(ucCmd){
		case edDummy:
				break;
		case edSensor:
			if(!isMstOrGw(pFrame)){
				procSensorCmd(pFrame);
			}			
				break;
		case edRepeat:
			myUtil.testProc(3,(uint32_t)pFrame->MyAddr.RxTx.iRxTx);
			if(!isMstOrGw(pFrame)){
				procRepeatCmd(pFrame);
			}
			if(isTx(pFrame)){
				while(1){
					printf("Duplicate Tx in this Group\n\r");
					wait(0.5);
					myLed.on(eRfLed);
					myLed.on(eSensLed);
					wait(0.5);
					myLed.off(eRfLed);
					myLed.off(eSensLed);
				}
			}
				break;
		case edLifeEnd:
				break;
		case edNewSet:
				break;
		case edNewSetAck:
				break;
		case edSearch:
				break;
		case edBack:
				break;
		case edAsk:
				break;
		case edVolume:
				break;
		case edDayLight:
				break;
		case edServerReq:
				break;
		case edClientReq:
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}
