#include <stdio.h>
#include <string.h>

#include "CmdDefine.h"
#include "procRf.h"

Flash* procRf::mpFlash=NULL;
Flash_t* procRf::mpFlashFrame=NULL;
rfFrame_t* procRf::mp_rfFrame=NULL;

procRf::procRf(){

}

void procRf::taskRf(rfFrame_t* pFrame){
	uint8_t ucCmd = pFrame->Cmd.Command;
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
