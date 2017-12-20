#include <stdio.h>
#include <string.h>

#include "CmdDefine.h"
#include "procRf.h"

Flash* procRf::mpFlash=NULL;
Flash_t* procRf::mpFlashFrame=NULL;
rfFrame_t* procRf::mp_rfFrame=NULL;
DimmerRf* procRf::pMyRf=NULL;
proc_mSec* procRf::pMy_mSec=NULL;

procRf::procRf(DimmerRf* pRf){
	pMyRf = pRf;
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

void procRf::procSensorCmd(rfFrame_t* pFrame){
	pMyRf->sendRf(pFrame);
	pMy_mSec->m_sPir.dTime = pFrame->Ctr.DTime*1000;
	pMy_mSec->m_sPir.target = (float)pFrame->Ctr.High/100.0;
	printf("target = %0.3f\n\r", pMy_mSec->m_sPir.target);
}
void procRf::set_procmSec(proc_mSec* pmSec){
	pMy_mSec = pmSec;
}
void procRf::taskRf(rfFrame_t* pFrame){
	uint8_t ucCmd = pFrame->Cmd.Command;
	switch(ucCmd){
		case edDummy:
				break;
		case edSensor:
			if(!isMstOrGw(pFrame)){
				printf("Not Mst or Gw\n\r");
				procSensorCmd(pFrame);
			}			
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
