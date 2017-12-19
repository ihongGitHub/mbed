#ifndef __PROCRF_H__
#define __PROCRF_H__

#include <stdint.h>
#include <stdbool.h>

#include "DimmerRf.h"

class procRf
{
private:
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
public:
	procRf();
	void taskRf(rfFrame_t*);
};
#endif
