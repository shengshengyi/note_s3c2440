
#include "s3c2440_soc.h" 
#include "uart.h" 


int main(void)
{
	unsigned char c;

	uart0_init();
	puts("ENTER TACC VAL: \n\r");

	while(1)
	{
		c = getchar();
		
		putchar(c);
		
		if(c >= '0' && c <= '7') 
		{
			bank0_tacc_set(c - '0');
			led_test();
		}
		
		else
		{
			puts("ERR,VAL BRTWEEN 0 ¨C 7\n\r");
			puts("ENTER TACC VAL:\n\r");
		}
	}
	return 0;
}

