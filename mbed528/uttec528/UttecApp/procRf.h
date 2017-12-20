#ifndef __PROCRF_H__
#define __PROCRF_H__

#include <stdint.h>
#include <stdbool.h>

#include "DimmerRf.h"
#include "proc_mSec.h"

class procRf
{
private:
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
	static DimmerRf* pMyRf;
	static proc_mSec* pMy_mSec;

	bool isMstOrGw(rfFrame_t*);
	bool isMst(rfFrame_t*);
	bool isGw(rfFrame_t*);
	bool isTx(rfFrame_t*);
	bool isSRx(rfFrame_t*);
	bool isRx(rfFrame_t*);
	bool isRpt(rfFrame_t*);
	void procSensorCmd(rfFrame_t*);
	void procRepeatCmd(rfFrame_t*);

public:
	procRf(DimmerRf*);
	void taskRf(rfFrame_t*);
	void set_procmSec(proc_mSec*);
};
#endif
