#ifndef __RS485_H__
#define __RS485_H__

#include <stdint.h>
#include <stdbool.h>

#include "mbed.h"
#include "DimmerRf.h"
typedef enum{
	eUart = 0,
	eRsDown = 1,
	eRsUp = 2
} rs485Channel_t;

typedef union
{
	uint16_t u16;
	uint8_t u8[2];
} UU16;

typedef enum{
	sof='{',
	eof='}',
//	FLENGTH=26,
//	De485FrameLength=24
	FLENGTH=70,
	De485FrameLength=68
} rs485_Frame_t;

typedef struct{
	bool flag;
	bool start;
	uint8_t count;
} rs485Status_t;

typedef struct{
	rfFrame_t sRf;
	uint16_t sum;
} rs485withRf_t;

class rs485
{
private:
	static rs485Status_t m_status;
	static rfFrame_t m_485Rx;
	static uint8_t m485Data[FLENGTH];
	static bool m_485Done;
	static Serial* pMySer;

	bool parse485Data(uint8_t);
	bool reform485toRx(uint8_t*);
	void changeChannel(rs485Channel_t);
	void sendByUart();
	void sendByDown();
	void sendByUp();

public:
	rs485(Serial*);
	bool is485Done();
	void clear485Done();
	void task485(rfFrame_t*);
//	void send485(rfFrame_t*);
	void send485(rfFrame_t*,rs485Channel_t);

	rfFrame_t* return485Buf();
};
#endif
