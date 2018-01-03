#include "mbed.h"
#include "nrf.h"
#include "uttecLib.h"

#include "procRf.h"
#include "procBle.h"
#include "procSx1276.h"
#include "proc485.h"
#include "procSec.h"
#include "procServer.h"

#include "UttecUtil.h"
#include "CmdDefine.h"
#include "serial_api.h"


#ifdef my52832
Serial Uart(p6, p8);
#else
Serial Uart(p9,p11);
#endif

Flash myFlash;
UttecBle myBle;
sx1276Exe mySx1276;

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

int main(void)
{
	uttecLib_t myLib;
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
		
	mSecExe my_mSec(&myRf);
	rs485 my485(&Uart);
	
	UttecBle myBle;
	
	myLib.pFlash = &myFlash;
	myLib.pDimmerRf = &myRf;
	myLib.pRs485 = &my485;
	myLib.pSx1276 = &mySx1276;
	myLib.pBle = &myBle;
	myLib.pMsec = &my_mSec;
	
	procServer mProcServer(myLib);
	procRf mProcRf(myLib, &mProcServer);
	procBle mProcBle(myLib, &mProcServer);
	procSx1276 mProcSx1276(myLib, &mProcServer);
	proc485 mProc485(myLib, &mProcServer);
	procSec mProcSec(myLib, &mProcServer);
/*
*/
	pFrame->Ctr.SensorRate = 1;
	my_mSec.setSensorLimit(pFrame->Ctr.SensorRate/100.0);
	//flash.resetFlash();
	mySx1276.initSx1276();
	
	myUtil.setWdt(3);
	mProcSx1276.setSimulationData();
	
	pFrame->MyAddr.RxTx.iRxTx = eTx;
	while(true){
		myUtil.setWdtReload();
		
		if(myRf.isRxDone()){		//For Rf Receive
			myRf.clearRxFlag();
			rfFrame_t* pFrame = myRf.returnRxBuf();
			myUtil.testProc(pFrame->Cmd.Command, 1);
			mProcRf.taskRf(pFrame);			
		}
		
		if(myBle.isBleRxReady()){		//For Ble Receive
			myBle.clearBleRxReady();
			mProcBle.bleTask(myBle.getBleRxData());
		}
		
		if(my485.is485Done()){		//For rs485 Receive
			my485.clear485Done();
			mProc485.rs485Task(my485.return485Buf());
		}		
/*		
		if(mySx1276.isSxRxReady()){		//For sx1276 Receive
			printf("-------------isSxRxReady\n\r");
			rfFrame_t sxRfFrame = *pFrame;
			sxRxFrame_t* psxRxFrame = mySx1276.readLoRa();
			mProcSx1276.m_sxFrame = *(sxFrame_t*)psxRxFrame->ptrBuf;
			mProcSx1276.reformSx2Rf(&sxRfFrame);
			mySx1276.clearSxRxFlag();
			
			if(mProcSx1276.isMyGroup(&sxRfFrame, pFrame))
				mProcSx1276.sx1276Task(&sxRfFrame);
		}
		*/
		if(mySx1276.isSxRxReady()){		//For sx1276 Receive
			rfFrame_t sxRfFrame = *pFrame;
			mProcSx1276.setSxRxData(mySx1276.readLoRa());
			printf("-----------i received readLoRa: \n\r");
			mProcSx1276.dispSx1276();
			
			mProcSx1276.reformSx2Rf(&sxRfFrame, &mProcSx1276.m_sxFrame);
			mySx1276.clearSxRxFlag();
				if(!myUtil.isTx(pFrame))
					printf("Resend rfFrame\n\r");
					mProcSx1276.sendSxFrame(&sxRfFrame);
		}

		if(my_mSec.returnSensorFlag()){		//For sensor Receive
			my_mSec.clearSensorFlag();
//			myRf.sendRf(pFrame);
//			my485.send485(pFrame);
//			mySx1276.sendSx1276(pFrame);
//			myBle.sendBle(pFrame);
		}
		
		if(tick_mSec){
			tick_mSec = false;
			my_mSec.msecTask(pFrame);
		}
		
		if(tick_Sec){
			printf("Sec proc\n\r");
			tick_Sec = false;			
			mProcSec.secTask(pFrame);			
			mProcSx1276.dispSx1276();
			mProcSx1276.setSimulationData();
			if(myUtil.isTx(pFrame)){
				pFrame->Cmd.Command = edServerReq;
				pFrame->Cmd.SubCmd = edsControl;
				mProcSx1276.sendSxFrame(pFrame);
			}
		}
	}
}
