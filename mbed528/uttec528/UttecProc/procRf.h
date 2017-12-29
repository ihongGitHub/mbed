#ifndef __PROCRF_H__
#define __PROCRF_H__

#include <stdint.h>
#include <stdbool.h>

#include "uttecLib.h"
#include "procServer.h"

#include "DimmerRf.h"
#include "mSecExe.h"

class procRf
{
private:
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
	static DimmerRf* pMyRf;
	static rs485* pMy485;
	static sx1276Exe* pMySx1276;
	static UttecBle* pMyBle;
	static mSecExe* pMy_mSec;
	static procServer* pMyServer;

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
	procRf(uttecLib_t, procServer*);
	procRf(Flash*, DimmerRf*, mSecExe*);
	void taskRf(rfFrame_t*);
};
#endif
