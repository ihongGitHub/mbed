
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "Rcu.h"
#include "test.h"

extern test myTest;
extern Timer rcuTimer;

InterruptIn rcuEvent(p2);

Rcu_t m_Rcu={true,false,false,false,0,0,0};

static void rcuCallback()
{
	bool bBitOk=false;
	uint32_t ulDelta;
	
	static int begin, end;
	
	end=rcuTimer.read_us();
	ulDelta = end - begin; 
	begin=rcuTimer.read_us();
	
	if((ulDelta>=(DeRcuHeader-DeRcuRange))&&(ulDelta<=(DeRcuHeader+DeRcuRange))){
		m_Rcu.bStart=true;
		m_Rcu.m_ulRcuTimeTimeOut=750;
		m_Rcu.m_bitCount=32;
		m_Rcu.m_RcuData=0;
//		putchar('H');
	}
	else if(m_Rcu.bStart&&(m_Rcu.m_bitCount--)){
		if((ulDelta>=(DeRcu_1-DeRcuRange))&&(ulDelta<=(DeRcu_1+DeRcuRange))){
			m_Rcu.m_RcuData|=0x00000001; bBitOk=true;
//			putchar('1');
		}
		else if((ulDelta>=(DeRcu_0-DeRcuRange))&&(ulDelta<=(DeRcu_0+DeRcuRange))){
			m_Rcu.m_RcuData&=~0x00000001; bBitOk=true;
//			putchar('0');
		}
		if(bBitOk){
			if(!m_Rcu.m_bitCount){
//				printf("End=%x",m_Rcu.m_RcuData);
				m_Rcu.bRcuFlag=true;
				m_Rcu.bStart=m_Rcu.bStart=false;
				m_Rcu.m_ulRcuTimeTimeOut=m_Rcu.m_bitCount=0;
			}
			else m_Rcu.m_RcuData<<=1;
		}
		else{
			m_Rcu.bEnd=m_Rcu.bStart=false;
			m_Rcu.m_ulRcuTimeTimeOut=m_Rcu.m_bitCount=0;
		}
	}
	else{
			m_Rcu.bEnd=m_Rcu.bStart=false;
			m_Rcu.m_ulRcuTimeTimeOut=m_Rcu.m_bitCount=0;
	}
/*	
	*/
}
void rcuTest(){
//	printf("&");
}

Rcu::Rcu(){
	m_Rcu.bRcuFlag=true;
	printf("Rcu\n\r");
	rcuTimer.start();
//	rcuEvent.rise(&rcuTest);
	rcuEvent.fall(&rcuCallback);
}

bool Rcu::isRcuReady(){
	return m_Rcu.bRcuFlag;
}

void Rcu::clearRcuFlag(){
	m_Rcu.bRcuFlag=false;
}
uint16_t Rcu::returnVendorCode(){
	uint32_t ulRcu;
	ulRcu=m_Rcu.m_RcuData;
	ulRcu>>=16;
	return ulRcu;
}
uint8_t Rcu::returnRcuCode(){
	uint32_t ulRcu;
	uint8_t ucRcu0,ucRcu1;
	ulRcu=m_Rcu.m_RcuData;
	ucRcu1=ulRcu>>8;
	ucRcu0=ulRcu;
	ucRcu1=~ucRcu1;
	if(ucRcu1==ucRcu0) printf("Ok=%x\n\r",ucRcu0);
	else printf("Error Rcu Data=%x\n\r",m_Rcu.m_RcuData);
	return ucRcu0;
}
bool Rcu::isUttecCode(){
	if(returnVendorCode()==DeUttecRcuCode) return true;
	else return false;
}
void Rcu::procRcu(rcuValue_t value)
{
	static uint16_t uiNum = 0;
	printf("From procRcu -> ");
	switch(value){
		case rPower: 
			myTest.setTestRfData(); 
		break;
		case rTv: printf("rTv\n\r"); 
			myTest.setTest485Data(); 				
		break;
		case rUp: printf("rUp\n\r"); 
			myTest.setTestBleData(); 
		break;
		case rDown: printf("rDown\n\r"); 
		break;
		case rRight: printf("rRight\n\r"); 
		break;
		case rLeft: printf("rLeft\n\r"); 
		break;
		case rMenu: printf("rMenu\n\r"); 
			uiNum = 0;
		break;
		case rScan: printf("rScan\n\r"); 
			myTest.setTestSxRxData(); 
		break;
		case rMute: printf("rMute\n\r"); 
		break;
		case rOne: printf("rOne\n\r"); 
			uiNum *= 10; uiNum += 1;
		break;
		case rTwo: printf("rTwo\n\r"); 
			uiNum *= 10; uiNum += 2;
		break;
		case rThree: printf("rThree\n\r");
			uiNum *= 10; uiNum += 3;
		break;
		case rFour: printf("rFour\n\r"); 
			uiNum *= 10; uiNum += 4;
		break;
		case rFive: printf("rFive\n\r"); 
			uiNum *= 10; uiNum += 5;
		break;
		case rSix: printf("rSix\n\r"); 
			uiNum *= 10; uiNum += 6;
		break;
		case rSeven: printf("rSeven\n\r"); 
			uiNum *= 10; uiNum += 7;
		break;
		case rEight: printf("rEight\n\r"); 
			uiNum *= 10; uiNum += 8;
		break;
		case rNine: printf("rNine\n\r"); 
			uiNum *= 10; uiNum += 9;
		break;
		case rZero: printf("rZero\n\r"); 
			uiNum *= 10; uiNum += 0;
		break;
		case rPlus: printf("rPlus\n\r"); 
			myTest.setTestReceiveFrameByNum(uiNum);
//			uiNum = 0;
		break;
		case rMinus: printf("rMinus\n\r"); 
			myTest.setTestMyFrameByNum(uiNum);
//			uiNum = 0;
		break;
		default: printf("Rcu Error value: %x\n\r", value);			
	}
	uiNum %= 10000;
	printf("Num = %d\n\r", uiNum);
}




