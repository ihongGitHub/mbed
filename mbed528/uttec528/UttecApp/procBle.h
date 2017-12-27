#ifndef __PROCBLE_H__
#define __PROCBLE_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

class procBle
{
private:
	static DimmerRf* m_pRf;
public:
	procBle(DimmerRf*);
	void bleTask(rfFrame_t*);
};

#endif
