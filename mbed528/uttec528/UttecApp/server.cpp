#include "server.h"

#include "CmdDefine.h"
#include "procRf.h"

#include "UttecUtil.h"
#include "UttecLed.h"

Flash* server::mpFlash=NULL;
Flash_t* server::mpFlashFrame=NULL;
rfFrame_t* server::mp_rfFrame=NULL;
DimmerRf* server::pMyRf=NULL;
mSecExe* server::pMy_mSec=NULL;

server::server(Flash* pFlash, DimmerRf* pRf, mSecExe* pMsec){
	mpFlash = pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pRf;
	pMy_mSec = pMsec;
}

void server::procControlSub(rfFrame_t* pFrame){
	pMy_mSec->m_sPir.target = pFrame->Ctr.Level/(float)100.0;
	pMy_mSec->sDim.forced = true;
	pMy_mSec->sDim.upStep = pMy_mSec->sDim.downStep = 0;
	UttecUtil myUtil;
	myUtil.testProc(3,1);
}
void server::procNewSetSub(rfFrame_t* pFrame){
	pMy_mSec->sDim.forced = false;
	UttecUtil myUtil;
	myUtil.testProc(3,2);
}

void server::procNewFactSetSub(rfFrame_t* pFrame){
	Flash myFlash;
	mp_rfFrame->Ctr = pFrame->Ctr;
	myFlash.WriteAllFlash();
	UttecUtil myUtil;
	myUtil.testProc(3,3);
}
void server::procAltSub(rfFrame_t* pFrame){
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

void server::taskServer(rfFrame_t* pFrame){
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

void server::taskClient(rfFrame_t* pFrame){
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

