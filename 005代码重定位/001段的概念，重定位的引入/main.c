
#include "s3c2440_soc.h" 
#include "uart.h" 
#include "init.h"
/*
char g_char = 'A';//����һ��ȫ�ֱ���
const char g_char2 = 'B';//��̬����
int g_A = 0;
int g_B ;


int main(void)
{
	uart0_init();
	
	while(1)
	{
		putchar(g_char);
		g_char++;
		delay(1000000);
	}
	
	return 0 ;
}
*/
char g_Char = 'A';
const char g_Char2 = 'B';
int g_A = 0;
int g_B;

int main(void)
{
	uart0_init();

	while (1)
	{
		putchar(g_Char);
		g_Char++;		  /* nor����ʱ, �˴�����Ч */
		delay(1000000);
	}

	
	return 0;
}


