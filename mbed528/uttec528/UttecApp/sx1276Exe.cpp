#include <stdio.h>
#include <string.h>

#include "sx1276Exe.h"
#include "UttecUtil.h"


sx1276Exe::sx1276Exe(){
}

bool sx1276Exe::isSx1276RxDone(){
	m_RxDone = false;
	return m_RxDone;
}
void sx1276Exe::clearSx1276RxDone(){
	m_RxDone = false;
}
