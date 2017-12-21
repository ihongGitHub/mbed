#ifndef __Serial_H__
#define __Serial_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int getCh();

#ifdef __cplusplus
}
#endif

typedef enum
{
	eNoWait = 0,
	eNoLimit = 0xffff
} Serial_t;

typedef enum
{
	UartPc=0,
	Uart485_1=1,
	UpAndSide485Dir_1=1,
	Uart485_2=2,
	Down485Dir_2=2,
} Uttec485_t;

#define EXT_TX1	24
#define EXT_TX2	25
#define EXT_TX3	26

#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED
#define orgTxPin 6
#define Ctr485_1 13
#define Ctr485_2 15

class Ser
{
private:
public:
	static bool firstSerial;
	static uint32_t ulSerialCount;

  Ser();

	void SetUartChannel(Uttec485_t);
	int getSer(uint16_t uiTime);

	void changeTxPin(uint32_t ulPinNum);
	void testPinChange();
	void testGetSer(uint16_t);
};
#endif
