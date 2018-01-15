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

void proc485::transferMstGwBy485(rfFrame_t* pFrame, UttecDirection_t dir){
	if(myUtil.isNotMyGroup(pFrame, mp_rfFrame)&&myUtil.isGw(mp_rfFrame)) return;
	
	char cCmd[20]; char cSub[20];
	myUtil.dispCmdandSub(cCmd, cSub, pFrame);
	if(dir == eDown){
		printf("server ---> gateway, %s, %s\n\r", cCmd, cSub);
		pMy485->send485(pFrame, eRsDown);
	}
	else{
		printf("gateway  ---> server, %s, %s\n\r", cCmd, cSub);
		pMy485->send485(pFrame, eRsUp);
	}
}

void proc485::procVolumeCmd(rfFrame_t* pFrame){
	UttecUtil myUtil;
	printf("procVolumeCmd\n\r");
	
	
	pMy_mSec->m_sensorType = eVolume;
	mp_rfFrame->Ctr.Level = pFrame->Ctr.Level;
	pMy_mSec->sDim.target = pFrame->Ctr.Level/100.0;
	printf("pMy_mSec->m_sPir.target = %0.3f\n\r",
	pMy_mSec->sDim.target);
}

void proc485::rs485Task(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;
	UttecUtil myUtil;
	ulCount++;	
	uint8_t ucCmd = pFrame->Cmd.Command;
	myUtil.testProc(pFrame->Cmd.Command, 2);
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
			if(myUtil.isMstOrGw(mp_rfFrame)){
				transferMstGwBy485(pFrame, eDown);
				return;
			}			
			procVolumeCmd(pFrame);
				break;
		case edDayLight:
				break;
		case edServerReq:
			if(myUtil.isMstOrGw(mp_rfFrame)){
				transferMstGwBy485(pFrame, eDown);
				return;
			}			
			pMyServer->taskServer(pFrame);
				break;
		case edClientAck:
			if(myUtil.isMstOrGw(mp_rfFrame)){
				transferMstGwBy485(pFrame, eUp);
				return;
			}			
				break;
		default:
			printf("Check Cmd %d\n\r", ucCmd);
			break;
	}
}
