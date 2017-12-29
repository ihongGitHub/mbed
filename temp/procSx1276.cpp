#include <stdio.h>
#include <string.h>

#include "procSx1276.h"
#include "UttecUtil.h"

DimmerRf* procSx1276::m_pRf = NULL;

procSx1276::procSx1276(DimmerRf* pRf){
	m_pRf = pRf;
}

void procSx1276::sx1276Task(rfFrame_t* pFrame){
}
