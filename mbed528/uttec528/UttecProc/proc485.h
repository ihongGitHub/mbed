#ifndef __PROC485_H__
#define __PROC485_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "uttecLib.h"
#include "procServer.h"

class proc485
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
public:
	proc485(uttecLib_t, procServer*);
	void rs485Task(rfFrame_t*);
};

#endif
