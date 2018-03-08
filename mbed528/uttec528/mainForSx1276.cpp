#include "mbed.h"
#include "nrf.h"

#include "UttecUtil.h"
#include "CmdDefine.h"
#include "uttecLib.h"
#include "UttecLed.h"
#include "Rcu.h"
#include "test.h"

#include "procRf.h"
#include "procBle.h"
#include "procSx1276.h"
#include "proc485.h"
#include "procSec.h"

#include "procServer.h"

#include "simSx.h"
#include "nrf_ble_gap.h"

//#include "bh1750.h"
#include "monitor.h"

DigitalIn CTS(p10, PullDown);

UttecBle myBle;

Flash myFlash;
test myTest;

static Ticker secTimer;
static Ticker msecTimer;

static bool tick_Sec = false;
static bool tick_mSec = false;

static void tickSec(){
	static uint32_t ulSecCount = 0;
	ulSecCount++;
	tick_Sec = true;
}
static void tickmSec(){
	tick_mSec = true;
}

int main(void)
{
Serial Uart(p9,p11);
	uttecLib_t myLib;	
	Uart.baud(115200);	
	
	printf("\n\rNow New nrf51822 2018.01.15 11:00\n\r");
	printf("bsl Manual Control\n\r");
	
	ble_gap_addr_t addr;
	for(int i=0; i<sizeof(addr.addr); i++) 
		addr.addr[i] = i*7;
		
	myFlash.ReadAllFlash();	
	Flash_t* pFlash = myFlash.getFlashFrame();
	rfFrame_t* pMyFrame=&pFlash->rfFrame;
	
	myFlash.isFactoryMode();
	
	printf("My Gid =%d, %f\n\r", pMyFrame->MyAddr.GroupAddr,
		pFlash->VolumeCheck);
	printf("high =%d, low =%d\n\r", pMyFrame->Ctr.High,
		pMyFrame->Ctr.Low);

	UttecUtil myUtil;
	DimmerRf myRf(&myFlash);
	myRf.initRfFrame(); 
	secTimer.attach(&tickSec, 1);
	msecTimer.attach(&tickmSec, 0.001);
		
	mSecExe my_mSec(&myRf);
	rs485 my485(&Uart);	
//	mySx1276.initSx1276(0);
	UttecBle myBle;
	
	myLib.pFlash = &myFlash;
	myLib.pDimmerRf = &myRf;
	myLib.pRs485 = &my485;
//	myLib.pSx1276 = &mySx1276;
	myLib.pBle = &myBle;
	myLib.pMsec = &my_mSec;
	
simSx mySim(&myRf);	

	procServer mProcServer(myLib);
	procRf mProcRf(myLib, &mProcServer);
	procBle mProcBle(myLib, &mProcServer);
	proc485 mProc485(myLib, &mProcServer);
	procSec mProcSec(myLib, &mProcServer);
	procSx1276 mProcSx1276(myLib, &mProcServer);
/*
	myFlash.resetFlash();	
	while(1);
*/
	
	myUtil.setWdt(6);	
	Rcu myRcu;	
	myTest.setTest(myLib, &mProcServer);
	
//	pFrame->MyAddr.SensorType.iSensor = eNoSensor;
//	pMyFrame->MyAddr.SensorType.iSensor = ePir;	//ePir, eDayLight
UttecLed myLed;
	
	while(true){
		myUtil.setWdtReload();
		
/*		
		*/
		if(mProcSec.m_product.rcu)
		if(myRcu.isRcuReady()){
			rcuValue_t myCode;
			myRcu.clearRcuFlag();
			myCode = (rcuValue_t)myRcu.returnRcuCode(); 
//			pMyFrame->MyAddr.RxTx.iRxTx = myRcu.forTest(myCode);
//			myRcu.procRcu(myCode);
		}

		if(mProcSec.m_product.rf)
		if(myRf.isRxDone()){		//For Rf Receive
			myLed.blink(eRfLed, eRfBlink);
			myRf.clearRxFlag();
			rfFrame_t* pFrame = myRf.returnRxBuf();
			myUtil.dispCmd(pFrame);
			mProcRf.taskRf(pFrame);			
		}
		
		if(mProcSec.m_product.sx1276)
		if(mySim.isSxRxDone()){		//For sx1276 Receive
			printf("-------------isSxRxDone\n\r");
			mySim.clearSxRxFlag();
			rfFrame_t* psRf = mProcSx1276.readSxFrame();
			sxFrame_t* psx = (sxFrame_t*)psRf;
			printf("gid = %d, %d\n\r", psRf->MyAddr.GroupAddr, 
				psRf->Ctr.Level);
			printf("\n\r");
			if(mProcSx1276.isMyGroup(pMyFrame, psRf))
				mProcSx1276.sx1276Task(psRf);
		}
		
		if(mProcSec.m_product.rs485)
		if(my485.is485Done()){		//For rs485 Receive			
			my485.clear485Done();
			rfFrame_t* p485Frame = my485.return485Buf();
//			mProc485.rs485Task(p485Frame);
			my485.send485(pMyFrame, eRsDown);
		}		
				
		if(my_mSec.returnSensorFlag()){		//For sensor Receive
			my_mSec.clearSensorFlag();
			if(!myUtil.isRx(pMyFrame))
				myRf.sendRf(pMyFrame);
						
//			my485.send485(pFrame);
//			mySx1276.sendSx1276(pFrame);
//			myBle.sendBle(pFrame);
		}
		
		if(tick_mSec){
			myLed.alarm();
			tick_mSec = false;
			my_mSec.msecTask(pMyFrame);
		}
		
		if(tick_Sec){	
			
			tick_Sec = false;			
			mProcSec.secTask(pMyFrame);	
			
			
			/*			
			static uint32_t ulCount = 0;
			static uint32_t ulNext = 0;
			static uint32_t ucMode = 0;
			uint8_t ucConst = 10;
			uint8_t ucNext=ucConst;
			if(ulCount == ulNext){
				ulNext += ucNext;
				ucMode++;
				myLed.setAlarmTime(500);
			}
			printf("Mode = %d, Next = %d, target = %f, pwm = %f\r\n", ucMode%7,
			 ucNext, my_mSec.sDim.target, my_mSec.sDim.current);
			switch(ucMode%7){
				case 0:
						ucNext = ucConst;
						my_mSec.sDim.target = 1.0;
					break;
				case 1:
						ucNext = ucConst;
						my_mSec.sDim.target = 0.1;
					break;
				case 2:
						ucNext = ucConst;
						my_mSec.sDim.target = 0.7;
					break;
				case 3:
						ucNext = ucConst;
						my_mSec.sDim.target = 0.3;
					break;
				case 4:
						ucNext = ucConst;
						my_mSec.sDim.target = 1.0;
					break;
				case 5:
						ucNext = ucConst;
						my_mSec.sDim.target = 0.0;
					break;
				case 6:
						ucNext = ucConst;
						my_mSec.sDim.target = 0.5;
					break;
			}
			ulCount++;
			monitor myM;
			myM.returnMonitor();
			bTest = !bTest;
			if(bTest) myLed.setAlarmTime(500);
			if(myM.getTrafficCountFlag()){ 
				printf("Traffic = %d\r\n", myM.getTraffic());
			}
			printf("high:%d, low:%d, rxtx:%d\r\n", pMyFrame->Ctr.High,
			pMyFrame->Ctr.Low, pMyFrame->MyAddr.RxTx.iRxTx);
			*/
		}		
	}
}

