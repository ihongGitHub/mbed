#include "mbed.h"
/*
#include "ble/BLE.h"
#include "ble/services/UARTService.h"
#include "UttecBle.h"
*/
#include "UttecUtil.h"

dimFactors_t UttecUtil::myDimFact = {0,};

UttecUtil::UttecUtil(){
}
	
uint16_t UttecUtil::gen_crc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = 0;
    int bits_read = 0, bit_flag;
	uint16_t CRC16=0x8005;

    if(data == NULL)
        return 0;
    while(size > 0)
    {
        bit_flag = out >> 15;
        out <<= 1;    out |= (*data >> bits_read) & 1; // item a) work from the least significant bits
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;            data++;            size--;
        }
        if(bit_flag)       out ^= CRC16;

    }
    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;        out <<= 1;        if(bit_flag)	out ^= CRC16;
    }
    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }
    return crc;
}

uint8_t UttecUtil::Hex2Dec(uint8_t cHex)
{
	switch(cHex){
		case '0':		case '1':		case '2':		case '3':		case '4':
		case '5':		case '6':		case '7':		case '8':		case '9':
			cHex-='0';	break;
		case 'a':		case 'b':		case 'c':		case 'd':		case 'e':		case 'f':
			cHex=cHex-'a'+10;	break;
		default:
			cHex=0x55;
	}
	return cHex;
}

#include <stdio.h>
#include <string.h>

char* dispRxTx(char* cResult, uint8_t ucRxTx){
	memset(cResult,0,5);
	switch(ucRxTx){
		case eRx: sprintf(cResult, "Rx"); break;
		case eTx: sprintf(cResult, "Tx"); break;
		case eSRx: sprintf(cResult, "SRx"); break;
		case eRpt: sprintf(cResult, "Rpt"); break;
		case eGW: sprintf(cResult, "GW"); break;
		case eMst: sprintf(cResult, "Mst"); break;
		case eReserved: sprintf(cResult, "TBD"); break;
		default: printf("Error value %d\n\r", ucRxTx);
	}
	return cResult;
}

char* dispSensor(char* cResult, uint8_t ucSensor){
	memset(cResult,0,5);
	switch(ucSensor){
		case eNoSensor: sprintf(cResult, "No"); break;
		case ePir: sprintf(cResult, "Pir"); break;
		case eMicroWave: sprintf(cResult, "Mw"); break;
		case eDayLight: sprintf(cResult, "Day"); break;
		case eUtraSonic: sprintf(cResult, "Snx"); break;
		case eHyBrid: sprintf(cResult, "Hyb"); break;
		case eVolume: sprintf(cResult, "Vol"); break;
		case eTestMode: sprintf(cResult, "Test"); break;
		default: printf("Error value %d\n\r", ucSensor);
	}
	return cResult;
}

#include <time.h>
void disTime(){
	static uint32_t ulTime = 0;
	tm myTime;
	ulTime++;
	myTime.tm_sec = ulTime%60;
	myTime.tm_min = (ulTime/60)%60;
	myTime.tm_hour = (ulTime/3600)%24;
	myTime.tm_mday = ulTime/(3600*24);
	printf("%dD %dH %dM %dS\n\r", myTime.tm_mday,
	myTime.tm_hour, myTime.tm_min, myTime.tm_sec);
}
char* dispForced(char* cResult, bool bForced){
	if(bForced) sprintf(cResult,"manu");
	else sprintf(cResult,"auto");
	return cResult;
}

void UttecUtil::dispSec(rfFrame_t* pFrame){
	char cResult[5];
	printf("G:%d P:%d RxTx:%s", 
	pFrame->MyAddr.GroupAddr, pFrame->MyAddr.PrivateAddr,
	dispRxTx(cResult,pFrame->MyAddr.RxTx.iRxTx));
	printf(" S:%s\n\r",
	dispSensor(cResult,pFrame->MyAddr.SensorType.iSensor));
	disTime();
	printf("Dim :%s, ",dispForced(cResult, myDimFact.forced));
	printf("pwm = %0.3f, Type = %s\n\r", 
		myDimFact.dimValue,dispSensor(cResult,myDimFact.sensorType));
}

void UttecUtil::testProc(uint8_t ucName, uint32_t ulValue){
	printf("\n\r-------- Name: %d ::: value: %d -----------\n\r",
	ucName, ulValue);
}
void UttecUtil::testProc(uint8_t ucName, uint32_t ulValue,
	uint32_t ulValue1){
	printf("\n\r------ Name: %d ::: value: %d value: %d ---\n\r",
	ucName, ulValue, ulValue1);
}
void UttecUtil::testProc(uint8_t ucName, uint32_t ulValue,
	float fValue){
	printf("\n\r------ Name: %d ::: value: %d value: %0.3f ---\n\r",
	ucName, ulValue, fValue);
}

void UttecUtil::setDimFactor(dimFactors_t sFact){
	myDimFact.dimValue = sFact.dimValue;
	myDimFact.sensorType = sFact.sensorType;
	myDimFact.forced = sFact.forced;
}
dimFactors_t UttecUtil::getDimFactor(){
	return myDimFact;
}

#define WDT_1Sec    32767UL
void UttecUtil::setWdt(uint8_t ucTime)
{
    NRF_WDT->POWER=WDT_POWER_POWER_Enabled;
    NRF_WDT->CRV=WDT_1Sec*ucTime;     //For 1Sec
    NRF_WDT->RREN=WDT_RREN_RR0_Enabled<<WDT_RREN_RR0_Pos;
    NRF_WDT->TASKS_START=1;
}
void UttecUtil::setWdtReload()
{
    NRF_WDT->RR[0]=     WDT_RR_RR_Reload;
}

uint16_t UttecUtil::changeBytesInWord(uint16_t uiData){
    ChangeByte_t uChange;
    uint8_t ucTemp;
    uChange.u16 = uiData;
    ucTemp = uChange.u8[0];
    uChange.u8[0]= uChange.u8[1];
    uChange.u8[1]= ucTemp;
    return uChange.u16;
}

