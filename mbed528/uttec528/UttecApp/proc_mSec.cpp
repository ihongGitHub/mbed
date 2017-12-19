#include <stdio.h>
#include <string.h>

#include "proc_mSec.h"

DigitalOut msecLed(p18, 0);
AnalogIn pir(p3);
DimmerRf* proc_mSec::m_pRf = NULL;
Uttec_Sensor_t proc_mSec::m_sensor = {0,};

proc_mSec::proc_mSec(DimmerRf* pRf){
	m_pRf = pRf;
}

bool proc_mSec::procSensor(){
	if(m_sensor.flag) return m_sensor.flag;
	
	m_sensor.current = pir.read();
	if(m_sensor.max<m_sensor.current) m_sensor.max = m_sensor.current;
	else if(m_sensor.min>m_sensor.current) m_sensor.min = m_sensor.current;

	m_sensor.realUpper= m_sensor.max*m_sensor.upper;
	
	m_sensor.average = (m_sensor.max + m_sensor.min)/2.0;	
	if(m_sensor.current>m_sensor.realUpper){
		m_sensor.flag = true;
		m_sensor.saved = m_sensor.current;
	}
	else if(m_sensor.current<(m_sensor.lower+floatZero)){
		m_sensor.flag = true;
		m_sensor.saved = m_sensor.current;
	}
	return m_sensor.flag;
}

void proc_mSec::msecTask(rfFrame_t* pFrame){
	static uint32_t ulCount = 0;	
	ulCount++;	
	if(procSensor()){
		clearSensorFlag();
		printf("Sens = %0.3f\n\r", m_sensor.saved);
	}
	if(!(ulCount%500)){
		printf("Pir Value = %0.3f\n\r", m_sensor.current);
		printf("upper = %0.3f, lower = %0.3f, max = %0.3f, min = %0.3f, avr = %0.3f\n\r",
		m_sensor.upper, m_sensor.lower, m_sensor.max, m_sensor.min, m_sensor.average);
		printf("realUpper = %0.3f, Lower = %0.3f \n\r",
		m_sensor.realUpper, (m_sensor.lower+floatZero));
		
		msecLed = !msecLed;
		if(pFrame->MyAddr.RxTx.Bit.Tx){
//			printf("My Role is Tx in mSec \n\r");
//			m_pRf->sendRf(pFrame);
		}
		else{
//			printf("My Role is Rx in mSec \n\r");
		}
	}
}

void proc_mSec::setSensorLimit(float fLimit){
	m_sensor.flag = false;
	m_sensor.average = 0.5;
	m_sensor.max = 0.5;
	m_sensor.min = 0.5;
	m_sensor.current = 0.5;
	
	m_sensor.upper = m_sensor.average + m_sensor.average - fLimit;
	m_sensor.lower = fLimit;
	m_sensor.rate = fLimit;
}
bool proc_mSec::returnSensorFlag(){
	return m_sensor.flag;
}
void proc_mSec::clearSensorFlag(){
	m_sensor.flag = false;
}

