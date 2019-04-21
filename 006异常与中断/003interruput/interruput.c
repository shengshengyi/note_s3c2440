#include "s3c2440_soc.h"

/* 读EINTPEND分辨率哪个EINT产生(eint4~23)
 * 清除中断时, 写EINTPEND的相应位
 */

/* SRCPND 用来显示哪个中断产生了, 需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK 用来屏蔽中断, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND 用来显示当前优先级最高的、正在发生的中断, 需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTOFFSET : 用来显示INTPND中哪一位被设置为1
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
	unsigned int val = EINTPEND;//清中断用，最后直接赋值回去
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;

	/* eint0 对应按键s2 ，控制gpf6(led4)*/
	if(irq == 0 )
	{	
		if (val1 & (1<<0)) /* s2 --> gpf6 */ /*判断是否为真，即是否有不为0的值，是即松开*/
		{
			/* 松开 */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<6);
		}
	}

	/* eint2 对应按键s3 ，控制gpf5(led2)*/
	else if (irq == 2)
	{
		if (val1 & (1<<2)) /* s3 --> gpf5 */
		{
			/* 松开 */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<5);
		}
	}


	/* eint8_23,eint11对应按键s4,控制gpg4(led1)       ,eint19对应按键s5 ，控制所有led*/
	/* 这里多一层判断
	 * 第一层是判断irq 也就是INTOFFSET，INTPND
	 * 第二层是增加的,因为INTPND(是eint0 eint1 ... enit4-23)所以需要判断eintpend是哪个eint产生的，
	 * 第三层才是按键状态(中断是双边沿触发，是否可以不判断按键状态来点灯)
	 */
	else if(irq == 5)
	{
		if(val & (1<<11))//eint11
		{
			if (val2 & (1<<3)) /* s4 --> gpf4 */
			{
				/* 松开 */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* 按下 */
				GPFDAT &= ~(1<<4);
			}
		}
		
		else if(val & (1<<19))//eint19
		{
		
			if (val2 & (1<<11)) /* s5 --> gpf4 */
			{
				/* 松开 */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* 按下 */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}
	EINTPEND = val;
}

void handle_irq_c(void)
{
	
	/* INTOFFSET : 用来显示INTPND中哪一位被设置为1	
	 * bit0-eint0
	 * bit2-eint2
	 * bit5-eint8_23
	 */
	int bit = INTOFFSET;

	if(bit == 0 || bit == 2 || bit == 5)
	{
		key_eint_irq(bit);
	}

   	/* 清中断
	 * 先清中断源某些寄存器，这个放到处理函数清
	 * 再清SRCPND
	 * 再清INTPND
	 */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);
}

