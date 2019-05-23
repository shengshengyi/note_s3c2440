
#include "i2c_controller.h"

#define I2C_CONTROLLER_NUN 10

/* 有一个i2c_controller数组用来存放各种不同芯片大的操作结构体 */
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

/* 根据名字来选择某款i2c控制器 */
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

/* i2c_transfer接口函数 */
int i2c_transfer(i2c_msg msgs,int num)
{
	return p_i2c_con_selected->master_xfer(msgs,num);
}

void i2c_init(void)
{
	/* 注册下面的i2c控制器 */
	s3c2440_i2c_con_add();

	/* 选择某款i2c控制器 */

	/* 调用其init函数 */
}
