#include <stdio.h>
#include <string.h>

#include "procSx1276.h"
#include "UttecUtil.h"

static UttecUtil myUtil;

Flash* procSx1276::mpFlash = NULL;
Flash_t* procSx1276::mpFlashFrame = NULL;
rfFrame_t* procSx1276::mp_rfFrame = NULL;
DimmerRf* procSx1276::pMyRf = NULL;
rs485* procSx1276::pMy485 = NULL;
sx1276Exe* procSx1276::pMySx1276 = NULL;
UttecBle* procSx1276::pMyBle = NULL;
mSecExe* procSx1276::pMy_mSec = NULL;
procServer* procSx1276::pMyServer = NULL;

sxRxFrame_t procSx1276::m_sxRxFrame = {0,};
sxFrame_t procSx1276::m_sxFrame = {0,};

procSx1276::procSx1276(uttecLib_t pLib, procServer* pServer){
	mpFlash = pLib.pFlash;
	mpFlashFrame = mpFlash->getFlashFrame();
	mp_rfFrame = &mpFlashFrame->rfFrame;
	
	pMyRf = pLib.pDimmerRf;
	pMy485 = pLib.pRs485;
//	UttecSx1276 = pLib.pSx1276;
	pMyBle = pLib.pBle;
	pMy_mSec = pLib.pMsec;
	pMyServer = pServer;
}

void procSx1276::transferMstGwBy485(rfFrame_t* pFrame, UttecDirection_t dir){
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

void procSx1276::procVolumeCmd(rfFrame_t* pFrame){
	UttecUtil myUtil;
	printf("procVolumeCmd\n\r");
		
	pMy_mSec->m_sensorType = eVolume;
	mp_rfFrame->Ctr.Level = pFrame->Ctr.Level;
	pMy_mSec->m_sPir.target = pFrame->Ctr.Level/100.0;
	printf("pMy_mSec->m_sPir.target = %0.3f\n\r",
	pMy_mSec->m_sPir.target);
}

void procSx1276::dispSx1276(){
	printf("cmd:%d, ",m_sxFrame.cmd);
	printf("sub:%d, ",m_sxFrame.sub);
	printf("size:%d, rssi:%d, snr:%d \n\r",
		m_sxRxFrame.size, m_sxRxFrame.rssi, m_sxRxFrame.snr);
}

void procSx1276::setSimulationData(){
	m_sxFrame.gid = 10;
	m_sxFrame.pid = 11;
	m_sxFrame.cmd = edServerReq;
	m_sxFrame.sub = edsControl;
	m_sxRxFrame.ptrBuf = (uint8_t*)&m_sxFrame;
	m_sxRxFrame.rssi = 101;
	m_sxRxFrame.snr = -10;
	m_sxRxFrame.size = sizeof(m_sxFrame);
}

void procSx1276::sendSxFrame(rfFrame_t* pFrame){
	m_sxTxFrame.gid = pFrame->MyAddr.GroupAddr;
	m_sxTxFrame.pid = pFrame->MyAddr.PrivateAddr;
	m_sxTxFrame.cmd = pFrame->Cmd.Command;
	m_sxTxFrame.sub = pFrame->Cmd.SubCmd;
	m_sxTxFrame.sxData = pFrame->Ctr.Level;
	m_sxTxFrame.ext = pFrame->Ctr.High;
	
	sxTxFrame_t sTxFrame;
	sTxFrame.ptrBuf = (uint8_t*)&m_sxTxFrame;
	sTxFrame.size = sizeof(m_sxTxFrame);
	pMySx1276->sendLoRa(sTxFrame);
}

void procSx1276::reformSx2Rf(rfFrame_t* pFrame){
	pFrame->MyAddr.GroupAddr = m_sxFrame.gid;
	pFrame->MyAddr.PrivateAddr = m_sxFrame.pid;
	pFrame->Cmd.Command = m_sxFrame.cmd;
	pFrame->Cmd.SubCmd = m_sxFrame.sub;
	pFrame->Ctr.Level = m_sxFrame.sxData;
	pFrame->Ctr.High = m_sxFrame.ext;
}

void procSx1276::reformRf2Sx(rfFrame_t* pFrame, sxFrame_t* pSx){
	pSx->gid = pFrame->MyAddr.GroupAddr;
	pSx->pid = pFrame->MyAddr.PrivateAddr;
	pSx->cmd = pFrame->Cmd.Command;
	pSx->sub = pFrame->Cmd.SubCmd;
	pSx->sxData = pFrame->Ctr.Level;
	pSx->ext = pFrame->Ctr.High;
}
bool procSx1276::isMyGroup(rfFrame_t* pSrc, rfFrame_t* pMy){
	if(pSrc->MyAddr.GroupAddr == pMy->MyAddr.GroupAddr)
		return true;
	else return false;
}

void procSx1276::sx1276Task(rfFrame_t* pFrame){
	
	uint8_t ucCmd = m_sxFrame.cmd;
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
