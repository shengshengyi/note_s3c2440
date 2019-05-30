
#include "../../s3c2440_soc.h"
#include "irda_raw.h"

/* IRDA���� : EINT1/GPF1 */

static unsigned long long g_last_time = 0;

/* 
 * ����GPIO, ע���ж�
 * ���жϴ�������:
      ��¼�жϷ�����ʱ��,
      ���ϴ��жϵ�ʱ��Ƚ�, �����������
      ��ȡ���ż���
      �����ݷ��뻷�ͻ�����
 */

 /* ��ʵ��GPIO�Ļ������� */
static void irda_data_cfg_as_eint(void)
{
	/* ����Ϊ�ж����� */
	GPFCON &= ~(3<<2);
	GPFCON |= (2<<2);

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<4);  /* eint1 */

}

static int irda_data_get(void)
{
	if (GPFDAT & (1<<1))
		return 1;
	else
		return 0;
}

void irda_irq(int irq)
{
	/* ���жϴ�������:
		 ��¼�жϷ�����ʱ��,
		 ���ϴ��жϵ�ʱ��Ƚ�, �����������
		 ��ȡ���ż���
		 �����ݷ��뻷�ͻ�����
	*/
	irda_raw_event event;
	unsigned long long cur = get_system_time_us();
	
	event.duration = delta_time_us(g_last_time, cur);
	event.pol      = !irda_data_get();
	ir_event_put(&event);
	g_last_time = cur;
}


/* ע���ж� */
void irda_init(void)
{
	irda_data_cfg_as_eint();
	register_irq(1, irda_irq);
}

void irda_raw_test(void)
{
	irda_raw_event event;
	unsigned long long pre = 0, cur;
	
	irda_init();

	while (1)
	{
		if (0 == ir_event_get(&event))
		{
			cur = get_system_time_us();
			if (delta_time_us(pre, cur) > 1000000)
				printf("\n\r");
			pre = cur;
			printf("%s %d us | ", event.pol? "hight" : "low", event.duration);
		}
	}
}

