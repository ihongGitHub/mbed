#include <stdio.h>
#include <string.h>

#include "proc_mSec.h"

DigitalOut msecLed(p18, 0);
AnalogIn pir(p3);
PwmOut dimer(LED3);

DimmerRf* proc_mSec::m_pRf = NULL;
UttecPir_t proc_mSec::m_sPir = {0,};
//ePir, eVolume
eSensorType_t proc_mSec::m_sensorType = eVolume;
bool proc_mSec::m_sensorFlag = false;

proc_mSec::proc_mSec(DimmerRf* pRf){
	m_pRf = pRf;
	dimer.period_us(300);
	dimer = 0.3;
}

#define DeAverageCount 1000
float proc_mSec::averageSensor(float fValue){
	static uint16_t uiCount = 0;
	static float fNow = 0.0;
	if(uiCount < DeAverageCount){
		fNow += fValue;
		uiCount++;
	}
	else{
		fNow = fNow - m_sPir.average;
		fNow = fNow + fValue;
		uiCount = DeAverageCount;
	}
	m_sPir.average = fNow/(float)uiCount; 
	return m_sPir.average;
}

bool proc_mSec::procPirSensor(){
	if(m_sPir.flag) return m_sPir.flag;
	
	m_sPir.current = pir.read();
	if(m_sPir.max<m_sPir.current) m_sPir.max = m_sPir.current;
	else if(m_sPir.min>m_sPir.current) m_sPir.min = m_sPir.current;

	m_sPir.realUpper= m_sPir.max*m_sPir.upper;
	averageSensor(m_sPir.current);
	
	m_sPir.average = (m_sPir.max + m_sPir.min)/2.0;	
	if(m_sPir.current>m_sPir.realUpper){
		m_sPir.flag = true;
		m_sPir.target = m_sPir.current;
	}
	else if(m_sPir.current<(m_sPir.lower+floatZero)){
		m_sPir.flag = true;
		m_sPir.target = m_sPir.current;
	}
	return m_sPir.flag;
}

void proc_mSec::setNextRange(){
	m_sPir.upper = m_sPir.current + m_sPir.rate;
	m_sPir.lower = m_sPir.current - m_sPir.rate;
	if(m_sPir.upper > m_sPir.max) m_sPir.upper = m_sPir.max;
	if(m_sPir.lower < m_sPir.min) m_sPir.lower = m_sPir.min;
}

bool proc_mSec::procVolumeSw(){
	static bool bTestPoint = true;	
	if(m_sPir.flag) return m_sPir.flag;
	
	m_sPir.current = pir.read();	//check range of max, min
	if(m_sPir.max<m_sPir.current) m_sPir.max = m_sPir.current;
	else if(m_sPir.min>m_sPir.current) m_sPir.min = m_sPir.current;
	averageSensor(m_sPir.current);
	
	m_sPir.volumeCount++;
	if(m_sPir.volumeCount == MaxVolumeCount){
		m_sPir.rate = (m_sPir.max - m_sPir.min)/100.0;
		
		printf("------------------Save range:unit = %0.3f, max = %0.3f, min = %0.3f\n\r",
			m_sPir.rate, m_sPir.max, m_sPir.min);
		bTestPoint = false;
	}
	
	if(m_sPir.current >= m_sPir.upper){
		m_sPir.flag = true;
		m_sPir.target = m_sPir.current/(m_sPir.rate*100);
		setNextRange();
	}
	else if(m_sPir.current <= (m_sPir.lower)){
		m_sPir.flag = true;
		m_sPir.target = m_sPir.current/(m_sPir.rate*100);
		setNextRange();
	}
	return m_sPir.flag;
}


void proc_mSec::switchSensorType(rfFrame_t* pFrame){
	switch(m_sensorType){
		case ePir:
			if(procPirSensor()){
				m_sPir.flag = false;
//				clearSensorFlag();
//				printf("Sens = %0.3f\n\r", m_sPir.saved);
			}
			break;
		case eVolume:
			if(procVolumeSw()){
				m_sPir.flag = false;
				clearSensorFlag();
				printf("Sens = %0.3f\n\r", m_sPir.target);
			}
			break;
		default:
			break;
	}
}
void proc_mSec::procDim(UttecDim_t sDim){
	static float fNow = 0;
	if(m_sPir.target >= fNow){
		fNow += sDim.upStep;
		if(fNow >= m_sPir.target) fNow = m_sPir.target;
	}
	else{
		fNow -= sDim.downStep;
		if(fNow <= m_sPir.target) fNow = m_sPir.target;
	}
	dimer = fNow;
}

void proc_mSec::switchDimType(rfFrame_t* pFrame){
	UttecDim_t sDim;
	switch(m_sensorType){
		case ePir:
//			putchar('.');
			if(m_sPir.dTime) m_sPir.dTime--;
			else{
				m_sPir.target = (float)pFrame->Ctr.Low/100.0;
			}
			sDim.upStep = 0.1;
			sDim.downStep = 0.02;
			sDim.direct = false;
			procDim(sDim);		
			break;
		case eVolume:
			sDim.upStep = 0.1;
			sDim.downStep = 0.1;
			sDim.direct = false;
			procDim(sDim);		
			break;
		default:
			break;
	}
}
void proc_mSec::monitorSensorFactor(){
	Flash flash;
	Flash_t* pFlash = flash.getFlashFrame();
	float fTest = 0.0;
	printf("Pir Value = %0.3f\n\r", m_sPir.current);
	printf("rate = %0.3f, rUpper = %0.3f, Upper = %0.3f, Lower = %0.3f\n\r",
	m_sPir.rate, m_sPir.realUpper, m_sPir.upper, m_sPir.lower);
	printf("max = %0.3f, min = %0.3f, avr = %0.3f, current = %0.3f, target = %0.3f \n\r",
	m_sPir.max, m_sPir.min, m_sPir.average, m_sPir.current, m_sPir.target);	
  printf("Flash Size = %d, float size = %d, %d\n\r", 
		sizeof(Flash_t), sizeof(float), sizeof(fTest));
	printf("Flash float init value = %f\n\r", pFlash->VolumeCheck);
	printf("Gid = %d\n\r", pFlash->rfFrame.MyAddr.GroupAddr);
}

void proc_mSec::msecTask(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;	
	ulCount++;
	
	m_sensorType = ePir;

//	switchSensorType(pFrame);
	switchDimType(pFrame);
	if(!(ulCount%500)){		
		msecLed = !msecLed;
//		if(msecLed) monitorSensorFactor();		
		if(pFrame->MyAddr.RxTx.Bit.Tx){
		}
		else{
		}
	}
}

void proc_mSec::setSensorLimit(float fLimit){
	m_sPir.flag = false;
	m_sPir.average = 0.0;
	m_sPir.max = 0.5;
	m_sPir.min = 0.5;
	m_sPir.current = 0.5;
	
	m_sPir.upper = m_sPir.average + m_sPir.average - fLimit;
	m_sPir.lower = fLimit;
	m_sPir.rate = fLimit;
}
bool proc_mSec::returnSensorFlag(){
	return m_sensorFlag;
}
void proc_mSec::clearSensorFlag(){
	m_sensorFlag = false;
}

