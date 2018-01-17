#ifndef __Rcu_H__
#define __Rcu_H__

#include <stdint.h>
#include "mbed.h"
 
#define DeRcuHeader	13430
#define DeRcu_1	2200
#define DeRcu_0	1100
#define DeRcuRange 	300

#define DeUttecRcuCode 0xa10c

typedef enum{
	rPower = 0xd7, rUp = 0x9b, rDown = 0xbb, rRight = 0x87, rTv = 0x77,
	rLeft = 0xb7, rMenu = 0x07, rScan = 0x3b, rMute = 0xdb,
	rOne = 0xeb, rTwo = 0x6b, rThree = 0xab, rFour = 0x2b,
	rFive = 0xcb, rSix = 0x4b, rSeven = 0x8b, rEight = 0x0b,
	rNine = 0xf3, rZero = 0x73, rPlus = 0x67, rMinus = 0xa7
} rcuValue_t;

typedef struct{
	bool bRcuEnable;
	bool bRcuFlag;
	bool bEnd;
	bool bStart;
	uint32_t m_ulRcuTimeTimeOut;
	uint32_t m_bitCount;
	uint32_t m_RcuData;
} Rcu_t;

class Rcu
{
private: 
	void setClock();
	void setOne();
	void setZero();
	void setHeader();

public:
	Rcu();
	bool isRcuReady();
	bool isUttecCode();
	void  clearRcuFlag();
	uint16_t returnVendorCode();
	uint8_t returnRcuCode();
	void procRcu(rcuValue_t);
	void setRcuPwm();
	void generateRcuSignal();
};


#endif

