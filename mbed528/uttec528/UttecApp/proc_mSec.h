#ifndef __PROCMSEC_H__
#define __PROCMSEC_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

#define floatZero (float)0.01

typedef struct{
	bool flag;
	float rate;
	float realUpper;
	float upper;
	float lower;
	float max;
	float min;
	float average;
	float current;
	float saved;
} Uttec_Sensor_t;

class proc_mSec
{
private:
	static DimmerRf* m_pRf;
	static Uttec_Sensor_t m_sensor;
	bool procSensor();
public:
	proc_mSec(DimmerRf*);
	void msecTask(rfFrame_t*);
	void setSensorLimit(float);
	bool returnSensorFlag();
	void clearSensorFlag();
};

#endif
