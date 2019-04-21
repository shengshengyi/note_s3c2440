
#include "s3c2440_soc.h" 
#include "uart.h" 
#include "init.h"

char g_char = 'A';//定义一个全局变量
char g_char3 = 'a';

const char g_char2 = 'B';//静态变量
int g_A = 0;
int g_B ;



int main(void)
{
	
	puts("\n\rg_A =");
	printHex(g_A);
	puts("\n\r");

	while(1)
	{
		putchar(g_char);
		g_char++;

		putchar(g_char3);
		g_char3++;
		delay(1000000);

	}
	return 0;
}


