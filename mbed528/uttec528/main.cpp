#include "mbed.h"
#include "nrf.h"

#include "Flash.h"
#include "DimmerRf.h"
#include "procRf.h"
#include "rs485.h"
#include "procSec.h"
#include "proc_mSec.h"

#ifdef my52832
Serial uart(p6, p8);
#else
Serial uart(p9,p11);
#endif

Flash flash;
Ticker secTimer;
Ticker msecTimer;
Timer timer;

bool tick_Sec = false;
bool tick_mSec = false;
void tickSec(){
	tick_Sec = true;
}
void tickmSec(){
	tick_mSec = true;
}

int main(void)
{
	uart.baud(115200);
	printf("\n\rNow New nrf52832 2017.12.18 12:50\n\r");
#ifdef 	my52832
	printf("My system is nrf52832\n\r");
#else
	printf("My system is nrf51822\n\r");
#endif

	wait(0.001);
	flash.isFactoryMode();
	Flash_t* pFlash = flash.getFlashFrame();
	rfFrame_t* pFrame=&pFlash->rfFrame;
	printf("My Gid =%d\n\r", pFrame->MyAddr.GroupAddr);

	DimmerRf myRf(&flash);
	myRf.initRfFrame();

	pFrame->MyAddr.RxTx.iRxTx = eTx;
	pFrame->Ctr.SensorRate = 10;

	secTimer.attach(&tickSec, 1);
	msecTimer.attach(&tickmSec, 0.001);
	uint32_t ulCount = 0;
	uint32_t ulmSecCount = 0;

	procRf rfProc;
	rs485 my485;
	procSec mySec(&myRf);
	proc_mSec my_mSec(&myRf);
	my_mSec.setSensorLimit(pFrame->Ctr.SensorRate/100.0);
	while (true) {
		if(myRf.isRxDone()){
			myRf.clearRxFlag();
			rfProc.taskRf(myRf.returnRxBuf());
			printf("---------Rf Received\n\r");
		}
		if(my485.is485Done()){
			my485.clear485Done();
			my485.task485(my485.return485Buf());
			printf("---------485 Received\n\r");
		}
		if(tick_Sec){
			tick_Sec = false;
			mySec.secTask(pFrame);
		}
		if(tick_mSec){
			tick_mSec = false;
			my_mSec.msecTask(pFrame);			
		}
	}
}
