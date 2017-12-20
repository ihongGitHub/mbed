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
bool testTick = false;
#define DeTestTime 5

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
int main(void)
{
	UttecUtil myUtil;
	
	uart.baud(115200);
#ifdef 	my52832
	printf("\n\rNow New nrf52832 2017.12.18 12:50\n\r");
#else
	printf("\n\rNow New nrf51822 2017.12.18 12:50\n\r");
#endif
	myUtil.testProc(1,0);
	wait(0.001);
	flash.isFactoryMode();
	Flash_t* pFlash = flash.getFlashFrame();
	rfFrame_t* pFrame=&pFlash->rfFrame;
	printf("My Gid =%d\n\r", pFrame->MyAddr.GroupAddr);

	DimmerRf myRf(&flash);
	myRf.initRfFrame();

	pFrame->MyAddr.RxTx.iRxTx = eSRx;
	pFrame->Ctr.SensorRate = 10;

	secTimer.attach(&tickSec, 1);
	msecTimer.attach(&tickmSec, 0.001);
	uint32_t ulCount = 0;
	uint32_t ulmSecCount = 0;

	procRf rfProc(&myRf);
	rs485 my485;
	procSec mySec(&myRf);
	proc_mSec my_mSec(&myRf);
	my_mSec.setSensorLimit(pFrame->Ctr.SensorRate/100.0);
	rfFrame_t stFrame = *pFrame;
	stFrame.Cmd.Command = edSensor;
	stFrame.MyAddr.RxTx.iRxTx = eSRx;
	
	rfProc.set_procmSec(&my_mSec);
	//flash.resetFlash();
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
			if((pFrame->MyAddr.RxTx.iRxTx == eSRx)&&testTick){
				rfProc.taskRf(&stFrame);
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
