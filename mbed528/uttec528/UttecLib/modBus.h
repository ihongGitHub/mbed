#ifndef __UTTECMODBUS_H__
#define __UTTECMODBUS_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"

typedef enum{
	sofModA=':',
	crModA = 13,
	eofModA= 10,
	modA_FLENGTH=70,
	DeModA_FrameLength=68
} modAscii_t;

typedef struct{
	bool flag;
	bool start;
	uint8_t count;
} modAsciiStatus_t;

typedef struct{
	uint16_t slave;
	uint16_t function;
	uint16_t pData[modA_FLENGTH];
} modAscii_Frame_t;

class modBus
{
private:
	modAscii_Frame_t m_modAFrame;
	static modAsciiStatus_t m_status;
	static Serial* pMySer;

	static bool m_modAsciiDone;
	static uint8_t mModAsciiData[modA_FLENGTH];
	

public:	
	modBus(Serial*);
	modBus();

	bool parseModAscii(uint8_t);

};

#endif
