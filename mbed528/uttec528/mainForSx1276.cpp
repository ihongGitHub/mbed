#include "mbed.h"
#include "nrf.h"
#include "uttecLib.h"

#include "procRf.h"
#include "procBle.h"
#include "proc485.h"
#include "procSec.h"
#include "procServer.h"

#include "UttecUtil.h"
#include "CmdDefine.h"
#include "serial_api.h"
#include "Rcu.h"
#include "test.h"
//#include "bh1750.h"
//#include "Pyd1788.h"
//#include "eprom.h"
//#include "HX711.h"
//#include "procSx1276.h"

#include "simSx.h"

#include "nrf_ble_gap.h"

#ifdef my52832
Serial Uart(p6, p8);
#else
Serial Uart(p9,p11);
#endif

Flash myFlash;
UttecBle myBle;

Timer rcuTimer;

static Ticker secTimer;
static Ticker msecTimer;

static bool tick_Sec = false;
static bool tick_mSec = false;
static bool testTick = false;
#define DeTestTime 3

static void tickSec(){
	static uint32_t ulSecCount = 0;
	ulSecCount++;
	if(!(ulSecCount%DeTestTime)) testTick = true;
	tick_Sec = true;
}
static void tickmSec(){
	tick_mSec = true;
}

//bh1750 my1750;		//100KHz
//Pyd1788 myPyd(p4);
//eprom myEprom;
//HX711 scale;

test myTest;
#include "comErr.h"

int main(void)
{
	ble_gap_addr_t addr;
	for(int i=0; i<sizeof(addr.addr); i++) 
		addr.addr[i] = i*7;
	
	uttecLib_t myLib;
//https://os.mbed.com/handbook/Serial	
	Uart.baud(115200);
	
#ifdef 	my52832
	printf("\n\rNow New nrf52832 2017.12.18 12:50\n\r");
#else
	printf("\n\rNow New nrf51822 2018.01.15 11:00\n\r");
#endif
	
	myFlash.ReadAllFlash();
//	while(1);
	
	Flash_t* pFlash = myFlash.getFlashFrame();
	rfFrame_t* pFrame=&pFlash->rfFrame;
	myFlash.isFactoryMode();
	
//	pFrame->Ctr.DTime = 1;
	printf("My Gid =%d, %f\n\r", pFrame->MyAddr.GroupAddr,
		pFlash->VolumeCheck);

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
*/
//	myFlash.resetFlash();	
	myUtil.setWdt(6);	
	
	Rcu myRcu;	
	myTest.setTest(myLib, &mProcServer);
	while(true){
		myUtil.setWdtReload();
		
		if(mProcSec.m_product.rcu)
		if(myRcu.isRcuReady()){
//		if(myRcu.isRcuReady()&&myRcu.isUttecCode()){
			rcuValue_t myCode;
			myRcu.clearRcuFlag();
			myRcu.returnUtRcuCode(); 
//			myCode = (rcuValue_t)myRcu.returnRcuCode(); 
//			myRcu.procRcu(myCode);
			if(myRcu.isUtRcuReady()){
				myRcu.clearUtRcuFlag();
				rcuFrame_t* pRcu;
				pRcu = myRcu.returnUtRcuCode();
				printf("rcu0: %llx, rcu1: %llx\n\r",
					pRcu->rcu0.ulData, pRcu->rcu1.ulData);
				myRcu.procUtRcu(pRcu);				
			}				
		}
		
		if(mProcSec.m_product.rf)
		if(myRf.isRxDone()){		//For Rf Receive
			myRf.clearRxFlag();
			rfFrame_t* pFrame = myRf.returnRxBuf();
			mProcRf.taskRf(pFrame);			
		}
		
		if(mProcSec.m_product.ble)
		if(myBle.isBleRxReady()){		//For Ble Receive
			myBle.clearBleRxReady();
			mProcBle.bleTask(myBle.getBleRxData());
		}
		
		if(mProcSec.m_product.rs485)
		if(my485.is485Done()){		//For rs485 Receive
			my485.clear485Done();
			mProc485.rs485Task(my485.return485Buf());
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
			if(mProcSx1276.isMyGroup(pFrame, psRf))
				mProcSx1276.sx1276Task(psRf);
		}
		
		if(my_mSec.returnSensorFlag()){		//For sensor Receive
			my_mSec.clearSensorFlag();
			myRf.sendRf(pFrame);
						
//			my485.send485(pFrame);
//			mySx1276.sendSx1276(pFrame);
//			myBle.sendBle(pFrame);
		}
		
		if(tick_mSec){
			tick_mSec = false;
			my_mSec.msecTask(pFrame);
//			putchar('.');
		}
		if(tick_Sec){			
			tick_Sec = false;			
			mProcSec.secTask(pFrame);	
//			myRcu.testRcuGenerate();
		}
		
		if(my485.isTestDone()){		//For test
			my485.clearTestDone();
			uint16_t uiResult = my485.returnTestData();
			printf("Test Data: %d\n\r",uiResult);
			if(uiResult>100) myTest.setTestReceiveFrameByNum(uiResult);
			else myTest.setTestMyFrameByNum(uiResult);
		}		
/*		
		*/
	}
}

/*
			pFrame->MyAddr.SensorType.iSensor = eNoSensor;
//			pFrame->MyAddr.RxTx.iRxTx = eRx;
//			pFrame->MyAddr.RxTx.iRxTx = eTx;
//			pFrame->MyAddr.RxTx.iRxTx = eSRx;
			pFrame->MyAddr.RxTx.iRxTx = eRpt;
			
			sxFrame_t sxFrame;
			sxFrame.gid = 2;
			sxFrame.cmd = edVolume;
			sxFrame.rxtx = eSRx;
			sxFrame.level = 33;

			
			for(int i = 0; i<6; i++) sxFrame.mac[i] = i;
			if(pFrame->MyAddr.RxTx.iRxTx == eTx){
				printf("sendSxFrame for eTx\n\r"); 
				mProcSx1276.sendSxFrame((rfFrame_t*)&sxFrame);
			}
*/
