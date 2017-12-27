#ifndef _UTTELIB_H_
#define _UTTELIB_H_

#include "Flash.h"
#include "DimmerRf.h"
#include "rs485.h"
#include "sx1276Exe.h"
#include "bleExe.h"
#include "mSecExe.h"

typedef struct{
	Flash* pFlash;
	DimmerRf* pDimmerRf;
	rs485* pRx485;
	sx1276Exe* pSx1276Exe;
	bleExe* pBleExe;
	mSecExe* pMsecEce;
} uttecLib_t;

#endif

