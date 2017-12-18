#include "mbed.h"
#include "nrf.h"

#include "Flash.h"
#include "DimmerRf.h"

DigitalOut alivenessLED(LED1, 0);

#ifdef my52832
Serial uart(p6, p8);
#else
Serial uart(p9,p11);
#endif

Flash flash;
Ticker secTimer;
Ticker msecTimer;
Timer timer;

PwmOut myPwm(LED2);
PwmOut myPwm1(LED3);
PwmOut myPwm2(LED4);

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
	
//	pFrame->MyAddr.RxTx.iRxTx = eTx;
	
	secTimer.attach(&tickSec, 1);	
	msecTimer.attach(&tickmSec, 0.001);	
	uint32_t ulCount = 0;
	uint32_t ulmSecCount = 0;
	bool mSecToggle = false;
	myPwm2.period_us(1000);
	myPwm1.period_us(1000);
	myPwm.period_us(1000);
	while (true) {
		if(myRf.isRxDone()){
			myRf.clearRxFlag();
			printf("---------Rf Received\n\r");
		}
		if(tick_Sec){
			tick_Sec = false;
			
			alivenessLED = !alivenessLED;
			ulCount++;
			if(pFrame->MyAddr.RxTx.Bit.Tx){
				printf("This is Tx: %d\n\r",ulCount);
				printf("Tx Send Rf Frame to Rx \n\r");
				myRf.sendRf(pFrame);
			}
			else{
				printf("This is Rx: %d\n\r",ulCount);
			}
		}
		if(tick_mSec){
			tick_mSec = false;
			ulmSecCount++;
			if(!(ulmSecCount%1000)) printf("ulmSecCount = %d\n\r",ulmSecCount);
			mSecToggle = !mSecToggle;			
			if(mSecToggle){
				if(!(ulmSecCount%7))
					myPwm = 0.5;
				else 
					myPwm2 = 0.1;
			}
			else{
				myPwm1 = 0.5;
			}
		}
	}
}
