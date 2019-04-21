#include "s3c2440_soc.h"

/* ��EINTPEND�ֱ����ĸ�EINT����(eint4~23)
 * ����ж�ʱ, дEINTPEND����Ӧλ
 */

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


void interrupt_init(void)
{
	INTMSK &= ~((1 << 0) | (1 << 2) | (1 << 5));
}


void key_eint_init(void)
{
	GPFCON &= ~((3 << 0) | (3 << 4));  
	GPFCON |=  ((2 << 0) | (2 << 4));

	GPGCON &= ~((3 << 6) | (3 << 22));  
	GPGCON |=  ((2 << 6) | (2 << 22));

	EXTINT0 &= ~((7 << 0) | (7 << 8)); 
	EXTINT0 |= ((7 << 0) | (7 << 8)); 

	EXTINT1 &= ~ (7 << 12); 
	EXTINT1 |=   (7 << 12); 

	EXTINT2 &= ~ (7 << 12); 
	EXTINT2 |=   (7 << 12); 

	EINTMASK &= ~((1 << 11) | (1 << 19));

}


void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;//���ж��ã����ֱ�Ӹ�ֵ��ȥ
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;

	/* eint0 ��Ӧ����s2 ������gpf6(led4)*/
	if(irq == 0 )
	{	
		if (val1 & (1<<0)) /* s2 --> gpf6 */ /*�ж��Ƿ�Ϊ�棬���Ƿ��в�Ϊ0��ֵ���Ǽ��ɿ�*/
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

	/* eint2 ��Ӧ����s3 ������gpf5(led2)*/
	else if (irq == 2)
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


	/* eint8_23,eint11��Ӧ����s4,����gpg4(led1)       ,eint19��Ӧ����s5 ����������led*/
	/* �����һ���ж�
	 * ��һ�����ж�irq Ҳ����INTOFFSET��INTPND
	 * �ڶ��������ӵ�,��ΪINTPND(��eint0 eint1 ... enit4-23)������Ҫ�ж�eintpend���ĸ�eint�����ģ�
	 * ��������ǰ���״̬(�ж���˫���ش������Ƿ���Բ��жϰ���״̬�����)
	 */
	else if(irq == 5)
	{
		if(val & (1<<11))//eint11
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
		
		else if(val & (1<<19))//eint19
		{
		
			if (val2 & (1<<11)) /* s5 --> gpf4 */
			{
				/* �ɿ� */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* ���� */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}
	EINTPEND = val;
}

void handle_irq_c(void)
{
	
	/* INTOFFSET : ������ʾINTPND����һλ������Ϊ1	
	 * bit0-eint0
	 * bit2-eint2
	 * bit5-eint8_23
	 */
	int bit = INTOFFSET;

	if(bit == 0 || bit == 2 || bit == 5)
	{
		key_eint_irq(bit);
	}

   	/* ���ж�
	 * �����ж�ԴĳЩ�Ĵ���������ŵ���������
	 * ����SRCPND
	 * ����INTPND
	 */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);
}

