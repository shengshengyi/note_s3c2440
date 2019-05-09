#include "my_printf.h"
#include "string_utils.h"


#define NOR_FLASH_BASE 0 /* nor--nGCS0  base addr = 0 */

void nor_write_word(unsigned int base,unsigned int offset,unsigned int val)
{
	volatile unsigned short *p = (volatile unsigned short*) (base + (offset << 1));
	*p = val;		
}

unsigned int nor_read_word(unsigned int base,unsigned int offset)
{
	volatile unsigned short *p = (volatile unsigned short*) (base + (offset << 1));
	return *p;
}

void nor_cmd(unsigned int offset , unsigned int cmd)
{
	nor_write_word(NOR_FLASH_BASE,offset,cmd);
}

unsigned int nor_dat(unsigned int offset)
{
	return nor_read_word(NOR_FLASH_BASE,offset);
}


void wait_ready(unsigned int addr)
{
	unsigned int val,pre;
	val = nor_dat(addr>>1);
	pre = nor_dat(addr>>1);

	while((val & (1<<6)) != (pre & (1<<6)))
	{
		pre = val;
		val = nor_dat(addr>>1);
	}
}

void do_scan_nor_flash()
{
	int manufacturer,device;
	char qry[4];
	unsigned int size;
	int regions,region_info_base;
	int block_addr,blocks,block_size;
	int i,j,cnt;
	
	
	/* ��ӡ����ID���豸ID */
	nor_cmd(0x555,0xaa);/* 0051 */
	nor_cmd(0x2aa,0x55);/* 0052 */
	nor_cmd(0x555,0x90);/* 0059 */
	

	manufacturer = nor_dat(0);/* C2H for manufacture code */
	device = nor_dat(1);/* 22DA/DA(Top), and 225B/5B(Bottom) for device code */

	nor_cmd(0,0xf0);/* reset */
	
	nor_cmd(0x55,0x98);/* cfi mode */

	qry[0] = nor_dat(0x10);/* qry command */
	qry[1] = nor_dat(0x11);
	qry[2] = nor_dat(0x12);
	qry[3] = '\0';
	printf("qry = %s\n\r",qry);

	/* ��ӡ���� */
	size = (nor_dat(0x27) << 1);
	printf("manufacturer id = 0x%x, device id = 0x%x,nor size = 0x%x, %dM\n\r",manufacturer,device,size,size/(1024*1024));
	
	/* ��ӡ������������ʼ��ַ */
	/* ���ʽ���:
	 *    erase block region : ���溬��1������block, ���ǵĴ�Сһ��
	 * һ��nor flash����1������region
	 * һ��region����1������block(����)

	 * Erase block region information:
	 *    ǰ2�ֽ�+1    : ��ʾ��region�ж��ٸ�block 
	 *    ��2�ֽ�*256  : ��ʾblock�Ĵ�С
	 */
	regions = nor_dat(0x2c);
	region_info_base = 0x2d;
	block_addr = 0;
	cnt = 0;
	printf("Block/Sector start Address:\n\r");

	for(i = 0;i < regions;i++)
	{
		blocks = 1 + nor_dat(region_info_base) + nor_dat((region_info_base + 1)<<8);
		block_size = 256 * (nor_dat(region_info_base + 2) + nor_dat((region_info_base + 3)<<8));
		region_info_base += 4;

		for(j = 0;j < blocks;j++)
		{
			/* ��ӡÿ��block����ʼ��ַ */
			printHex(block_addr);
			putchar(' ');
			cnt++;
			block_addr += block_size;
			if(cnt % 5 == 0)
				printf("\n\r");
		}
	}
	printf("\n\r");

	/* �˳�cfi mode */
	nor_cmd(0,0xf0);
}

void do_write_nor_flash()
{
	unsigned int addr;
	unsigned char str[100];
	int i,j;
	unsigned int val;

	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string  to write: ");
	gets(str);

	printf("writing...\n\r");

	/*	str[0],str[1]�����16bit
		str[2],str[3]�����16bit
	*/
	i = 0;
	j = 1;
	while(str[i] && str[j])
	{
		val = str[i] + (str[j] << 8);

		
		/* ��д */
		nor_cmd(0x555,0xaa);/* ���� */
		nor_cmd(0x2aa,0x55);
		nor_cmd(0x555,0xa0);/* program */

		nor_cmd(addr>>1,val);
		/* �ȴ���д��� : ������, Q6�ޱ仯ʱ��ʾ���� */

		wait_ready(addr);

		i += 2;
		j += 2;
		addr += 2;
	}

	val = str[i];	
	/* ��д */
	nor_cmd(0x555,0xaa);
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0xa0);
	nor_cmd(addr>>1,val);
	/* �ȴ���д��� : ������, Q6�ޱ仯ʱ��ʾ���� */
	wait_ready(addr);
}

void do_read_nor_flash()
{
	unsigned int addr;
	volatile unsigned char *p;
	unsigned char c;
	unsigned char str[16];
	int i,j;
	
	printf("Enter the address to read: ");
	addr = get_uint();

	p = (volatile unsigned char *)addr;

	printf("Data : \n\r");

	for(i = 0;i < 4;i++)
	{
		for(j = 0;j < 16;j++)
		{
			c = *p++;
			str[j] = c;
			printf("%02x",c);
		}

		printf("   ;");
		
		for(j = 0;j < 16;j++)
		{
			if(str[j] < 0x20 || str[j] > 0x7e)/* �������ַ� */
				putchar('.');
			else
				putchar(str[j]);
		}
	printf("\n\r");
	}
}

void do_erase_nor_flash()
{
	unsigned int addr;

	printf("Enter the address of sector to erase:");
	addr = get_uint();

	printf("erasing...\n\r");

	nor_cmd(0x555,0xaa); /* ���� */
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x80); /* erase sector */
	
	nor_cmd(0x555,0xaa);/* ���� */
	nor_cmd(0x2aa,0x55);
	nor_cmd(addr>>1,0x30);/* ����������ַ��������������Ϊʲô��ԭ����ֵ */
	wait_ready(addr);/* offset�Ǵ�nor�Ƕȳ������������ƣ�������ַ��2440�ĽǶȷ��������øı䣿 */
}




void nor_flash_test(void)
{
	char c;

	while(1)
	{
		printf("[s] Scan nor flash\n\r");
		printf("[w] Write nor flash\n\r");
		printf("[r] Read nor flash\n\r");
		printf("[e] Erase nor flash\n\r");
		printf("[q] quit\n\r");
		printf("Enter selection: ");

		c = getchar();
		printf("%c\n\r",c);

		switch(c)
		{
			case'q':
			case'Q':
				return;
				break;

			case's':
			case'S':
				do_scan_nor_flash();
				break;
				

			case'w':
			case'W':
				do_write_nor_flash();
				break;

			case'r':
			case'R':
				do_read_nor_flash();
				break;

			case'e':
			case'E':
				do_erase_nor_flash();
				break;

			default:
				break;
		}
	}
}
