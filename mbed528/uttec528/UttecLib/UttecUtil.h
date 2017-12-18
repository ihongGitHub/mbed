#ifndef __UttecUtil_H__
#define __UttecUtil_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

class UttecUtil
{
private:

public:
	UttecUtil();
	uint16_t gen_crc16(const uint8_t *data, uint16_t size);
	uint8_t Hex2Dec(uint8_t cHex);
};
#endif
