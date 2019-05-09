
#include "s3c2440_soc.h" 
#include "uart.h" 
#include "init.h"
#include "nand_flash.h" 

char g_char = 'A';//����һ��ȫ�ֱ���
char g_char3 = 'a';

const char g_char2 = 'B';//��̬����
int g_A = 0;
int g_B ;



int main(void)
{
	led_init();
	//interrupt_init();
	key_eint_init();
	//timer_init();

	puts("\n\rg_A =");
	printHex(g_A);
	puts("\n\r");

	//nor_flash_test();

	nand_init();
	nand_flash_test();

	return 0;
}


