#include"s3c2440_soc.h"
#include "nand_flash.h" 


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

int isBootFromNorFlash(void)
{
	volatile unsigned int *p = (volatile unsigned int *) 0;
	unsigned int val = *p;

	*p = 0x12345678;
	if(*p == 0x12345678)
	{
		/* 写成功，是nand启动 */
		*p = val;
		return 0;
	}
	else
	{
		return 1;
	}

}
void nand_init(void);
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

void copy2sdram(void)
{
	/*要从lds文件中获得_code_start,_bss_start
	 *然后从0地址把数据复制到_code_start
	 */
	extern int _code_start,_bss_start;
	
	volatile unsigned int *dest =(volatile unsigned int *)&_code_start;
	volatile unsigned int *end =(volatile unsigned int *)&_bss_start;
	volatile unsigned int *src =(volatile unsigned int *)0;
	int len;

	len = ((int)&_bss_start) - ((int)&_code_start);
		
	if(isBootFromNorFlash())
	{
		while(dest < end)
		{
			*dest++ = *src++;
		}
	}
	else
	{
		nand_init();
		nand_read(src, dest, len);
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

