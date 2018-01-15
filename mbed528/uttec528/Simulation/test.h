#ifndef __TEST_H__
#define __TEST_H__

#include <stdint.h>
#include "mbed.h"

#include "uttecLib.h"

#include "procRf.h"
#include "procBle.h"
#include "procSx1276.h"
#include "proc485.h"
#include "procSec.h"
#include "procServer.h"

class test
{
private: 
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
	static DimmerRf* pMyRf;
	static rs485* pMy485;
	static UttecBle* pMyBle;
	static mSecExe* pMy_mSec;
	static procServer* pMyServer;

public:
	test();
	void setTest(uttecLib_t pLib, 
			procServer* pServer);
	void setTestRfData();
	void setTestSxRxData();
	void setTest485Data();
	void setTestBleData();
	void testTicker();
	void setTestReceiveFrameByNum(uint16_t);
	void setTestMyFrameByNum(uint16_t);
};


#endif

