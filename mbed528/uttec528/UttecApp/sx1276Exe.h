#ifndef __SX1276EXE_H__
#define __SX1276EXE_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

class sx1276Exe
{
private:
	bool m_RxDone;
public:
	sx1276Exe();
	bool isSx1276RxDone();
	void clearSx1276RxDone();
};

#endif
