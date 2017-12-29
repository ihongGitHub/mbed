#ifndef __PROCSEC_H__
#define __PROCSEC_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

class procSec
{
private:
	static DimmerRf* m_pRf;
public:
	procSec(DimmerRf*);
	void secTask(rfFrame_t*);
};

#endif
