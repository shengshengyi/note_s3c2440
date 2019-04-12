
#include "s3c2440_soc.h" 
#include "uart.h" 


int main(void)
{
	unsigned char c;

	uart0_init();
	//puts("ENTER TACC VAL: \n\r");
	sdram_init();

	if(sdram_test() == 0)
		led_test();

	return 0;
}

