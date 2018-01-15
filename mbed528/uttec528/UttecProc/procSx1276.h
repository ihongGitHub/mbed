#ifndef __PROCSX1276_H__
#define __PROCSX1276_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "uttecLib.h"
#include "procServer.h"
#include "CmdDefine.h"

#include "simSx.h"

#define DeSx1276SetTimeout	10

class procSx1276
{
private:
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
	static DimmerRf* pMyRf;
	static rs485* pMy485;

	static simSx* pMySx1276;

	static UttecBle* pMyBle;
	static mSecExe* pMy_mSec;
	static procServer* pMyServer;

	void procVolumeCmd(rfFrame_t*);
	void resendByRepeater(rfFrame_t*);
	void transferMstGwBy485(rfFrame_t*, UttecDirection_t);
	void setNewFactor();

	void procSensorCmd(rfFrame_t*);

public:
	procSx1276();
	procSx1276(uttecLib_t, procServer*);
	void sx1276Task(rfFrame_t*);
	void dispSx1276();
	void setSimulationData();
	void sendSxFrame(rfFrame_t*);
	bool isMyGroup(rfFrame_t*, rfFrame_t*);
	rfFrame_t* readSxFrame();
	void searchSx1276(rfFrame_t*);
	void processCmdNewSet(rfFrame_t*);
};

#endif
