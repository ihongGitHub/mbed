#include "procServer.h"

#include "CmdDefine.h"
#include "procRf.h"

#include "UttecUtil.h"
#include "UttecLed.h"

static UttecUtil myUtil;

Flash* procServer::mpFlash = NULL;
Flash_t* procServer::mpFlashFrame = NULL;
rfFrame_t* procServer::mp_rfFrame = NULL;
DimmerRf* procServer::pMyRf = NULL;
rs485* procServer::pMy485 = NULL;
UttecBle* procServer::pMyBle = NULL;
mSecExe* procServer::pMy_mSec = NULL;

procServer::procServer(uttecLib_t pLib){
	mpFlash = pLib.pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pLib.pDimmerRf;
	pMy485 = pLib.pRs485;
	pMyBle = pLib.pBle;
	pMy_mSec = pLib.pMsec;
}
void procServer::procControlSub(rfFrame_t* pFrame){
	pMy_mSec->setForcedDim(pFrame->Ctr.Level/(float)100.0);
	printf("procControlSub: Level = %d\n\r",pFrame->Ctr.Level);
}
void procServer::procNewSetSub(rfFrame_t* pFrame){
	pMy_mSec->sDim.forced = false;
	printf("procNewSetSub: Level = %d\n\r",pFrame->Ctr.Level);
}

void procServer::procNewFactSetSub(rfFrame_t* pFrame){
	Flash myFlash;
	mp_rfFrame->Ctr = pFrame->Ctr;
	myFlash.writeFlash();
	printf("procNewFactSetSub: Level = %d\n\r",pFrame->Ctr.Level);
}
void procServer::procAltSub(rfFrame_t* pFrame){
	Flash myFlash;
	if(mp_rfFrame->MyAddr.PrivateAddr%2 ==
		pFrame->MyAddr.PrivateAddr%2){
		pMy_mSec->setForcedDim(pFrame->Ctr.Level/(float)100.0);
	}
	printf("procAltSub: Level = %d\n\r",pFrame->Ctr.Level);
}

void procServer::procStatus(rfFrame_t* pFrame){
	printf("Return Status tbd\n\r");
	printf("procStatus: Level = %d\n\r",pFrame->Ctr.Level);
}

void procServer::taskServer(rfFrame_t* pFrame){
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
			printf("edsPing tbd\n\r");
				break;
		case edsPhoto:
			printf("edsPhoto return Photo Value\n\r");
				break;
		case edsColor:
				break;
		case edsCmd_485NewSet:
			procNewFactSetSub(pFrame);
				break;
		case edsCmd_Alternative:
			procAltSub(pFrame);
				break;
		case edsCmd_Status:
			procStatus(pFrame);
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}

void procServer::taskClient(rfFrame_t* pFrame){
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

