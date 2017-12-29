#include "mbed.h"
#include "nrf.h"
#include "uttecLib.h"

/*
#include "Flash.h"
#include "DimmerRf.h"
#include "rs485.h"
#include "sx1276Exe.h"
#include "bleExe.h"
#include "mSecExe.h"

#include "procSx1276.h"
#include "procBle.h"
#include "procRf.h"
#include "procSec.h"

*/

#include "procRf.h"
#include "procBle.h"
#include "procSx1276.h"
#include "proc485.h"
#include "procSec.h"
#include "procServer.h"

#include "UttecUtil.h"
#include "serial_api.h"

#ifdef my52832
Serial Uart(p6, p8);
#else
Serial Uart(p9,p11);
#endif

Flash myFlash;
UttecBle myBle;
sx1276Exe mySx1276;
PwmOut pwm(p22);

Ticker secTimer;
Ticker msecTimer;

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
	uttecLib_t myLib;
	
	uint32_t ulCount = 0;
//https://os.mbed.com/handbook/Serial	
	Uart.baud(115200);
#ifdef 	my52832
	printf("\n\rNow New nrf52832 2017.12.18 12:50\n\r");
#else
	printf("\n\rNow New nrf51822 2017.12.18 12:50\n\r");
#endif
	
	myFlash.isFactoryMode();
	Flash_t* pFlash = myFlash.getFlashFrame();
	rfFrame_t* pFrame=&pFlash->rfFrame;
	printf("My Gid =%d\n\r", pFrame->MyAddr.GroupAddr);

	UttecUtil myUtil;
	
	DimmerRf myRf(&myFlash);
	myRf.initRfFrame(); 
	secTimer.attach(&tickSec, 1);
	msecTimer.attach(&tickmSec, 0.001);
	myUtil.setWdt(3);
	
	mSecExe my_mSec(&myRf);
	rs485 my485(&Uart);
//	mySx1276.initSx1276();
	UttecBle myBle;
	
	myLib.pFlash = &myFlash;
	myLib.pDimmerRf = &myRf;
	myLib.pRx485 = &my485;
	myLib.pSx1276Exe = &mySx1276;
	myLib.pBleExe = &myBle;
	myLib.pMsecEce = &my_mSec;
	
	procServer mProcServer(myLib);
	procRf mProcRf(myLib, &mProcServer);
	procBle mProcBle(myLib, &mProcServer);
	procSx1276 mProcSx1276(myLib, &mProcServer);
	proc485 mProc485(myLib, &mProcServer);
	procSec mProcSec(myLib, &mProcServer);
/*
*/	
	my_mSec.setSensorLimit(pFrame->Ctr.SensorRate/100.0);
	//flash.resetFlash();
	uint8_t ucTest = 0;
	
	while(true){
		myUtil.setWdtReload();
		
		if(myRf.isRxDone()){
			myRf.clearRxFlag();
			mProcRf.taskRf(myRf.returnRxBuf());
		}
		
		if(myBle.isBleRxReady()){
			myBle.clearBleRxReady();
			mProcBle.bleTask(myBle.getBleRxData());
		}
		
		if(my485.is485Done()){
			my485.clear485Done();
			mProc485.rs485Task(my485.return485Buf());
		}
		
		if(mySx1276.isSx1276RxDone()){
			uint8_t ucTemp[10];
			mySx1276.clearSx1276RxDone();
			mProcSx1276.sx1276Task(ucTemp);
		}
		
		if(tick_Sec){
			printf("Sec proc\n\r");
			tick_Sec = false;
			mProcSec.secTask(pFrame);			
		}
	}

	while (true) {
		if(tick_Sec){
			printf("Sec proc\n\r");
			tick_Sec = false;
//			mySec.secTask(pFrame);
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
