#include <stdio.h>
#include <string.h>

#include "mSecExe.h"
#include "UttecLed.h"

//DigitalOut rfLed(LED2, 0);
//DigitalOut sensorLed(LED1, 0);
AnalogIn pir(p3);
PwmOut dimer(LED3);

DimmerRf* mSecExe::m_pRf = NULL;
UttecPir_t mSecExe::m_sPir = {0,};
UttecDim_t mSecExe::sDim = {0,};
//ePir, eVolume
eSensorType_t mSecExe::m_sensorType = eVolume;
bool mSecExe::m_sensorFlag = false;
UttecLed myLed;

mSecExe::mSecExe(DimmerRf* pRf){
	m_pRf = pRf;
	dimer.period_us(300);
	dimer = 0.3;
}

#define DeAverageCount 1000
float mSecExe::averageSensor(float fValue){
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

bool mSecExe::procPirSensor(){
	if(m_sPir.flag) return m_sPir.flag;
	
	m_sPir.current = pir.read();
	if(m_sPir.max<m_sPir.current) m_sPir.max = m_sPir.current;
	else if(m_sPir.min>m_sPir.current) m_sPir.min = m_sPir.current;

	m_sPir.realUpper= m_sPir.max*m_sPir.upper;
	averageSensor(m_sPir.current);
	
	m_sPir.average = (m_sPir.max + m_sPir.min)/(float)2.0;	
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

void mSecExe::setNextRange(){
	m_sPir.upper = m_sPir.current + m_sPir.rate;
	m_sPir.lower = m_sPir.current - m_sPir.rate;
	if(m_sPir.upper > m_sPir.max) m_sPir.upper = m_sPir.max;
	if(m_sPir.lower < m_sPir.min) m_sPir.lower = m_sPir.min;
}

bool mSecExe::procVolumeSw(){
	if(m_sPir.flag) return m_sPir.flag;
	
	m_sPir.current = pir.read();	//check range of max, min
	if(m_sPir.max<m_sPir.current) m_sPir.max = m_sPir.current;
	else if(m_sPir.min>m_sPir.current) m_sPir.min = m_sPir.current;
	averageSensor(m_sPir.current);
	
	m_sPir.volumeCount++;
	if(m_sPir.volumeCount == MaxVolumeCount){
		m_sPir.rate = (m_sPir.max - m_sPir.min)/(float)100.0;
		
		printf("------------------Save range:unit = %0.3f, max = %0.3f, min = %0.3f\n\r",
			m_sPir.rate, m_sPir.max, m_sPir.min);
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


void mSecExe::switchSensorType(rfFrame_t* pFrame){
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
void mSecExe::procDim(UttecDim_t sDim){
	static float fNow = 0;
	if(m_sPir.target >= fNow){
		fNow += sDim.upStep;
		if(fNow >= m_sPir.target) fNow = m_sPir.target;
	}
	else{
		fNow -= sDim.downStep;
		if(fNow <= m_sPir.target) fNow = m_sPir.target;
	}
	dimer = (float)1.0 - fNow;
//	dimer = fNow;
}

void mSecExe::switchDimType(rfFrame_t* pFrame){
	if(sDim.forced){	//when forced Mode
		dimer = m_sPir.target;
		return;
	}
	switch(m_sensorType){
		case ePir:
			if(m_sPir.dTime) m_sPir.dTime--;
			else	//when Delay Time out
				m_sPir.target = (float)pFrame->Ctr.Low/(float)100.0;
			sDim.upStep = 0.001;
			sDim.downStep = 0.0003;
			sDim.forced = false;
			procDim(sDim);		
			break;
		case eVolume:
			sDim.upStep = 0.01;
			sDim.downStep = 0.01;
			sDim.forced = false;
			procDim(sDim);		
			break;
		default:
			break;
	}
}
void mSecExe::monitorSensorFactor(){
	Flash flash;
	Flash_t* pFlash = flash.getFlashFrame();
	printf("Pir Value = %0.3f\n\r", m_sPir.current);
	printf("rate = %0.3f, rUpper = %0.3f, Upper = %0.3f, Lower = %0.3f\n\r",
	m_sPir.rate, m_sPir.realUpper, m_sPir.upper, m_sPir.lower);
	printf("max = %0.3f, min = %0.3f, avr = %0.3f, current = %0.3f, target = %0.3f \n\r",
	m_sPir.max, m_sPir.min, m_sPir.average, m_sPir.current, m_sPir.target);	
  printf("Flash Size = %d, float size = %d\n\r", 
		sizeof(Flash_t), sizeof(float));
	printf("Flash float init value = %f\n\r", pFlash->VolumeCheck);
	printf("Gid = %d\n\r", pFlash->rfFrame.MyAddr.GroupAddr);
}
#include "UttecUtil.h"

void mSecExe::msecTask(rfFrame_t* pFrame){
	
	static uint32_t ulCount = 0;	
	ulCount++;
	
//	m_sensorType = ePir;

//	switchSensorType(pFrame);
	switchDimType(pFrame);
	myLed.taskLed();
	if(!(ulCount%500)){
		UttecUtil myUtil;
		dimFactors_t sFactors = {m_sPir.target,sDim.forced, m_sensorType}; 
		
		myUtil.setDimFactor(sFactors);
		myLed.blink(eRfLed, eRfBlink);
		myLed.blink(eSensLed, eSensBlink);
//		if(rfLed) monitorSensorFactor();		
	}
}

void mSecExe::setSensorLimit(float fLimit){
	m_sPir.flag = false;
	m_sPir.average = 0.0;
	m_sPir.max = 0.5;
	m_sPir.min = 0.5;
	m_sPir.current = 0.5;
	
	m_sPir.upper = m_sPir.average + m_sPir.average - fLimit;
	m_sPir.lower = fLimit;
	m_sPir.rate = fLimit;
}
bool mSecExe::returnSensorFlag(){
	return m_sensorFlag;
}
void mSecExe::clearSensorFlag(){
	m_sensorFlag = false;
}

