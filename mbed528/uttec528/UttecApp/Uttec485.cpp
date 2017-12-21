#include <stdio.h>
#include <string.h>

#include "app_uart.h"
#include "Uttec485.h"


#include "retarget.h"

rfFrame_t Uttec485::mRfFrame={0};
uint8_t Uttec485::m485Data[]={0};
rs485Status_t Uttec485::m_status={0};
bool Uttec485::m_485Done=false;
bool Uttec485::m_Mod485Done=false;
bool Uttec485::m_Ascii485Done=false;
uint32_t Uttec485::m_ModBusTime=0;
ModRxData_t Uttec485::m_ModData={0,};
//MBAscii_t Uttec485::mbData={0,};
ModeBus_t Uttec485::m_ModeBusMode=eWhichMode;
MiniFrame_t Uttec485::m_miniData={0,};

Uttec485::Uttec485(){
	uint8_t* ucpData=(uint8_t*)&mRfFrame;
	for(int i=0;i<sizeof(rfFrame_t);i++) *ucpData++=0;
	for(int i=0;i<sizeof(m485Data);i++) m485Data[i]=0;
	m_status.flag=false;
	m_status.start=false;
	m_status.count=0;
}

bool Uttec485::test(uint8_t ucData){
	bool bResult=false;
	return bResult;
}

#include "nrf_delay.h"

