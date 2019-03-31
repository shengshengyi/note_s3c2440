

void delay(volatile int d)
{
	while(d--);
}

int main(void)
{	
	int val1,val2;
	
	unsigned int *GPFCON = (unsigned int*)0x56000050;
	unsigned int *GPFDAT = (unsigned int*)0x56000054;

	unsigned int *GPGCON = (unsigned int*)0x56000060;
	unsigned int *GPGDAT = (unsigned int*)0x56000064;

	/* 配置gpf4 5 6为输出，用于点灯 */
	*GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	*GPFCON |=  ((1 << 8) | (1 << 10) | (1 << 12));

	
	/* 配置3个按键gpf0（s2） gpf2（s3） gpg3（s4）为输入 */
	*GPFCON &= ~((3 << 0) | (3 << 4));
	*GPGCON &= ~((3 << 6));
	
	while(1)
	{
		val1 = *GPFDAT;
		val2 = *GPGDAT;

		/* (gpf0)s2->gpf6 */
		/* 按下接地 ，0 & 1 = 0   1 & 1 = 1  判断松开*/
		if(val1 & (1<<0))//松开
		{
			*GPFDAT |= (1<<6); /* led接电，高电平灭 */
		}
		else
		{
			*GPFDAT &= ~(1<<6); /* led接电，低电平亮 */
		}

		
		/* (gpf2)s3->gpf5 */
		if(val1 & (1<<2))//松开
		{
			*GPFDAT |= (1<<5); /* led接电，高电平灭 */
		}
		else
		{
			*GPFDAT &= ~(1<<5); /* led接电，低电平亮 */
		}


		/* (gpg3)s4->gpf4 */
		if(val2 & (1<<3))//松开
		{
			*GPFDAT |= (1<<4); /* led接电，高电平灭 */
		}
		else
		{
			*GPFDAT &= ~(1<<4); /* led接电，低电平亮 */
		}
	}
	return 0 ;
}
