
#include "irda_raw.h"

/*
 * �ӻ��ͻ������л����������,
 * �����ó�address, data
 */

#define DURATION_BASE  563
#define DURATION_DELTA (DURATION_BASE/2)

#define DURATION_HEAD_LOW    (16*DURATION_BASE)
#define DURATION_HEAD_HIGH   (8*DURATION_BASE)
#define DURATION_REPEAT_HIGH (4*DURATION_BASE)
#define DURATION_DATA_LOW    (1*DURATION_BASE)
#define DURATION_DATA1_HIGH  (3*DURATION_BASE)
#define DURATION_DATA0_HIGH  (1*DURATION_BASE)
#define DURATION_END_LOW     (1*DURATION_BASE)

static int duration_in_margin(int duration, int us)
{
	if ((duration > (us - DURATION_DELTA)) && (duration < us + DURATION_DELTA))
		return 1;
	else
		return 0;
}

/*
 * ����ֵ: 0-�õ�����, 1-�õ��ظ���, -1 : ʧ��
 */
int irda_nec_read(int *address, int *data)
{
	irda_raw_event event;	
	int i;
	unsigned int val = 0;
	unsigned char byte[4];

	while (1)
	{
		if (0 == ir_event_get(&event))
		{
			/* �������� */
			/* 1. �ж��Ƿ�Ϊ9MS�ĵ����� */
			if (event.pol == 0 && \
				duration_in_margin(event.duration, DURATION_HEAD_LOW))
			{
				/* ���к����ж� */
				/* 2. ����һ������������ */
				if (0 == ir_event_get_timeout(&event, 10000))
				{
					/* 3. �ж����Ƿ�4.5ms�ĸ�����
					 *    ���� 2.25ms�ĸ�����
					 */
					if (event.pol == 1 && \
						duration_in_margin(event.duration, DURATION_HEAD_HIGH))
					{
						/* 4.5ms�ĸ����� */
						/* 4. �ظ�����32λ���� */
						for (i = 0; i < 32; i++)
						{
							/* 5. ��0.56ms�ĵ����� */
							if (0 == ir_event_get_timeout(&event, 10000))
							{
								if (event.pol == 0 && \
									duration_in_margin(event.duration, DURATION_DATA_LOW))
								{
									/* 6. ����һ������, �ж����� 0.56ms/1.68ms�ĸ����� */
									if (0 == ir_event_get_timeout(&event, 10000))
									{
										if (event.pol == 1 && \
											duration_in_margin(event.duration, DURATION_DATA1_HIGH))
										{
											/* �õ���bit 1 */
											val |= (1<<i);
										}
										else if (event.pol == 1 && \
												duration_in_margin(event.duration, DURATION_DATA0_HIGH))
										{
											/* �õ���bit 0 */
										}
										else
										{
											printf("%s %d\n\r", __FUNCTION__, __LINE__);
											return -1;
										}
									}
									else
									{
										printf("%s %d\n\r", __FUNCTION__, __LINE__);
										return -1;
									}
								}
								else
								{
									printf("%s %d\n\r", __FUNCTION__, __LINE__);
									return -1;
								}
							}
							else
							{
								printf("%s %d\n\r", __FUNCTION__, __LINE__);
								return -1;
							}
						}

						/* 7. �õ���32λ����, �ж������Ƿ���ȷ */
						byte[0] = val & 0xff;
						byte[1] = (val>>8) & 0xff;
						byte[2] = (val>>16) & 0xff;
						byte[3] = (val>>24) & 0xff;

						//printf("get data: %x %x %x %x\n\r", byte[0], byte[1], byte[2], byte[3]);
						byte[1] = ~byte[1];
						byte[3] = ~byte[3];
						
						if (byte[0] != byte[1])
						{
							/* ��Щң��������ȫ����NEC�淶 */
							//printf("%s %d\n\r", __FUNCTION__, __LINE__);
							//return -1;
						}
						if (byte[2] != byte[3])
						{
							printf("%s %d\n\r", __FUNCTION__, __LINE__);
							return -1;
						}
						*address = byte[0];
						*data    = byte[2];
						return 0;
						
					}
					else if (event.pol == 1 && \
						duration_in_margin(event.duration, DURATION_REPEAT_HIGH))
					{
						/* 2.25ms�ĸ����� */
						return 1;  /* �ظ��� */
					}
					else
					{
						printf("%s %d\n\r", __FUNCTION__, __LINE__);
						return -1;  /* ���� */
					}
				}
			}
			else
			{
				//printf("%s %d\n\r", __FUNCTION__, __LINE__);
				return -1; /* ��Ч����δ��ʼ */
			}
		}
	}
}

void irda_nec_test(void)
{
	int address;
	int data;
	int ret;
	
	irda_init();

	while (1)
	{
		ret = irda_nec_read(&address, &data);
		if (ret == 0)
		{
			printf("irda_nec_read: address = 0x%x, data = 0x%x\n\r", address, data);
		}
		else if (ret == 1)
		{
			printf("irda_nec_read: repeat code\n\r");
		}
	}
}

