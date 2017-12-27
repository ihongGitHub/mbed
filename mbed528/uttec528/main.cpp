#include "mbed.h"
#include "nrf.h"

Serial uart(USBTX, USBRX);
		
int main(void)
{
	uart.baud(115200);
	printf("now start 2017.12.23\n\r");
	
}
