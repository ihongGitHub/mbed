#ifndef __PROCSX1276_H__
#define __PROCSX1276_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "nrf.h"
#include "uttecLib.h"
#include "procServer.h"
#include "CmdDefine.h"

typedef struct{
	uint8_t gid;
	uint8_t pid;
	uint8_t cmd;
	uint8_t sub;
	uint8_t sxData;
	uint8_t ext;
} sxFrame_t;

class procSx1276
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

	sxFrame_t m_sxTxFrame;

	void procVolumeCmd(rfFrame_t*);
	void resendByRepeater(rfFrame_t*);
	void transferMstGwBy485(rfFrame_t*, UttecDirection_t);
public:
	static sxRxFrame_t m_sxRxFrame;
	static sxFrame_t m_sxFrame;

	procSx1276(uttecLib_t, procServer*);
	void sx1276Task(rfFrame_t*);
	void dispSx1276();
	void setSimulationData();
	void sendSxFrame(rfFrame_t*);
	void reformSx2Rf(rfFrame_t*,sxFrame_t*);
	void reformRf2Sx(sxFrame_t*, rfFrame_t*);
	bool isMyGroup(rfFrame_t*, rfFrame_t*);
	void setSxRxData(sxRxFrame_t*);
};

#endif
