#include"uart.h"

void printException(unsigned int cpsr,char *str)
{
	puts("Exception!\n\r");
	puts("cpsr = ");
	puts(str);
	puts("\n\r");
}
