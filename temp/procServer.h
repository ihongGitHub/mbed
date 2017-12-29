#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "DimmerRf.h"
#include "mSecExe.h"

class procServer
{
private:
	static Flash* mpFlash;
	static Flash_t* mpFlashFrame;
	static rfFrame_t* mp_rfFrame;
	static DimmerRf* pMyRf;
	static mSecExe* pMy_mSec;

	void procControlSub(rfFrame_t*);
	void procNewSetSub(rfFrame_t*);
	void procNewFactSetSub(rfFrame_t*);
	void procAltSub(rfFrame_t*);
public:
	procServer(Flash* pFlash, DimmerRf* pRf, mSecExe* pMsec);
	void taskServer(rfFrame_t*);
	void taskClient(rfFrame_t*);
};

#endif
