#ifndef __UttecUtil_H__
#define __UttecUtil_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Flash.h"
typedef struct{
	float dimValue;
	bool forced;
	uint8_t sensorType;	
} dimFactors_t;

class UttecUtil
{
private:
	static dimFactors_t myDimFact;
public:
	UttecUtil();
	uint16_t gen_crc16(const uint8_t *data, uint16_t size);
	uint8_t Hex2Dec(uint8_t cHex);
	void dispSec(rfFrame_t*);
	void testProc(uint8_t , uint32_t );
	void testProc(uint8_t , uint32_t , uint32_t);
	void testProc(uint8_t , uint32_t , float);
	void setDimFactor(dimFactors_t);
	dimFactors_t getDimFactor();
};
#endif
