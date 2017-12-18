#include <stdio.h>
#include <string.h>

#include "procRf.h"

Flash* procRf::mpFlash=NULL;
Flash_t* procRf::mpFlashFrame=NULL;
rfFrame_t* procRf::mp_rfFrame=NULL;

procRf::procRf(){

}

void procRf::process(rfFrame_t* pFrame){
	uint8_t ucCmd = pFrame->Cmd.Command;
	switch(ucCmd){
		case 1:
				break;
	}
}
