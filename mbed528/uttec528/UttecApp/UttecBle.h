#ifndef __BLEEXE_H__
#define __BLEEXE_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "DimmerRf.h"

class UttecBle
{
private:
	static DimmerRf* m_pRf;
public:
	UttecBle();
};

#endif
