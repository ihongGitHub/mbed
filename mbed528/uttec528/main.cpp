#include "mbed.h"
#include "nrf.h"

#include "Flash.h"
#include "DimmerRf.h"
#include "procRf.h"
#include "rs485.h"
#include "procSec.h"
#include "proc_mSec.h"
#include "UttecUtil.h"

#include "serial_api.h"

#ifdef my52832
//Serial uart(p13, p8);
Serial uart(p6, p8);
//Serial uart(LED4, p8);
#else
Serial uart(p9,p11);
#endif

Flash flash;
Ticker secTimer;
Ticker msecTimer;
Timer timer;

bool tick_Sec = false;
bool tick_mSec = false;
bool testTick = false;
#define DeTestTime 3

void tickSec(){
	static uint32_t ulSecCount = 0;
	ulSecCount++;
	if(!(ulSecCount%DeTestTime)) testTick = true;
	tick_Sec = true;
}
void tickmSec(){
	tick_mSec = true;
}
#include "CmdDefine.h"
#include "serial_api.h"
int main(void)
{
	UttecUtil myUtil;
	uint32_t ulCount = 0;
//https://os.mbed.com/handbook/Serial	
	uart.baud(115200);
#ifdef 	my52832
	printf("\n\rNow New nrf52832 2017.12.18 12:50\n\r");
#else
	printf("\n\rNow New nrf51822 2017.12.18 12:50\n\r");
#endif
	flash.isFactoryMode();
	Flash_t* pFlash = flash.getFlashFrame();
	rfFrame_t* pFrame=&pFlash->rfFrame;
	printf("My Gid =%d\n\r", pFrame->MyAddr.GroupAddr);

	DimmerRf myRf(&flash);
	myRf.initRfFrame();

	secTimer.attach(&tickSec, 1);
	msecTimer.attach(&tickmSec, 0.001);

	rs485 my485(&uart);
	procSec mySec(&myRf);
	proc_mSec my_mSec(&myRf);
	procRf rfProc(&flash, &myRf, &my_mSec);
	
	my_mSec.setSensorLimit(pFrame->Ctr.SensorRate/100.0);
	//flash.resetFlash();
	uint8_t ucTest = 0;
	while (true) {
		if(myRf.isRxDone()){
			myRf.clearRxFlag();
			rfProc.taskRf(myRf.returnRxBuf());
		}
		if(my485.is485Done()){
			my485.clear485Done();
			my485.task485(my485.return485Buf());
		}
		if(tick_Sec){
			tick_Sec = false;
			mySec.secTask(pFrame);
			my485.send485(pFrame);
//		pFrame->MyAddr.RxTx.iRxTx = eTx;
		pFrame->MyAddr.SensorType.iSensor = eVolume;	
		pFrame->MyAddr.PrivateAddr = 	11; //org 10
		pFrame->Cmd.Command = edServerReq;
		pFrame->Cmd.SubCmd = edsControl;
		pFrame->Ctr.Level = ucTest++;
		static bool bCmd = false;	
		if(ucTest > 100) ucTest = 0;		
			if((pFrame->MyAddr.RxTx.iRxTx == eTx)&&testTick){
				bCmd = !bCmd;
				if(bCmd) 
					pFrame->Cmd.SubCmd = edsControl;
				else
					pFrame->Cmd.SubCmd = edsCmd_Alternative;
					
				myUtil.testProc(1,0);
				myRf.sendRf(pFrame);
				testTick = false;
			}
		}
		if(tick_mSec){
			tick_mSec = false;
			my_mSec.msecTask(pFrame);		
			if(my_mSec.returnSensorFlag()){
			}				
		}
	}
}
