#include "nrf.h"
#include "app_uart.h"
#include "Serial.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif
#include "nrf_drv_uart.h"

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */
//hong
bool Ser::firstSerial=true;
uint32_t Ser::ulSerialCount=0;

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

const app_uart_comm_params_t comm_params =
{
		8,	//Rx
		6,	//Tx
		5,
		7,
		UART_HWFC,
		false,
		NRF_UART_BAUDRATE_115200
};


void initSerial()
{
  uint32_t err_code;	
  APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);	
	if(err_code != NRF_SUCCESS) while(1);
}

Ser::Ser()
{
    if(firstSerial){
      initSerial();
			firstSerial=false;
		}
    else			
      printf("New Serial = %d\n\r", ulSerialCount++);
}
void setUartInstance(){
//	nrf_drv_uart_t dma_uart_driver_instance = NRF_DRV_UART_INSTANCE(UART0_INSTANCE_INDEX);
	nrf_drv_uart_t dma_uart_driver_instance ={
		NRF_UARTE0,
		UART0_INSTANCE_INDEX,
	};
	// unitialize previous config
	nrf_drv_uart_uninit(&dma_uart_driver_instance);
}

void Ser::changeTxPin(uint32_t ulTxPin){
	uint32_t err_code;
	app_uart_comm_params_t comm_params1 =comm_params;
	comm_params1.tx_pin_no=ulTxPin;
	setUartInstance();
	APP_UART_FIFO_INIT(&comm_params1,
											 UART_RX_BUF_SIZE,
											 UART_TX_BUF_SIZE,
											 uart_error_handle,
											 APP_IRQ_PRIORITY_LOWEST,
											 err_code);
	if(err_code) printf("err_cod=%d\n\r",err_code);
}


#include "TimerCpp.h"
int Ser::getSer(uint16_t uiTime){
	TimerCpp myTimer;
	int iResult = -1;
	if(!uiTime) return getCh();
	else if(uiTime<eNoLimit){
		myTimer.setSerTimeOut(uiTime);
		iResult = getCh();
		while((iResult == -1)&&myTimer.getSerTimeOut()){
			iResult = getCh();
		}
	}
	else{
		iResult = getCh();
		while(iResult == -1){
			iResult = getCh();
		}
	}		
	return iResult;
}
/*
*/
void Ser::testGetSer(uint16_t uiTime){
	printf("This is testGetSer(500) 2017.12.15 \n\r");
	printf("Wait for input character for %d mSec\n\r", uiTime);
	int iResult = getSer(uiTime);
	if(iResult == -1) printf("No character\n\r");
	else printf("Input character is %c\n\r",iResult);
	printf("End of testGetSer\n\r");
	
	while(1);
}

#include "nrf_delay.h"
void Ser::testPinChange(){
	uint32_t ulCount = 0;
	bool bToggle = false;
	printf("This is testPinChange 2017.12.15\n\r");
	
	while(1){
		ulCount++;
		bToggle = !bToggle;
		if(bToggle){
			changeTxPin(6);
			printf("This is original pin: %d, %d\n\r", 6, ulCount);
		}
		else{
//			changeTxPin(13);
			printf("This is change pin: %d, %d\n\r", 13, ulCount);
		}
		nrf_delay_ms(500);
	}
}

