
#include "i2c_controller.h"

#define I2C_CONTROLLER_NUN 10

/* ��һ��i2c_controller����������Ÿ��ֲ�ͬоƬ��Ĳ����ṹ�� */
static p_i2c_conotroller p_i2c_conotrollers[I2C_CONTROLLER_NUN];
static p_i2c_conotroller p_i2c_con_selected;

void register_i2c_controller(p_i2c_conotroller *p)
{
	int i;
	for(i = 0;i < I2C_CONTROLLER_NUN; i++)
	{
		if(!p_i2c_conotrollers[i])
		{
			p_i2c_conotrollers[i] = p;
			return;
		}
	}

}

/* ����������ѡ��ĳ��i2c������ */
void select_i2c_controller(char *name)
{
	int i ;
	for(i = 0;i < I2C_CONTROLLER_NUN; i++)
	{
		if(p_i2c_conotrollers[i] && !strcmp(name,p_i2c_conotrollers[i]->name))
		{
			p_i2c_con_selected = p_i2c_conotrollers[i];
			return 0;
		}
	}
	return -1;
}

/* i2c_transfer�ӿں��� */
int i2c_transfer(i2c_msg msgs,int num)
{
	return p_i2c_con_selected->master_xfer(msgs,num);
}

void i2c_init(void)
{
	/* ע�������i2c������ */
	s3c2440_i2c_con_add();

	/* ѡ��ĳ��i2c������ */

	/* ������init���� */
}
