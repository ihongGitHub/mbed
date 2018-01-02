#ifndef __MSECEXE_H__
#define __MSECEXE_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

#define floatZero (float)0.01
#define MaxVolumeCount 10000
#define VolumeZeroPoint (float)0.01
//0.2Sec	
#define TimeoutForVolumeRepeat 200 
#define TimeoutForPirRepeat 300 

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
	float pwm;
	uint32_t volumeCount;
} UttecPir_t;

typedef struct{
	bool forced;
	float upStep;
	float downStep;	
} UttecDim_t;


class mSecExe
{
private:
	static DimmerRf* m_pRf;
	static bool m_sensorFlag;

	void switchSensorType(rfFrame_t*);
	bool procPirSensor(rfFrame_t*);
	bool procVolumeSw();
	void setNextRange();
	void monitorSensorFactor();
	float averageSensor(float);
	void procDim(UttecDim_t);
	void setSensorFlag();
public:
	static eSensorType_t m_sensorType;
	static UttecPir_t m_sPir;
	static UttecDim_t sDim;

	mSecExe(DimmerRf*);
	void msecTask(rfFrame_t*);
	void setSensorLimit(float);
	
	bool returnSensorFlag();
	void clearSensorFlag();
	void switchDimType(rfFrame_t*);
};

#endif