void temp(){
	while(1){
		printf("Test\r\n");
		wait(1);
	}
}

/*		
//#include "bh1750.h"
//#include "Pyd1788.h"
//#include "eprom.h"
//#include "HX711.h"
//#include "procSx1276.h"

//bh1750 my1750;		//100KHz
//Pyd1788 myPyd(p4);
//eprom myEprom;
//HX711 scale;


		if(my485.isAnyDone()){
			if(my485.is485Done()){		//For rs485 Receive
				my485.clear485Done();
				printf("\n\ris485Done\n\r");
				mProc485.rs485Task(my485.return485Buf());
			}		

		if(my485.isAnyDone()){		//For Test
			uint32_t ulMyAdd = 50000;
			uint32_t ulYourAdd = 60000;
			if(my485.isTestDone()){		//For test
				my485.clearTestDone();
				uint32_t uiResult = my485.returnTestData();
				printf("Test Data: %d\n\r",uiResult);
				if(uiResult<ulMyAdd) myTest.setTestReceiveFrameByNum(uiResult);
				else if((uiResult<ulYourAdd)&&(uiResult>=ulMyAdd)) myTest.setTestMyAddr(uiResult);
				else myTest.setTestYourAddr(uiResult);
			}					
		}
		
		if(myRcu.isRcuReady()&&myRcu.isUttecCode()){
			myRcu.returnUtRcuCode(); 
			if(myRcu.isUtRcuReady()){
				myRcu.clearUtRcuFlag();
				rcuFrame_t* pRcu;
				pRcu = myRcu.returnUtRcuCode();
				printf("rcu0: %llx, rcu1: %llx\n\r",
					pRcu->rcu0.ulData, pRcu->rcu1.ulData);
				myRcu.procUtRcu(pRcu);				
			}
*/
