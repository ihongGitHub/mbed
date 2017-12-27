#include <stdio.h>
#include <string.h>

#include "procBle.h"
#include "UttecUtil.h"

DimmerRf* procBle::m_pRf = NULL;

procBle::procBle(DimmerRf* pRf){
	m_pRf = pRf;
}

void procBle::bleTask(rfFrame_t* pFrame){
}
