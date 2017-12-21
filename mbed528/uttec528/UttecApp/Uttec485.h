#ifndef __Uttec485_H__
#define __Uttec485_H__

#include <stdint.h>
#include <stdbool.h>

#include "Flash.h"
#include "Serial.h"
#include "UttecUtil.h"

typedef enum
{
	ByRf=0,
	By485,
	ByAll
} TxCtr_t;

typedef union
{
	uint16_t u16;
	uint8_t u8[2];
} UU16;

typedef union
{
	uint32_t u32;
	uint16_t u16[2];
	uint8_t u8[4];
} UU32;

typedef enum{
	sof='{',
	eof='}',
	FLENGTH=26,
	De485FrameLength=24
//	FLENGTH=70,
//	De485FrameLength=68
} rs485_Frame_t;

typedef struct{
	bool flag;
	bool start;
	uint8_t count;
} rs485Status_t;

typedef enum{
	eModStandby=0,
	eModProcess,
	eModWaitTimeOut
} ModStatus_t;

typedef struct{
	uint8_t GateId;
	uint8_t Function;
	uint16_t Gid;
	uint16_t Data;
	uint16_t Crc;
} ModRxData_t;

typedef struct{
	uint8_t GateId;
	uint8_t Function;
	uint8_t byteNo;
	uint16_t Data;
	uint16_t Crc;
} ModTxData_t;

typedef enum{
	eDimSet=6,
	eDimAsk=3,
	eDimEmergency=55,
} ModFunction_t;

typedef struct{
	uint8_t slaveAddr;
	uint8_t function;
	uint16_t reg;
	uint16_t length;
	uint8_t temp;
	uint16_t gid;
	uint16_t pid;
	uint16_t cmd;
	uint16_t delayTime;
	uint16_t high;
	uint16_t low;
	uint16_t level;
	uint16_t tbd;
	uint8_t lrc;
} MBAscii_t;

typedef struct{
	uint16_t Gid;
	uint8_t Cmd;
	uint8_t SubCmd;
	uint32_t data1;
	uint32_t data2;
} MiniFrame_t;

#define DeModBusTimeOut	100
#define DeModFrameLength	8
#define DeModAsciiMaxFrameLength	32*2

typedef enum{
	eReadReg=4,
	eWriteReg=16
} JunghoCmd_t;

typedef enum{
	eUttecMode=0,
	eJunghoMode,
	eJitcMode,
	eWhichMode
} ModeBus_t; 

class Uttec485
{
private:
	static rfFrame_t mRfFrame;
	static uint8_t m485Data[FLENGTH];
	static rs485Status_t m_status;
	static uint32_t m_ModBusTime;
	static ModRxData_t m_ModData;
	static MiniFrame_t m_miniData;;
//	static MBAscii_t mbData;


	MBAscii_t mbData;
	UttecUtil myUtil;
	bool CheckCrcError(uint8_t* cData, uint8_t cLength);
	bool ProcessFinish485();
	bool ProcessFinishMini485();

	void processJunghoData(uint8_t *mModData,uint8_t ucModCount);

public:
	static bool m_485Done;
	static bool m_Mod485Done;
	static bool m_Ascii485Done;
	static ModeBus_t m_ModeBusMode;

	Uttec485( );
	bool parse485Data(uint8_t ucChar);
	bool parseModBus();
	bool parseModBusAscii(uint8_t ucChar);
	bool test(uint8_t ucData);
	void send485(rfFrame_t*,Uttec485_t);
	bool is485Done(uint8_t ucChar);
	bool isModBusDone();
	bool isModBusAsciiDone(uint8_t ucChar);
	rfFrame_t* return485Frame();
	ModRxData_t* returnModFrame();

	void clear485Done();
	void clearMod485Done();
	void clearAscii485Done();

	uint16_t ModRTU_CRC(uint8_t* buf, int len);
	void decModBusTime();
	void returnJungho();
	MBAscii_t* getJunghoData();
//	void setJunghoData(MBAscii_t);
	rfFrame_t* get485Frame();
	void sendMini485(rfFrame_t* spFrame,Uttec485_t cChannel);
	void sendTestMini485();
	
};

#endif
