#include <stdio.h>
#include <string.h>

#include "monitor.h"
#include "photoAnalog.h"
#include "mSecExe.h"
#include "UttecUtil.h"
//monitor_t monitor::m_monitor = {0,};

static Timeout monitorT;

static bool bAckTimeoutFlag = true;

static void timeoutM(){
	printf("\r\ntimeoutAck\n\r");
	bAckTimeoutFlag = false;
}

monitor::monitor(){
}

<<<<<<< HEAD
=======
void monitor::askAlive(){
	printf("Monitor ask to %dth member, whether alive\n\r",
		m_monitor.nextNum);
	printf("and process previous Ack result\n\\r");
}
>>>>>>> bbb8c67653aee43815baf561b4d515df7c7b5644

bool monitor::returnMonitor(){
	bool bResult = true;
	mSecExe myMsec;
	photoAnalog myPhoto;
	UttecUtil myUtil;
	myUtil.setWdt(5);
	float photo;
	monitorT.attach(&timeoutM,1);
	bAckTimeoutFlag = true;
	myMsec.setDirectDim(0);
	while(bAckTimeoutFlag){
		myUtil.setWdtReload();
		wait(0.1);
	}
	photo = myPhoto.getPhotoAnalog();
	printf("myPhoto = %f\r\n",photo);
	wait(0.2);
	monitorT.attach(&timeoutM,1);
	bAckTimeoutFlag = true;
	myMsec.setDirectDim(1);
	while(bAckTimeoutFlag){
		myUtil.setWdtReload();
		wait(0.1);
	}
	photo -= myPhoto.getPhotoAnalog();
	printf("difference = %f\r\n",photo);	
	if(photo > 0.2){
		printf("Ok Lamp\r\n");
	}
	else{
		printf("Lamp Error \r\n");
		bResult = false;
	}
	return bResult;
}

