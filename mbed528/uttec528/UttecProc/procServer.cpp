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
sx1276Exe* procServer::pMySx1276 = NULL;
UttecBle* procServer::pMyBle = NULL;
mSecExe* procServer::pMy_mSec = NULL;

procServer::procServer(uttecLib_t pLib){
	mpFlash = pLib.pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pLib.pDimmerRf;
	pMy485 = pLib.pRs485;
	pMySx1276 = pLib.pSx1276;
	pMyBle = pLib.pBle;
	pMy_mSec = pLib.pMsec;
}
void procServer::resendByRepeater(rfFrame_t* pFrame){
	if(myUtil.isRpt(mp_rfFrame)){	//Repeat Function
		myUtil.testProc(pFrame->Cmd.Command, 3);
		if(!myUtil.isRpt(pFrame)){	//From Rpt?
			pFrame->MyAddr.RxTx.iRxTx = eRpt;
			printf("resend by repeater\n\r");
			myUtil.testProc(pFrame->Cmd.Command,
				(uint32_t)pFrame->Cmd.SubCmd, (uint32_t)4);
			pMyRf->sendRf(pFrame);	
		}			
	}
}

void procServer::procControlSub(rfFrame_t* pFrame){
	pMy_mSec->m_sPir.target = pFrame->Ctr.Level/(float)100.0;
	pMy_mSec->sDim.forced = true;
	pMy_mSec->sDim.upStep = pMy_mSec->sDim.downStep = 0;
	myUtil.testProc(pFrame->Cmd.Command, 
		(uint32_t)pFrame->Cmd.SubCmd, (uint32_t)3);
	resendByRepeater(pFrame);
}
void procServer::procNewSetSub(rfFrame_t* pFrame){
	pMy_mSec->sDim.forced = false;
	myUtil.testProc(pFrame->Cmd.SubCmd,2);
	myUtil.testProc(pFrame->Cmd.Command, 
		(uint32_t)pFrame->Cmd.SubCmd, (uint32_t)3);
	resendByRepeater(pFrame);
}

void procServer::procNewFactSetSub(rfFrame_t* pFrame){
	Flash myFlash;
	mp_rfFrame->Ctr = pFrame->Ctr;
	myFlash.writeFlash();
	myUtil.testProc(pFrame->Cmd.SubCmd,2);
	myUtil.testProc(pFrame->Cmd.Command, 
		(uint32_t)pFrame->Cmd.SubCmd, (uint32_t)3);
	resendByRepeater(pFrame);
}
void procServer::procAltSub(rfFrame_t* pFrame){
	Flash myFlash;
	if(mp_rfFrame->MyAddr.PrivateAddr%2 ==
		pFrame->MyAddr.PrivateAddr%2){
		pMy_mSec->sDim.forced = true;
		pMy_mSec->m_sPir.target = 0;
		pMy_mSec->sDim.upStep = pMy_mSec->sDim.downStep = 0;		
	}
	myUtil.testProc(pFrame->Cmd.Command, 
		(uint32_t)pFrame->Cmd.SubCmd, (uint32_t)3);
	resendByRepeater(pFrame);
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

