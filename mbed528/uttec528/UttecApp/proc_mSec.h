#ifndef __PROCMSEC_H__
#define __PROCMSEC_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

#define floatZero (float)0.01
#define MaxVolumeCount 10000
#define VolumeZeroPoint (float)0.01
	
typedef struct{
	bool flag;
	uint32_t dTime;
	float rate;
	float realUpper;
	float upper;
	float lower;
	float max;
	float min;
	float average;
	float current;
	float target;
	uint32_t volumeCount;
} UttecPir_t;

typedef struct{
	bool direct;
	float upStep;
	float downStep;	
} UttecDim_t;

typedef enum{
	eUp = 0,
	eDown = 1
} UttecDirection_t;

class proc_mSec
{
private:
	static DimmerRf* m_pRf;

	static bool m_sensorFlag;

	void switchSensorType(rfFrame_t*);
	bool procPirSensor();
	bool procVolumeSw();
	void setNextRange();
	void monitorSensorFactor();
	float averageSensor(float);
	void procDim(UttecDim_t);
public:
	static eSensorType_t m_sensorType;
	static UttecPir_t m_sPir;
	proc_mSec(DimmerRf*);
	void msecTask(rfFrame_t*);
	void setSensorLimit(float);
	bool returnSensorFlag();
	void clearSensorFlag();
	void switchDimType(rfFrame_t*);
};

#endif
