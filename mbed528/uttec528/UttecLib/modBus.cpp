#include <stdio.h>
#include <string.h>

#include "modBus.h"
#include "UttecUtil.h"

static DigitalOut downCtr(p13);	//FOR DOWNLOAD TXCTR
static DigitalOut upCtr(p19);	//For upload TXCtr

Serial* modBus::pMySer = NULL;
static UttecUtil myUtil;

bool modBus::m_modAsciiDone = false;
uint8_t modBus::mModAsciiData[] = {0,};
modAsciiStatus_t modBus::m_status = {0,};

modBus::modBus(Serial* pSer){
	pMySer = pSer;
	downCtr = 1;
	upCtr = 1;	
}

modBus::modBus(){
}


bool modBus::parseModAscii(uint8_t ucChar)
{
	static uint8_t* ucpModA = mModAsciiData;
	if((ucChar==sofModA)&&(!m_status.start)){
		ucpModA = (uint8_t*)&m_modAFrame;
		m_status.flag=false;
		m_status.count=0;
		m_status.start=true;
		m_modAsciiDone=false;
		putchar('1');
	}
	else if(ucChar==eofModA){
		if((m_status.start)&&(mModAsciiData[m_status.count-1]==crModA)){
			/*
			printf("\n\r");
			for(int i=0;i<sizeof(mtData);i++) putchar(mtData[i]);
			printf("\n\r");
			*/
			printf("count = %d\n\r", m_status.count);
			m_modAsciiDone =true;
			m_status.flag=true;
			m_status.count=0;
			m_status.start=false;
		}
		else{
			m_status.flag=false;
			m_status.count=0;
			m_status.start=false;
			m_modAsciiDone=false;
		}
	}
	else if(m_status.count>modA_FLENGTH){
			m_status.flag=false;
			m_status.count=0;
			m_status.start=false;
			m_modAsciiDone=false;
	}
	else{
		*ucpModA++ = ucChar;
		m_status.count++;
		putchar(ucChar);
	}
//	else mModAsciiData[m_status.count++]=ucChar;
	return m_modAsciiDone;
}
