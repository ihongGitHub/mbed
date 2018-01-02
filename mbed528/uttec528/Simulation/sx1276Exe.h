#ifndef __UttecSx1276_H__
#define __UttecSx1276_H__
/* Set this flag to '1' to display debug messages on the console */
#define DEBUG_MESSAGE   1

/* Set this flag to '1' to use the LoRa modulation or to '0' to use FSK modulation */
#define USE_MODEM_LORA  1
#define USE_MODEM_FSK   !USE_MODEM_LORA

#define RF_FREQUENCY                                    868000000 // Hz
#define TX_OUTPUT_POWER                                 14        // 14 dBm

//    #define LORA_BANDWIDTH                              2         // [0: 125 kHz,   org
    #define LORA_BANDWIDTH                              2         // [0: 125 kHz,
                                                                  //  1: 250 kHz,
                                                                  //  2: 500 kHz,
                                                                  //  3: Reserved]
//    #define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]        org
    #define LORA_SPREADING_FACTOR                       12         // [SF7..SF12]
    #define LORA_CODINGRATE                             1         // [1: 4/5,           org
//    #define LORA_CODINGRATE                             4         // [1: 4/5,
                                                                  //  2: 4/6,
                                                                  //  3: 4/7,
                                                                  //  4: 4/8]
    #define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
    #define LORA_SYMBOL_TIMEOUT                         5         // Symbols
    #define LORA_FIX_LENGTH_PAYLOAD_ON                  false
    #define LORA_FHSS_ENABLED                           false  
    #define LORA_NB_SYMB_HOP                            4     
    #define LORA_IQ_INVERSION_ON                        false
    #define LORA_CRC_ENABLED                            true
    

#define RX_TIMEOUT_VALUE                                3500000   // in us
#define BUFFER_SIZE                                     32        // Define the payload size here


/*
 *  Global variables declarations
 */
typedef enum
{
	LOWPOWER = 0,
	IDLE,
	
	RX,
	RX_TIMEOUT,
	RX_ERROR,
	
	TX,
	TX_TIMEOUT,
	
	CAD,
	CAD_DONE
}AppStates_t;

typedef struct{    
	uint8_t* ptrBuf;
	uint8_t size;    
} sxTxFrame_t;

typedef struct{
	uint8_t* ptrBuf;
	uint16_t size;
	int16_t rssi;
	int8_t snr;
	bool rxFlag;
} sxRxFrame_t;

#define DeTxDelay   1000

class sx1276Exe
{
    
private:

public:
	sx1276Exe();
	void sendLoRa(sxTxFrame_t sFrame);
	sxRxFrame_t* readLoRa();
	void initSx1276_org();
	void initSx1276();
	void testTxRx(uint32_t);
	void clearRxFlag();
	bool getRxFlag();
};
#endif // __UttecSx1276_H__
//#endif

