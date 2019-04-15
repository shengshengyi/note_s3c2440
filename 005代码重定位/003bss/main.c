
#include "s3c2440_soc.h" 
#include "uart.h" 
#include "init.h"

char g_char = 'A';//定义一个全局变量
const char g_char2 = 'B';//静态变量
int g_A = 0;
int g_B ;



int main(void)
{
	uart0_init();

	puts("\n\rg_A =");
	printHex(g_A);
	puts("\n\r");

	
	return 0;
}


