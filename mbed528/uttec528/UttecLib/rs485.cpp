#include <stdio.h>
#include <string.h>

#include "rs485.h"

bool rs485::m_doneFlag = false;
rfFrame_t rs485::m_485Rx = {0,};
rs485Status_t rs485::m_status={0};
uint8_t rs485::m485Data[]={0};
bool rs485::m_485Done=false;

Serial* rs485::pMySer = NULL;

rs485::rs485(Serial* pSer){
	pMySer = pSer;
}

bool rs485::is485Done(){
//	if(pMySer->readable()) parse485Data(pMySer->getc());
	if(pMySer->readable()) parse485Data(getchar());
	return m_485Done;
}

void rs485::clear485Done(){
	m_doneFlag = false;
}

void rs485::task485(rfFrame_t* pFrame){
}

rfFrame_t* rs485::return485Buf(){
	return &m_485Rx;
}

bool rs485::parse485Data(uint8_t ucChar)
{
	if(ucChar==sof){
		m_status.flag=false;
		m_status.count=0;
		m_status.start=true;
		m_485Done=false;
	}
	else if(ucChar==eof){
		if((m_status.start)&&(m_status.count==De485FrameLength)){
			printf("\n\r");
			for(int i=0;i<De485FrameLength;i++) putchar(m485Data[i]);
//			m_status.flag=ProcessFinish485();
			m_status.flag=true;
			m_485Done=true;
			m_status.count=0;
			m_status.start=false;
		}
		else{
			m_status.flag=false;
			m_status.count=0;
			m_status.start=false;
			m_485Done=false;
		}
	}
	else if(m_status.count>De485FrameLength){
			m_status.flag=false;
			m_status.count=0;
			m_status.start=false;
			m_485Done=false;
	}
	else m485Data[m_status.count++]=ucChar;
	return m_485Done;
}

void rs485::send485(rfFrame_t* spFrame)
{
	DigitalOut dTest(LED4);
	UttecUtil myUtil;
	UU16 uu16Temp;
	uint8_t cdTemp[40];
	uint8_t *cpTemp;

//	printf("-------Command=%d\n\r",spFrame->Cmd.Command);
	cpTemp=(uint8_t*)spFrame;
	for(uint8_t i=0;i<sizeof(rfFrame_t);i++) {
		cdTemp[i]=*cpTemp;
		cpTemp++;
	}

	uu16Temp.u16=myUtil.gen_crc16(cdTemp,sizeof(rfFrame_t));
	cdTemp[sizeof(rfFrame_t)]=uu16Temp.u8[0];
	cdTemp[sizeof(rfFrame_t)+1]=uu16Temp.u8[1];

	m485Data[0]='{';
	cpTemp=m485Data;
	cpTemp++;

	for(uint8_t uc=0;uc<(sizeof(rfFrame_t)+2);uc++){
		sprintf((char*)cpTemp,"%02x",cdTemp[uc]);
		cpTemp+=2;
	}
	*cpTemp='}' ;
	dTest = 0;	
	for(int i=0;i<FLENGTH;i++){
		putchar(m485Data[i]);
	}
	dTest = 1;
	
//	for(int i=0;i<FLENGTH;i++) putchar(m485Data[i]);
}

