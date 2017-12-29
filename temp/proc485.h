#ifndef __PROC485_H__
#define __PROC485_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

class proc485
{
private:
	static DimmerRf* m_pRf;
public:
	proc485(DimmerRf*);
	void rs485Task(rfFrame_t*);
};

#endif
