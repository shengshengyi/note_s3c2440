

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

	/* ����gpf4 5 6Ϊ��������ڵ�� */
	*GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	*GPFCON |=  ((1 << 8) | (1 << 10) | (1 << 12));

	
	/* ����3������gpf0��s2�� gpf2��s3�� gpg3��s4��Ϊ���� */
	*GPFCON &= ~((3 << 0) | (3 << 4));
	*GPGCON &= ~((3 << 6));
	
	while(1)
	{
		val1 = *GPFDAT;
		val2 = *GPGDAT;

		/* (gpf0)s2->gpf6 */
		/* ���½ӵ� ��0 & 1 = 0   1 & 1 = 1  �ж��ɿ�*/
		if(val1 & (1<<0))//�ɿ�
		{
			*GPFDAT |= (1<<6); /* led�ӵ磬�ߵ�ƽ�� */
		}
		else
		{
			*GPFDAT &= ~(1<<6); /* led�ӵ磬�͵�ƽ�� */
		}

		
		/* (gpf2)s3->gpf5 */
		if(val1 & (1<<2))//�ɿ�
		{
			*GPFDAT |= (1<<5); /* led�ӵ磬�ߵ�ƽ�� */
		}
		else
		{
			*GPFDAT &= ~(1<<5); /* led�ӵ磬�͵�ƽ�� */
		}


		/* (gpg3)s4->gpf4 */
		if(val2 & (1<<3))//�ɿ�
		{
			*GPFDAT |= (1<<4); /* led�ӵ磬�ߵ�ƽ�� */
		}
		else
		{
			*GPFDAT &= ~(1<<4); /* led�ӵ磬�͵�ƽ�� */
		}
	}
	return 0 ;
}
