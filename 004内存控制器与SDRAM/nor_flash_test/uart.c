
#include "s3c2440_soc.h"


void uart0_init()
{
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |=  (2<<4) | (2<<6);

	GPHUP &= ~((1<<2)| (1<<3));

	//UCON0 &= ~((1<<0) | (1<<2));
	UCON0 =  0x00000005;

	UBRDIV0 = 26;

	ULCON0 = 0x00000003;
}

int putchar(int c)
{
	while(!(UTRSTAT0 & (1<<2) ) );// 0正在传输 等待
	UTXH0 = (unsigned char) c;
}

int getchar(void)
{
	while(!(UTRSTAT0 & (1<<0)));
	return URXH0;
}

int puts(const char *s)
{
	while(*s)
	{
		putchar(*s);
		s++;
	}
}

