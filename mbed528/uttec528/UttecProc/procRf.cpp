#include <stdio.h>
#include <string.h>

#include "CmdDefine.h"
#include "procRf.h"

#include "UttecUtil.h"
#include "UttecLed.h"

Flash* procRf::mpFlash = NULL;
Flash_t* procRf::mpFlashFrame = NULL;
rfFrame_t* procRf::mp_rfFrame = NULL;
DimmerRf* procRf::pMyRf = NULL;
rs485* procRf::pMy485 = NULL;
sx1276Exe* procRf::pMySx1276 = NULL;
UttecBle* procRf::pMyBle = NULL;
mSecExe* procRf::pMy_mSec = NULL;
procServer* procRf::pMyServer = NULL;

procRf::procRf(uttecLib_t pLib, procServer* pServer){
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

procRf::procRf(Flash* pFlash, DimmerRf* pRf, mSecExe* pMsec){
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
	if(isMstOrGw(mp_rfFrame)) return;
	if(isTx(mp_rfFrame)) conflictTx();
	
	printf("procRepeatCmd\n\r");
	pFrame->Ctr.High = mp_rfFrame->Ctr.High;
	pFrame->Ctr.Low = mp_rfFrame->Ctr.Low;
	pFrame->Ctr.Level = mp_rfFrame->Ctr.Level;
	pFrame->Ctr.DTime = mp_rfFrame->Ctr.DTime;
	pFrame->Ctr.Type = mp_rfFrame->Ctr.Type;
	if(isRx(mp_rfFrame)){
		pMy_mSec->m_sensorType = 
			(eSensorType_t)pFrame->MyAddr.SensorType.iSensor;
	}
}
void procRf::procSensorCmd(rfFrame_t* pFrame){
	if(isMstOrGw(mp_rfFrame)) return;
	pMy_mSec->m_sPir.dTime = mp_rfFrame->Ctr.DTime*1000;
	pMy_mSec->m_sPir.target = mp_rfFrame->Ctr.High/100.0;
}

void procRf::procVolumeCmd(rfFrame_t* pFrame){
	UttecUtil myUtil;
	printf("procVolumeCmd\n\r");
	
	if(isMstOrGw(mp_rfFrame)) return;
	if(isTx(mp_rfFrame)) conflictTx();
	
	pMy_mSec->m_sensorType = eVolume;
	mp_rfFrame->Ctr.Level = pFrame->Ctr.Level;
	pMy_mSec->m_sPir.target = pFrame->Ctr.Level/100.0;
}

void procRf::taskRf(rfFrame_t* pFrame){
	UttecUtil myUtil;
	UttecLed myLed;
	
	uint8_t ucCmd = pFrame->Cmd.Command;
	switch(ucCmd){
		case edDummy:
				break;
		case edSensor:
			procSensorCmd(pFrame);
				break;
		case edRepeat:
			procRepeatCmd(pFrame);
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
			procVolumeCmd(pFrame);
				break;
		case edDayLight:
				break;
		case edServerReq:
//			pMyServer->taskServer(pFrame);
				break;
		case edClientReq:
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}

void procRf::conflictTx(){
	UttecLed myLed;
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



