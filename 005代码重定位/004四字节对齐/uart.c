
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

void printHex(unsigned int val)
{
	int i;
	unsigned char arr[8];

	for(i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;
	}

	puts("0x");
	for(i = 7;i >= 0; i--)
	{
		if(arr[i] >= 0 && arr[i] <= 9)
			putchar(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			putchar(arr[i] - 0xA + 'A');
	}
}


