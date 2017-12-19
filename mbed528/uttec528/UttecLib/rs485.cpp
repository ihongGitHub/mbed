#include <stdio.h>
#include <string.h>

#include "rs485.h"

bool rs485::m_doneFlag = false;
rfFrame_t rs485::m_485Rx = {0,};
	
rs485::rs485(){

}

bool rs485::is485Done(){
	return false;
}

void rs485::clear485Done(){
	m_doneFlag = false;
}

void rs485::task485(rfFrame_t* pFrame){
}

rfFrame_t* rs485::return485Buf(){
	return &m_485Rx;
}