bool Uttec485::parse485Data(uint8_t ucChar)
{
	if(ucChar==sof){
		m_status.flag=false;
		m_status.count=0;
		m_status.start=true;
		m_ModeBusMode=eUttecMode;
		m_485Done=false;
	}
	else if(ucChar==eof){
		if((m_status.start)&&(m_status.count==De485FrameLength)){
			nrf_delay_ms(5);
			printf("\n\r");
			for(int i=0;i<De485FrameLength;i++) putchar(m485Data[i]);
			m_status.flag=ProcessFinishMini485();
//			m_status.flag=ProcessFinish485();
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
		m_ModeBusMode=eWhichMode;
	}
	else if(m_status.count>De485FrameLength){
			m_status.flag=false;
			m_status.count=0;
			m_status.start=false;
			m_ModeBusMode=eWhichMode;
			m_485Done=false;
	}
	else m485Data[m_status.count++]=ucChar;
	return m_485Done;
}


bool Uttec485::parseModBus()
{
	static uint8_t mModData[DeModFrameLength];
	static ModStatus_t sModeStatus=eModStandby;
	static uint8_t ucModCount=0;
	uint8_t ucChar;
	UU16 uu16;
	uint16_t uiTest;

	if(!m_ModBusTime){
		sModeStatus=eModStandby;
		ucModCount=0;
	}
//	if(app_uart_get(&ucChar)!=NRF_SUCCESS) return false;
	if(uttec_uart_get(&ucChar)!=NRF_SUCCESS) return false;

	switch(sModeStatus){
		case eModStandby:
			printf("eModStandby\n\r");
			memset(mModData,0,DeModFrameLength);
			sModeStatus=eModProcess;
			m_ModBusTime=DeModBusTimeOut;
			m_Mod485Done=false;
			mModData[ucModCount]=ucChar;
			ucModCount++;
			break;

		case eModProcess:
			mModData[ucModCount]=ucChar;
			ucModCount++;
			m_ModBusTime=DeModBusTimeOut;
			if(ucModCount>=DeModFrameLength){
				uu16.u8[0]=mModData[6];
				uu16.u8[1]=mModData[7];
				uiTest=ModRTU_CRC(mModData,6);
				m_ModData.Crc=uiTest;
				printf("Crc=%x\n\r",uiTest);
				if(uiTest==uu16.u16) {
					m_ModData.GateId=mModData[0];
					m_ModData.Function=mModData[1];
					uu16.u8[1]=mModData[2];
					uu16.u8[0]=mModData[3];
					m_ModData.Gid=uu16.u16;
					uu16.u8[1]=mModData[4];
					uu16.u8[0]=mModData[5];
					m_ModData.Data=uu16.u16;
					printf("\n\r--------Ok Crc\n\r");
					printf("GateWay Id=%d\n\r",m_ModData.GateId);
					printf("Function=%d\n\r",m_ModData.Function);
					printf("Gid=%d\n\r",m_ModData.Gid);
					printf("Data=%d\n\r",m_ModData.Data);
					printf("Crc=%d\n\r",m_ModData.Crc);
					m_Mod485Done=true;
				}
				else{
					m_Mod485Done=false;
					printf("Crc Error -------\n\r");
				}
				sModeStatus=eModWaitTimeOut;
			}
			break;

		case eModWaitTimeOut:
			if(!m_ModBusTime){
				sModeStatus=eModStandby;
				printf("TimeOut\n\r");
			}
			ucModCount=0;
			break;

		default:
			break;
	}
	return m_Mod485Done;
}

bool isCrLf(uint8_t crlf){
	// putchar(crlf);
	// printf("crlf=%d",crlf);
	static uint8_t sModeStatus=0;
	bool result=false;
	if(crlf==0x0d){
		// printf("cr\n\r");
		sModeStatus=1;
	}
	else if(crlf==0x0a){
		// printf("lf\n\r");
		if(sModeStatus) result=true;
		sModeStatus=0;
	}
	else sModeStatus=0;
	return result;
}

void Uttec485::processJunghoData(uint8_t *mModData,uint8_t ucModCount)
{
	uint8_t uLrc=0;
	uint16_t uiData[8];
	
	printf("processJunghoData=%d\n\r",ucModCount);
	for(int i=0;i<ucModCount;i++){
		putchar(mModData[i]);
	}
	uint8_t ucTemp=0;
	uint8_t ucCount=0;
	
	for(int i=0;i<ucModCount-1;i++){
		mModData[i]=myUtil.Hex2Dec(mModData[i]);
		printf("%x",mModData[i]);
		if(i%2){
			ucTemp+=mModData[i];
			m485Data[ucCount++]=ucTemp;
			if(i<(ucModCount-2))
				uLrc+=ucTemp;
		}
		else{
			ucTemp=mModData[i]*16;
		}
	}
	
	uLrc=~uLrc;
	uLrc++;
	printf("uLrc=%x\n\r",uLrc);
	
	ucCount=0;
	mbData.slaveAddr=m485Data[ucCount++];	
	mbData.function=m485Data[ucCount++];	
	
	Flash myFlash;
	uint16_t myGid=myFlash.getFlashFrame()->rfFrame.MyAddr.GroupAddr/10;
	MBAscii_t* phks=getJunghoData();
	switch(mbData.function){
		case eReadReg:
			printf("eReadReg\n\r");
			mbData.gid=m485Data[ucCount++]*256;				
			mbData.gid=mbData.gid+m485Data[ucCount++];	
			mbData.length=m485Data[ucCount++]*256;				
			mbData.length=mbData.length+m485Data[ucCount++];	
			printf("&&&&&&&&&&& Gid=%d, slave=%d, myGid=%d\n\r",
		mbData.gid,mbData.slaveAddr,myGid);
			mbData.pid=0;
		
			if((mbData.slaveAddr!=mbData.gid/10)||(myGid!=(mbData.gid/10))){
				printf("~~~~~~~~~No Matching Slave Address = %d, MyAddr = %d\n\r",
				mbData.slaveAddr,mbData.gid);
				m_Ascii485Done=false;
				return; 
			}
/*		
*/	
			printf("-----now return gid=%d\n\r",phks->gid);
		
			mbData.lrc=m485Data[ucCount++];
			printf("lrc=%X\n\r",mbData.lrc);
			if(uLrc==mbData.lrc){
				printf("Ok LRC\n\r");
				m_Ascii485Done=true;
			}
		
		break;
		case eWriteReg:
			printf("eWriteReg\n\r");
		
			mbData.reg=m485Data[ucCount++]*256;
			mbData.reg=mbData.reg+m485Data[ucCount++];	
			mbData.length=m485Data[ucCount++]*256;				
			mbData.length=mbData.reg+m485Data[ucCount++];	
			mbData.temp=m485Data[ucCount++];
			
			for(int i=0;i<8;i++){
				uiData[i]=m485Data[ucCount++]*256;
				uiData[i]=uiData[i]+m485Data[ucCount++];
			}
			mbData.lrc=m485Data[ucCount++];
			printf("lrc=%X\n\r",mbData.lrc);
			if(uLrc==mbData.lrc){
				printf("Ok LRC\n\r");
				m_Ascii485Done=true;
			}
			
			ucCount=0;
			mbData.gid=uiData[ucCount++];
			mbData.pid=uiData[ucCount++];
			mbData.cmd=uiData[ucCount++];
			mbData.delayTime=uiData[ucCount++];
			mbData.high=uiData[ucCount++];
			mbData.low=uiData[ucCount++];
			mbData.level=uiData[ucCount++];
			mbData.tbd=uiData[ucCount++];		
		break;
	}
}

#include "nrf_delay.h"

void Uttec485::returnJungho()
{
	Serial mySerial;
	
	uint8_t ucData[DeModAsciiMaxFrameLength];
	uint8_t* pucData=ucData;
	
//	mbData.slaveAddr=11;
//	mbData.reg=0;
//	mbData.temp=mbData.function;

	if(mbData.function==eWriteReg){
		printf("eWriteReg\n\r");
		mbData.function=0x10;
		
		*pucData++=mbData.slaveAddr;
		*pucData++=mbData.function;
		*pucData++=0; *pucData++=mbData.reg;
		*pucData++=0; *pucData++=mbData.length;		

	}
	else{
		*pucData++=mbData.slaveAddr;
		*pucData++=mbData.function=4;
		*pucData++=mbData.length=10;
		
		*pucData++=mbData.gid/256; 
		*pucData++=mbData.gid%256;
		*pucData++=mbData.pid;
		*pucData++=mbData.cmd;
		*pucData++=mbData.delayTime/256; 
		*pucData++=mbData.delayTime%256;
		*pucData++=mbData.high;
		*pucData++=mbData.low;
		*pucData++=mbData.level;
		*pucData++=mbData.level;
		/*
		printf("\n\r+++++++++++++++++ Level = %d\n\r",mbData.level);
		printf("Gid=%d\n\r",mbData.gid);
		printf("pid=%d\n\r",mbData.pid);
		printf("cmd=%d\n\r",mbData.cmd);
		printf("delayTime=%d\n\r",mbData.delayTime);
		printf("high=%d\n\r",mbData.high);
		printf("low=%d\n\r",mbData.low);
		*/
	}
	
	uint8_t ucDelta=(uint8_t)(pucData-ucData);
	uint8_t uLrc=0;
	
	printf("-------Total lenght=%d\n\r",ucDelta);
	printf(" From Uttec Gw --> Jungho Gw\n\r");
	
	mySerial.SetUartChannel(Uart485_1);
//	nrf_delay_ms(1);
	putchar(':');
	for(int i=0;i<ucDelta;i++){
		printf("%02X",ucData[i]);
		uLrc+=ucData[i];
	}
	uLrc=~uLrc;
	uLrc++;
	mbData.lrc=uLrc;
	printf("%02X",mbData.lrc);
	
	printf("\r\n");
//	nrf_delay_ms(1);
	mySerial.SetUartChannel(UartPc);
	nrf_delay_ms(1);
	printf("LRC=%X\n\r",uLrc);
	
}

bool checkLrcForJungho()
{
}

bool Uttec485::parseModBusAscii(uint8_t ucChar)
{
	static uint8_t mModData[DeModAsciiMaxFrameLength];
	static ModStatus_t sModeStatus=eModStandby;
	static uint8_t ucModCount=0;
	
	UU16 uu16;
	
	uint16_t uiTest;

	switch(sModeStatus){
		case eModStandby:
			if(ucChar==':'){
//				printf("00000000000\n\r");
				ucModCount=0;
				sModeStatus=eModProcess;
				memset(mModData,0,DeModAsciiMaxFrameLength);
				m_ModeBusMode=eJunghoMode;
			}
			m_ModBusTime=DeModBusTimeOut;
			m_Ascii485Done=false;
			break;

		case eModProcess:
			m_ModBusTime=DeModBusTimeOut;
			if(isCrLf(ucChar)){
				m_ModeBusMode=eWhichMode;
				
				processJunghoData(mModData,ucModCount);
				
				if(m_Ascii485Done){
					switch(mbData.function){
						case eReadReg:
							printf("###########eReadReg\n\r");
							/*
							printf("slaveAddr=%X\n\r",mbData.slaveAddr);
							printf("function=%X\n\r",mbData.function);
							printf("gid=%X\n\r",mbData.gid);
							printf("length=%X\n\r",mbData.length);
						*/
						break;
						case eWriteReg:
							printf("*************eWriteReg\n\r");
						/*
							printf("slaveAddr=%X\n\r",mbData.slaveAddr);
							printf("function=%X\n\r",mbData.function);
							printf("reg=%X\n\r",mbData.reg);
							printf("length=%X\n\r",mbData.length);
							printf("temp=%X\n\r",mbData.temp);
							printf("gid=%d\n\r",mbData.gid);
							printf("pid=%d\n\r",mbData.pid);
							printf("cmd=%d\n\r",mbData.cmd);
							printf("delayTime=%d\n\r",mbData.delayTime);
							printf("high=%d\n\r",mbData.high);
							printf("low=%d\n\r",mbData.low);
							printf("level=%d\n\r",mbData.level);
							printf("tbd=%d\n\r",mbData.tbd);
						*/
							returnJungho();
						break;
					}
				}
				else{
					printf("Error in processJunghoData\n\r");
				}
				sModeStatus=eModStandby;
				m_ModeBusMode=eWhichMode;
			}
			else if(ucModCount<=DeModAsciiMaxFrameLength){
				mModData[ucModCount]=ucChar;
				ucModCount++;
			}
			else if(ucModCount>DeModAsciiMaxFrameLength){
				sModeStatus=eModStandby;
				printf("Frame Length is Over =%d\n\r",ucModCount);
				m_ModeBusMode=eWhichMode;
			}
			break;
			
		case eModWaitTimeOut:
			if(!m_ModBusTime){
				m_ModeBusMode=eWhichMode;
				sModeStatus=eModStandby;
				printf("TimeOut\n\r");
			}
			ucModCount=0;
			break;

		default:
			break;
	}
	return m_Ascii485Done;
}
MBAscii_t* Uttec485::getJunghoData() 
{
	return &mbData;
}
/*
void Uttec485::setJunghoData(MBAscii_t jData)
{
	mbData=jData;
}
*/
bool Uttec485::CheckCrcError(uint8_t* cData, uint8_t cLength)
{
	bool fResult=false;
	uint8_t cTemp=0;
	uint8_t cHex0,cHex1;//,cHex2;

	UU16 uu16Temp;
	uint16_t iTemp;

	for(cTemp=0;cTemp<cLength;cTemp+=2){
			cHex0=myUtil.Hex2Dec(*cData); cData++;
			cHex1=myUtil.Hex2Dec(*cData); cData++;
			cHex1+=cHex0*16;
			m485Data[cTemp/2]=cHex1;
//		printf("%d=%02x,%02x,%02x\n\r",cTemp,cHex1,cHex0,cHex2);
	}
	iTemp=myUtil.gen_crc16(m485Data,32);
	uu16Temp.u8[0]=m485Data[32];
	uu16Temp.u8[1]=m485Data[33];
	if(uu16Temp.u16==iTemp){
		fResult=true;
	}
	else{
		printf("No Match Return Crc=%d, Org Crc=%d\n\r",iTemp,uu16Temp.u16);
	}
	return fResult;
}

bool Uttec485::ProcessFinishMini485(void)
{
	bool bResult=true;
	uint8_t cHex0,cHex1;//,cHex2;
	uint8_t cTemp=0;
	uint8_t* cData=m485Data;
//	MiniFrame_t myFrame;
	uint8_t* ucDst=(uint8_t*)&m_miniData;
	
//	for(int i=0;i<De485FrameLength;i++) putchar(m485Data[i]);
	
	for(cTemp=0;cTemp<De485FrameLength;cTemp+=2){
			cHex0=myUtil.Hex2Dec(*cData); cData++;
			cHex1=myUtil.Hex2Dec(*cData); cData++;
			cHex1+=cHex0*16;
			m485Data[cTemp/2]=cHex1;
//		printf("%d=%02x,%02x,%02x\n\r",cTemp,cHex1,cHex0,cHex2);
	}
	
	cData=m485Data;
	memcpy(ucDst, cData, De485FrameLength/2);
	printf("\n\rGid = %d\n\r", m_miniData.Gid);
	
	mRfFrame.MyAddr.GroupAddr=m_miniData.Gid;
	mRfFrame.Cmd.Command=m_miniData.Cmd;
	mRfFrame.Cmd.SubCmd=m_miniData.SubCmd;
	mRfFrame.Ctr.Level = m_miniData.data1%256;
	printf("+++++++++++++ Level = %d\n\r",mRfFrame.Ctr.Level);
	
		
	return bResult;
}

bool Uttec485::ProcessFinish485(void)
{
	bool bResult=false;
	if(CheckCrcError(m485Data,De485FrameLength)){
		m_ModeBusMode=eWhichMode;
		printf("485Finish\n\r");
		uint8_t* ucpData=(uint8_t*)&mRfFrame;
		for(int i=0;i<sizeof(rfFrame_t);i++) *ucpData++=m485Data[i];
		printf("Gid=%d,Pid=%d\n\r",mRfFrame.MyAddr.GroupAddr,
			mRfFrame.MyAddr.PrivateAddr);
		printf("----Command=%d\n\r",mRfFrame.Cmd.Command);
		bResult=true;
	}
	else{
		printf("485 Error\n\r");
	}
	return bResult;
}
#include "CmdDefine.h"

void Uttec485::sendMini485(rfFrame_t* spFrame,Uttec485_t cChannel){
	Serial mySerial;
	uint8_t cdTemp[40];
	uint8_t *cpTemp;
	
	printf("-------Mini Command=%d\n\r",spFrame->Cmd.Command);
	MiniFrame_t myFrame;
	miniData_t myData;
	
	uint8_t* pSrc=(uint8_t*)&spFrame->Ctr;
	uint8_t* pDst= (uint8_t*)&myData;
	
	myFrame.Gid=spFrame->MyAddr.GroupAddr;
	myFrame.Cmd=spFrame->Cmd.Command;
	myFrame.SubCmd=spFrame->Cmd.SubCmd;
	memcpy(pDst, pSrc, 8);
	myFrame.data1 = myData.data1;
	myFrame.data2 = myData.data2;
	
	pSrc = (uint8_t*)&myFrame;
	
	cdTemp[0]='{';
	cpTemp=cdTemp;
	cpTemp++;

	for(uint8_t uc=0;uc<12;uc++){
		sprintf((char*)cpTemp,"%02x",*pSrc++);
		cpTemp+=2;
	}
	*cpTemp='}' ;
	
	mySerial.SetUartChannel(cChannel);
	nrf_delay_ms(3);
	for(int i=0;i<FLENGTH;i++) putchar(cdTemp[i]);
	nrf_delay_ms(5);
	mySerial.SetUartChannel(UartPc);	
}

void Uttec485::sendTestMini485(){
	Serial mySerial;
	uint8_t cdTemp[40];
	uint8_t *cpTemp;
	
	printf("-------Mini Frame Test\n\r");
	MiniFrame_t myFrame;
	miniData_t myData;
	
	myFrame.Gid=25;
	myFrame.Cmd=edServerReq;
	myFrame.SubCmd=edsControl;
	myFrame.data1 = 0x12345678;
//	myFrame.data1 = 100l;
//	myFrame.data2 = 0;
	myFrame.data2 = 0x12345678;

	uint8_t* pSrc= (uint8_t*)&myFrame;
	
	cdTemp[0]='{';
	cpTemp=cdTemp;
	cpTemp++;

	for(uint8_t uc=0;uc<12;uc++){
		sprintf((char*)cpTemp,"%02x",*pSrc++);
		cpTemp+=2;
	}
	*cpTemp='}' ;
	
// UartPc	, Uart485_1,UpAndSide485Dir_1,  Uart485_2, Down485Dir_2
	mySerial.SetUartChannel(UartPc);
	nrf_delay_ms(3);
	for(int i=0;i<FLENGTH;i++) putchar(cdTemp[i]);
	nrf_delay_ms(5);
	mySerial.SetUartChannel(UartPc);	
}

void Uttec485::send485(rfFrame_t* spFrame,Uttec485_t cChannel)
{
	Serial mySerial;

	UU16 uu16Temp;
	uint8_t cdTemp[40];
	uint8_t *cpTemp;

	printf("-------Command=%d\n\r",spFrame->Cmd.Command);
	cpTemp=(uint8_t*)spFrame;
//	cpTemp=(uint8_t*)&mRfFrame;		//Test Only
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

	
	mySerial.SetUartChannel(cChannel);
	nrf_delay_ms(3);
	for(int i=0;i<FLENGTH;i++) putchar(m485Data[i]);
	nrf_delay_ms(5);
	mySerial.SetUartChannel(UartPc);
	app_uart_flush();
}

bool Uttec485::is485Done(uint8_t ucChar){
//	putchar('&');
	
	if(!parse485Data(ucChar)) return false;

	Flash* myFlash;
	rfFrame_t* myRf=&myFlash->getFlashFrame()->rfFrame;
	uint16_t myAddr=myRf->MyAddr.GroupAddr;
	uint16_t srcAddr=mRfFrame.MyAddr.GroupAddr;
	
//	m_485Done=true;
	printf(" is485Done Ok\n\r");
	
/*
	switch(myRf->MyAddr.RxTx.iRxTx){
		case eRx:
		case eTx:
		case eSRx:
		case eRpt:
			if(myAddr==srcAddr)
				m_485Done=true;
			break;
		case eGW:
			myAddr=myAddr-myAddr%10;
			if((srcAddr>=myAddr)&&(srcAddr<(myAddr+10)))
				m_485Done=true;
			break;
		case eMst:
				m_485Done=true;
			break;
		default:
			break;
	}
	if(m_485Done) printf("~~~485 Pass\n\r");
	else printf("###Not Pass in %d, My=%d, From=%d\n\r",
		myRf->MyAddr.RxTx.iRxTx,myRf->MyAddr.GroupAddr,
		mRfFrame.MyAddr.GroupAddr);
		*/
	return m_485Done;
}

bool Uttec485::isModBusDone(){
	if(!parseModBus()) return false;
	printf("ModBusDone\n\r");
	return m_Mod485Done;
}

bool Uttec485::isModBusAsciiDone(uint8_t ucChar){
	if(!parseModBusAscii(ucChar)) return false;
	return m_Ascii485Done;
}


void Uttec485::clear485Done(){
	m_485Done=false;
	m_ModeBusMode=eWhichMode;
}
void Uttec485::clearMod485Done(){
	m_Mod485Done=false;
}
void Uttec485::clearAscii485Done(){
	m_Ascii485Done=false;
	m_ModeBusMode=eWhichMode;
}

rfFrame_t* Uttec485::return485Frame(){
	return &mRfFrame;
}

uint16_t Uttec485::ModRTU_CRC(uint8_t* buf, int len)
{
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  return crc;
}
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)

void Uttec485::decModBusTime(void){
	if(m_ModBusTime) m_ModBusTime--;
}

ModRxData_t* Uttec485::returnModFrame(){
	return &m_ModData;
}

rfFrame_t* Uttec485::get485Frame(){
	return &mRfFrame;
}
/*

*/
