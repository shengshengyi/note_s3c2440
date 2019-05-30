
#include "../../s3c2440_soc.h"

/* ʹ��GPG5����dht11��DATA���� */

/* ����GPIO��ȡDHT11������ 
 * 1. ������������18MS�ĵ�����: start�ź�
 * 2. start�źű�Ϊ��, 20-40us֮��, dht11����������ά��80us
      Ȼ������80us: ��Ӧ�ź�
 * 3. ֮���������, ��λ����
 *    bit0 : 50us������, 26-28us������
 *    bit1 : 50us������, 70us������
 * 4. ������40bit: 8bitʪ����������+8bitʪ��С������
                   +8bit�¶���������+8bit�¶�С������
                   +8bitУ���
 */


/* ��ʵ��GPIO�Ļ������� */
static void dht11_data_cfg_as_output(void)
{
	GPGCON &= ~(3<<10);
	GPGCON |= (1<<10);
}

static void dht11_data_cfg_as_input(void)
{
	GPGCON &= ~(3<<10);
}

static void dht11_data_set(int val)
{
	if (val)
		GPGDAT |= (1<<5);
	else
		GPGDAT &= ~(1<<5);
}

static int dht11_data_get(void)
{
	if (GPGDAT & (1<<5))
		return 1;
	else
		return 0;
}


/* ����ʵ��DHT11�Ķ����� */

void dht11_init(void)
{
	dht11_data_cfg_as_output();
	dht11_data_set(1);
	mdelay(2000);
}

static void dht11_start(void)
{
	dht11_data_set(0);
	mdelay(20);
	dht11_data_cfg_as_input();
}

static int dht11_wait_ack(void)
{
	udelay(60);
	return dht11_data_get();
}

static int dht11_recv_byte(void)
{
	int i;
	int data = 0;
	
	for (i = 0; i < 8; i++)
	{
		if (dht11_wait_for_val(1, 1000))
		{
			printf("dht11 wait for high data err!\n\r");
			return -1;
		}
		udelay(40);
		data <<= 1;
		if (dht11_data_get() == 1)
			data |= 1;
		
		if (dht11_wait_for_val(0, 1000))
		{
			printf("dht11 wait for low data err!\n\r");
			return -1;
		}
	}
	
	return data;
}

static int dht11_wait_for_val(int val, int timeout_us)
{
	while (timeout_us--)
	{
		if (dht11_data_get() == val)
			return 0; /* ok */
		udelay(1);
	}
	return -1; /* err */
}


int dht11_read(int *hum, int *temp)
{
	unsigned char hum_m, hum_n;
	unsigned char temp_m, temp_n;
	unsigned char check;	
	
	dht11_start();
	
	if (0 != dht11_wait_ack())
	{
		printf("dht11 not ack, err!\n\r");
		return -1;
	}

	if (0 != dht11_wait_for_val(1, 1000))  /* �ȴ�ACK��Ϊ�ߵ�ƽ, ��ʱʱ����1000us */
	{
		printf("dht11 wait for ack high err!\n\r");
		return -1;
	}

	if (0 != dht11_wait_for_val(0, 1000))  /* ���ݽ׶�: �ȴ��͵�ƽ, ��ʱʱ����1000us */
	{
		printf("dht11 wait for data low err!\n\r");
		return -1;
	}

	hum_m  = dht11_recv_byte();
	hum_n  = dht11_recv_byte();
	temp_m = dht11_recv_byte();
	temp_n = dht11_recv_byte();
	check  = dht11_recv_byte();

	dht11_data_cfg_as_output();
	dht11_data_set(1);

	if (hum_m + hum_n + temp_m + temp_n == check)
	{
		*hum  = hum_m;
		*temp = temp_m;
		mdelay(2000);  /* ��ȡ������2S, ���ܶ�̫Ƶ�� */
		return 0;
	}
	else
	{
		printf("dht11 checksum err!\n\r");
		return -1;
	}

}


void dht11_test(void)
{
	int hum, temp;
	
	dht11_init();
	
	while (1)
	{
		if (dht11_read(&hum, &temp) !=0 )
		{
			printf("\n\rdht11 read err!\n\r");
			dht11_init();
		}
		else
		{
			printf("\n\rDHT11 : %d humidity, %d temperature\n\r", hum, temp);
		}
	}
}


