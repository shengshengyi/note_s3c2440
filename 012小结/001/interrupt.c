#include "s3c2440_soc.h"

typedef void(*irq_func)(int);
irq_func irq_array[32];


/* SRCPND ������ʾ�ĸ��жϲ�����, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK ���������ж�, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND ������ʾ��ǰ���ȼ���ߵġ����ڷ������ж�, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTOFFSET : ������ʾINTPND����һλ������Ϊ1
 */

/* ��ʼ���жϿ����� */
void interrupt_init(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
	INTMSK &= ~(1<<10);  /* enable timer0 int */
}

/* ��EINTPEND�ֱ����ĸ�EINT����(eint4~23)
 * ����ж�ʱ, дEINTPEND����Ӧλ
 */


void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;
	int i,j;
	if (irq == 0) /* eint0 : s2 ���� D12 */
	{
		if (val1 & (1<<0)) /* s2 --> gpf6 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<6);
		}
		
	}
	else if (irq == 2) /* eint2 : s3 ���� D11 */
	{
		if (val1 & (1<<2)) /* s3 --> gpf5 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<5);
		}
		
	}
	else if (irq == 5) /* eint8_23, eint11--s4 ���� D10, eint19---s5 ��������LED */
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (val2 & (1<<3)) /* s4 --> gpf4 */
			{
				/* �ɿ� */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* ���� */
				GPFDAT &= ~(1<<4);
			}
		}
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* �ɿ� */
				/* Ϩ������LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* ����: ��������LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
		else if (val & (1<<15)) /* eint15, ���ڹ������� */
		{
			if (val2 & (1<<7))
			{

				for (j = 20; j <20+16; j++)
				{
					for (i = 20; i < 480; i++)
					{
						fb_put_pixel(i, j, 0x80FFFF);
					}
				}
				fb_print_string(20, 20, "rphotoresistor  light!!!!!", 0xA0);
			}
			else
			{
				for (j = 20; j <20+16; j++)
				{
					for (i = 20; i < 480; i++)
					{
						fb_put_pixel(i, j, 0x80FFFF);
					}
				}
				fb_print_string(20, 20, "rphotoresistor dark.......", 0xA0);
			}
		}
	}

	EINTPEND = val;
}

void handle_irq_c(void)
{
	/* �ֱ��ж�Դ */
	int bit = INTOFFSET;

	/* ���ö�Ӧ�Ĵ����� */
	irq_array[bit](bit);
	
	/* ���ж� : ��Դͷ��ʼ�� */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);	
}

void register_irq(int irq, irq_func fp)
{
	irq_array[irq] = fp;

	INTMSK &= ~(1<<irq);
}

void unregister_irq(int irq)
{
	INTMSK |= (1<<irq);
}

/* ��ʼ������, ��Ϊ�ж�Դ */
void key_eint_init(void)
{
	/* ����GPIOΪ�ж����� */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3������Ϊ�ж����� */

	GPGCON &= ~((3<<6) | (3<<22));
	GPGCON |= ((2<<6) | (2<<22));   /* S4,S5������Ϊ�ж����� */

	/* ����GPG7Ϊ�ж�����, ���ڹ������� */
	//GPGCON &= ~((3<<14));
	//GPGCON |= ((2<<14));   
	

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* ����GPG7/EINT15Ϊ˫���ش���, ���ڹ������� */
	//EXTINT1 |= (7<<28);

	/* ����EINTMASKʹ��eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));

	/* ʹ���ж�GPG7/EINT15, ���ڹ������� */
	//EINTMASK &= ~((1<<15));

	register_irq(0, key_eint_irq);
	register_irq(2, key_eint_irq);
	register_irq(5, key_eint_irq);
}


void key_uninstall()
{
	EINTMASK |= ((1<<11) | (1<<19));
	unregister_irq(0);
	unregister_irq(2);
	unregister_irq(5);
}

