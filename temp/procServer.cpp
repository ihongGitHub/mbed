#include "procServer.h"

#include "CmdDefine.h"
#include "procRf.h"

#include "UttecUtil.h"
#include "UttecLed.h"

Flash* procServer::mpFlash=NULL;
Flash_t* procServer::mpFlashFrame=NULL;
rfFrame_t* procServer::mp_rfFrame=NULL;
DimmerRf* procServer::pMyRf=NULL;
mSecExe* procServer::pMy_mSec=NULL;

procServer::procServer(Flash* pFlash, DimmerRf* pRf, mSecExe* pMsec){
	mpFlash = pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pRf;
	pMy_mSec = pMsec;
}

void procServer::procControlSub(rfFrame_t* pFrame){
	pMy_mSec->m_sPir.target = pFrame->Ctr.Level/(float)100.0;
	pMy_mSec->sDim.forced = true;
	pMy_mSec->sDim.upStep = pMy_mSec->sDim.downStep = 0;
	UttecUtil myUtil;
	myUtil.testProc(3,1);
}
void procServer::procNewSetSub(rfFrame_t* pFrame){
	pMy_mSec->sDim.forced = false;
	UttecUtil myUtil;
	myUtil.testProc(3,2);
}

void procServer::procNewFactSetSub(rfFrame_t* pFrame){
	Flash myFlash;
	mp_rfFrame->Ctr = pFrame->Ctr;
	myFlash.writeFlash();
	UttecUtil myUtil;
	myUtil.testProc(3,3);
}
void procServer::procAltSub(rfFrame_t* pFrame){
	Flash myFlash;
	if(mp_rfFrame->MyAddr.PrivateAddr%2 ==
		pFrame->MyAddr.PrivateAddr%2){
		pMy_mSec->sDim.forced = true;
		pMy_mSec->m_sPir.target = 0;
		pMy_mSec->sDim.upStep = pMy_mSec->sDim.downStep = 0;		
	}
	UttecUtil myUtil;
	myUtil.testProc(3,4);
}

void procServer::taskServer(rfFrame_t* pFrame){
	UttecUtil myUtil;
	UttecLed myLed;
	
	uint8_t ucCmd = pFrame->Cmd.SubCmd;
	switch(ucCmd){
		case edsPowerReset:	//100
				break;
		case edsPowerRead:
				break;
		case edsMonitor:
				break;
		case edsControl:
			procControlSub(pFrame);
				break;
		case edsNewSet:
			procNewSetSub(pFrame);
				break;
		case edsPing:
				break;
		case edsPhoto:
				break;
		case edsColor:
				break;
		case edsCmd_485NewSet:
			procNewFactSetSub(pFrame);
				break;
		case edsCmd_Alternative:
			procAltSub(pFrame);
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}

void procServer::taskClient(rfFrame_t* pFrame){
	UttecUtil myUtil;
	UttecLed myLed;
	
	uint8_t ucCmd = pFrame->Cmd.Command;
	switch(ucCmd){
		case edsPowerReset:	//100
				break;
		case edsPowerRead:
				break;
		case edsMonitor:
				break;
		case edsControl:
				break;
		case edsNewSet:
				break;
		case edsPing:
				break;
		case edsPhoto:
				break;
		case edsColor:
				break;
		case edsCmd_485NewSet:
				break;
		case edsCmd_Alternative:
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}

