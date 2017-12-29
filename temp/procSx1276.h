#ifndef __PROCSX1276_H__
#define __PROCSX1276_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

class procSx1276
{
private:
	static DimmerRf* m_pRf;
public:
	procSx1276(DimmerRf*);
	void sx1276Task(rfFrame_t*);
};

#endif
