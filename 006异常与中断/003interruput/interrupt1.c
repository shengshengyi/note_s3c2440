#include "s3c2440_soc.h"


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

/* 初始化中断控制器 */
void interrupt_init(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
}

/* 初始化按键, 设为中断源 */
void key_eint_init(void)
{
	/* 配置GPIO为中断引脚 */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3被配置为中断引脚 */

	GPGCON &= ~((3<<6) | (3<<22));
	GPGCON |= ((2<<6) | (2<<22));   /* S4,S5被配置为中断引脚 */
	

	/* 设置中断触发方式: 双边沿触发 */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* 设置EINTMASK使能eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));
}

/* 读EINTPEND分辨率哪个EINT产生(eint4~23)
 * 清除中断时, 写EINTPEND的相应位
 */


void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;
	
	if (irq == 0) /* eint0 : s2 控制 D12 */
	{
		/*
			关于这句判断终于弄懂了
			这里是要判断按键是松开还是按下，然后根据按键的状态赋值
			这里首先判断它是否松开，怎么判断呢，看GPFDAT的定义，若为输出，则GPFDAT和对应位相同，即GPFDAT=eint0
			进行判断，可以看成是GPFDAT与上GPFDAT对应位的状态，若两者相同则1，为松开
			看一下GPFDAT的定义：当端口被配置为输入端口时，相应的位就是引脚状态。
								当端口被配置为输出端口时，引脚状态与对应位相同。现在是这个，对应位是eint0 
								当端口配置为功能引脚时，将读取未定义的值。
			算了不懂cao，反正就是判断按键状态的
		*/
		if (val1 & (1<<0)) /* s2 --> gpf6 */
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
	else if (irq == 2) /* eint2 : s3 控制 D11 */
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
	/*
	第一层是判断INTOFFSET，INTPND
	第二层是读EINTPEND分辨率哪个EINT产生(eint4~23)	
	第三层是判断按键状态
 	*/
	else if (irq == 5) /* eint8_23, eint11--s4 控制 D10, eint19---s5 控制所有LED */
	{
		if (val & (1<<11)) /* eint11 */
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
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* 松开 */
				/* 熄灭所有LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* 按下: 点亮所有LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}

	
	/* 读EINTPEND分辨率哪个EINT产生(eint4~23)
	 * 清除中断时, 写EINTPEND的相应位
	 */
	EINTPEND = val;
}


void handle_irq_c(void)
{
	/* INTOFFSET : 用来显示INTPND中哪一位被设置为1
		EINT0 ：0
		EINT2 ：2
		EINT8_23 ：5
 	*/
	int bit = INTOFFSET;

	/* 调用对应的处理函数 */
	if (bit == 0 || bit == 2 || bit == 5)  /* eint0,2,eint8_23 */
	{
		key_eint_irq(bit); /* 处理中断, 清中断源EINTPEND */
	}

	/* 清中断 : 从源头开始清 */
	
	/* SRCPND 用来显示哪个中断产生了, 需要清除对应位
	 * bit0-eint0
 	* bit2-eint2
 	* bit5-eint8_23
 	*/
	SRCPND = (1<<bit);
	/* INTPND 用来显示当前优先级最高的、正在发生的中断, 需要清除对应位
 	* bit0-eint0
	* bit2-eint2
 	* bit5-eint8_23
 	*/
	INTPND = (1<<bit);	
}

