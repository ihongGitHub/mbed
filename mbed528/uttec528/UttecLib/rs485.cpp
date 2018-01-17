#include <stdio.h>
#include <string.h>

#include "rs485.h"

rfFrame_t rs485::m_485Rx = {0,};
rs485Status_t rs485::m_status={0};
rs485Status_t rs485::m_tstatus={0};

uint8_t rs485::m485Data[]={0};
uint8_t rs485::mtData[]={0};
bool rs485::m_485Done=false;
bool rs485::m_testDone=false;

Serial* rs485::pMySer = NULL;
UttecUtil myUtil;

DigitalOut downCtr(p13);	//FOR DOWNLOAD TXCTR
DigitalOut upCtr(p19);	//For upload TXCtr

rs485::rs485(Serial* pSer){
	pMySer = pSer;
	downCtr = 1;
	upCtr = 1;
}

bool rs485::is485Done(){
//	if(pMySer->readable()) parse485Data(getchar());
	return m_485Done;
}
bool rs485::isTestDone(){
	if(pMySer->readable()) parseTestData(getchar());
	return m_testDone;
}

void rs485::clear485Done(){
	m_485Done = false;
}
void rs485::clearTestDone(){
	m_testDone = false;
}

void rs485::task485(rfFrame_t* pFrame){
}

rfFrame_t* rs485::return485Buf(){
	return &m_485Rx;
}
uint16_t rs485::returnTestData(){
	uint32_t uiResult = 0;
	uint8_t ucData;
	for(int i = 0; i<sizeof(mtData)-2; i++){
		ucData = mtData[i];
		if((ucData < '0')||(ucData>'9')){
			printf("Error Data\n\r");
		}
		uiResult += (ucData-'0');
		uiResult *= 10;
	}
	
	return uiResult/10;
}
bool rs485::reform485toRx(uint8_t* p485){
	rs485withRf_t frame;
	rs485withRf_t* pFrame = &frame;
	uint8_t* ucpTemp = (uint8_t *)pFrame;
	uint8_t ucTemp;
	uint16_t uiSum;
	bool bResult = false;
//	printf("sizeof(rs485withRf_t) = %d\n\r", sizeof(rs485withRf_t));	
	for(int i = 0; i<sizeof(rs485withRf_t); i++){
		ucTemp=myUtil.Hex2Dec(p485[i*2]);
		ucTemp=ucTemp*16 + myUtil.Hex2Dec(p485[i*2+1]);		
		*ucpTemp++ = ucTemp;
//		printf("%d, ", ucTemp);
	}
	uiSum = myUtil.gen_crc16((uint8_t*)&frame.sRf,sizeof(rfFrame_t));
	printf("Gid = %d, Pid = %d, sum = %d, crc = %d\n\r", frame.sRf.MyAddr.GroupAddr,
		frame.sRf.MyAddr.PrivateAddr, frame.sum, uiSum);
	if(uiSum == frame.sum){
		bResult = true;
		m_485Rx = frame.sRf;
	}
	else printf("gen_crc16 Error %d, %d\n\r", frame.sum, uiSum);
//	myUtil.dispRfFactor(&frame.sRf);
//	frame.sRf.MyAddr.GroupAddr=myUtil.changeBytesInWord(frame.sRf.MyAddr.GroupAddr);
	return bResult;
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
			printf("\n\r");
			m_485Done=reform485toRx(m485Data);
//			m_status.flag=ProcessFinish485();
			m_status.flag=true;
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

bool rs485::parseTestData(uint8_t ucChar)
{
	if((ucChar==tsof)&&(!m_tstatus.start)){
		m_tstatus.flag=false;
		m_tstatus.count=0;
		m_tstatus.start=true;
		m_testDone=false;
	}
	else if(ucChar==teof){
		if((m_tstatus.start)&&(m_tstatus.count==4)){
			/*
			printf("\n\r");
			for(int i=0;i<sizeof(mtData);i++) putchar(mtData[i]);
			printf("\n\r");
			*/
			m_testDone =true;
			m_tstatus.flag=true;
			m_tstatus.count=0;
			m_tstatus.start=false;
		}
		else{
			m_tstatus.flag=false;
			m_tstatus.count=0;
			m_tstatus.start=false;
			m_testDone=false;
		}
	}
	else if(m_tstatus.count>4){
			m_tstatus.flag=false;
			m_tstatus.count=0;
			m_tstatus.start=false;
			m_testDone=false;
	}
	else mtData[m_tstatus.count++]=ucChar;
	return m_testDone;
}

void rs485::sendByUart(){
	Serial sendUart(p9, p11);		//For Uart stdio
	sendUart.baud(115200);
	for(int i=0;i<FLENGTH;i++){
		sendUart.putc(m485Data[i]);
	}
}

void rs485::sendByDown(){
	Serial sendDown(p12, p11);	//FOR Download TX
	sendDown.baud(115200);
	for(int i=0;i<FLENGTH;i++){
		downCtr = 0;
		sendDown.putc(m485Data[i]);
	}
	wait(0.0002);
	downCtr = 1;		
}

void rs485::sendByUp(){
	Serial sendUp(p18, p11);		//For Up Tx
	sendUp.baud(115200);		
//	sendUp.baud(115200/12);		//For 9600bps
	for(int i=0;i<FLENGTH;i++){
		upCtr = 0;
		sendUp.putc(m485Data[i]);
	}
	wait(0.0002);
	upCtr = 1;		
}

void rs485::changeChannel(rs485Channel_t ch){
	switch(ch){
		case eUart:
			sendByUart();
			break;
		case eRsDown:
			sendByDown();
			break;
		case eRsUp:
			sendByUp();
			break;
		default:
			break;
	}
	Serial sendUart(p9, p11);		//For Uart stdio
	sendUart.baud(115200);
}

void rs485::send485(rfFrame_t* spFrame, rs485Channel_t ch)
{		
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
	changeChannel(ch);
}

void rs485::set485Done(){
	m_485Done = true;
}


