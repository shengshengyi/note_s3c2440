#include"s3c2440_soc.h"

void bank0_tacc_set(int val)
{
	BANKCON0 = val << 8;
}

void sdram_init(void)
{
	BWSCON = 0x22000000;
	
	BANKCON6 = 0x00018001;
	BANKCON7 = 0x00018001;
	
	REFRESH = 0x8404F5;
	
	BANKSIZE= 0xb1;
	
	MRSRB6 = 0x20	;
	MRSRB7 = 0x20;
}

int sdram_test(void)
{
	volatile unsigned char* p = (volatile unsigned char*) 0x30000000;
	int i;

	for(i = 0;i < 1000;i++)
		p[i] = 0x55;

	for(i = 0;i < 1000;i++)
		if(p[i] != 0x55)
			return -1;

	return 0;
}

void copy2sdram(void)
{
	/*要从lds文件中获得_code_start,_bss_start
	 *然后从0地址把数据复制到_code_start
	 */
	extern int _code_start,_bss_start;
	
	volatile unsigned int *dest =(volatile unsigned int *)&_code_start;
	volatile unsigned int *end =(volatile unsigned int *)&_bss_start;
	volatile unsigned int *src =(volatile unsigned int *)0;

	while(dest < end)
	{
		*dest++ = *src++;
	}
}

void clean_bss(void)
{
	extern int _end,_bss_start;

	volatile unsigned int *start =(volatile unsigned int *)&_bss_start;
	volatile unsigned int *end =(volatile unsigned int *)&_end;

	while(start <= end)
	{
		*start++ = 0;
	}
}

