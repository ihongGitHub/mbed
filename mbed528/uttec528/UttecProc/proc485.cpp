#include <stdio.h>
#include <string.h>

#include "proc485.h"
#include "UttecUtil.h"

static UttecUtil myUtil;

Flash* proc485::mpFlash = NULL;
Flash_t* proc485::mpFlashFrame = NULL;
rfFrame_t* proc485::mp_rfFrame = NULL;
DimmerRf* proc485::pMyRf = NULL;
rs485* proc485::pMy485 = NULL;
UttecBle* proc485::pMyBle = NULL;
mSecExe* proc485::pMy_mSec = NULL;
procServer* proc485::pMyServer = NULL;

proc485::proc485(uttecLib_t pLib, procServer* pServer){
	mpFlash = pLib.pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pLib.pDimmerRf;
	pMy485 = pLib.pRs485;
	pMyBle = pLib.pBle;
	pMy_mSec = pLib.pMsec;
	pMyServer = pServer;
}


void proc485::sendAckBy485(rfFrame_t* pFrame){
	mp_rfFrame->Cmd.Command = edClientAck;
	pMy485->send485(pFrame, eRsUp);
	printf("by485 Up: From %s -> Server\n\r", myUtil.dispRxTx(mp_rfFrame));
}

void proc485::procVolumeCmd(rfFrame_t* pFrame){
	pMy_mSec->m_sensorType = eVolume;
	mp_rfFrame->Ctr.Level = pFrame->Ctr.Level;
	pMy_mSec->sDim.target = pFrame->Ctr.Level/100.0;
	printf("End of procVolumeCmdL %0.3f\n\r",
		pMy_mSec->sDim.target);
}

void proc485::answerReqBy485(rfFrame_t* pFrame){
	switch(pFrame->Cmd.SubCmd){
		case edsControl:
		case edsNewSet:
		case edsPhoto:
		case edsCmd_485NewSet:
		case edsCmd_Alternative:
		case edsPing:
		case edsCmd_Status:
				printf(" 485 Ack From %s -> server \n\r", myUtil.dispRxTx(pFrame));
				sendAckBy485(mp_rfFrame);
			break;
	}
}

void proc485::transferBy485(rfFrame_t* pFrame){
	bool bPass = false;
	bool bReturn = false;
	char cCmd[20]; char cSub[20];
	char cSrc[5]; char cMy[5];
	
	if(myUtil.isMyRxTx(mp_rfFrame, pFrame)) bReturn = true;
	
	switch(mp_rfFrame->MyAddr.RxTx.iRxTx){
		case eMst: bPass = true; break;
		case eGW: 
			if(!myUtil.isNotMyGwGroup(pFrame, mp_rfFrame))
				bPass = true;
			else bReturn = false;
			break;
		case eTx:
			if(pFrame->MyAddr.GroupAddr == mp_rfFrame->MyAddr.GroupAddr)
				bPass = true;
			else bReturn = false;
			break;
		case eRpt:
		case eSRx:
		case eRx:
			if(pFrame->MyAddr.GroupAddr != mp_rfFrame->MyAddr.GroupAddr)
				bReturn = false;
			break;
	}
	if(bReturn){
		pMyServer->taskServer(pFrame);
		answerReqBy485(pFrame);
		return;
	}
	else if(bPass){
		if(myUtil.isTx(mp_rfFrame)){
				printf("byRf: isTx ");
				pMyRf->sendRf(pFrame);	
		}
		myUtil.dispCmdandSub(cCmd, cSub, pFrame);
		printf("by485 Down: , %s, %s\n\r", cCmd, cSub);
		pMy485->send485(pFrame, eRsDown);
	}
	else{
		printf("Not match Src: %s, My: %s\n\r", 
			myUtil.dispRxTx(pFrame), myUtil.dispRxTx(mp_rfFrame));
	}
}

void proc485::rs485Task(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;
	UttecUtil myUtil;
	ulCount++;	
	uint8_t ucCmd = pFrame->Cmd.Command;
	printf("From 485:%d -> ", ucCmd);
	switch(ucCmd){
		case edDummy:
				break;
		case edSensor:
				break;
		case edRepeat:
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
			if(myUtil.isMst(mp_rfFrame)){
				printf("and by485: isMst -> ");
				pMy485->send485(pFrame, eRsDown);
				return;
			}			
			if(myUtil.isGw(mp_rfFrame)&&
				(!myUtil.isNotMyGwGroup(mp_rfFrame,pFrame))){
				printf("and by485: isGw -> ");
				pMy485->send485(pFrame, eRsDown);
				return;
			}			
			if(myUtil.isTx(mp_rfFrame)){
				printf("byRf: isTx ");
				pMyRf->sendRf(pFrame);	
				printf("and by485: isTx -> ");
				pMy485->send485(pFrame, eRsDown);
			}
			procVolumeCmd(pFrame);
				break;
		case edDayLight:
				break;
		case edServerReq:
			transferBy485(pFrame);
				break;
		case edClientAck:
			sendAckBy485(pFrame);
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}
