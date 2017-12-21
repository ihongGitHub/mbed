#ifndef __PROCRF_H__
#define __PROCRF_H__

#include <stdint.h>
#include <stdbool.h>

#include "DimmerRf.h"
#include "proc_mSec.h"
#include "server.h"

class procRf
{
private:
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
	static DimmerRf* pMyRf;
	static proc_mSec* pMy_mSec;
	static server* pMyServer;

	bool isMstOrGw(rfFrame_t*);
	bool isMst(rfFrame_t*);
	bool isGw(rfFrame_t*);
	bool isTx(rfFrame_t*);
	bool isSRx(rfFrame_t*);
	bool isRx(rfFrame_t*);
	bool isRpt(rfFrame_t*);
	void conflictTx();
	void procSensorCmd(rfFrame_t*);
	void procRepeatCmd(rfFrame_t*);
	void procVolumeCmd(rfFrame_t*);
public:
	procRf(Flash*, DimmerRf*, proc_mSec*);
	void taskRf(rfFrame_t*);
};
#endif
