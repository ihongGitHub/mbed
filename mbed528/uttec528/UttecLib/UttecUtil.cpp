#include "UttecUtil.h"

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
