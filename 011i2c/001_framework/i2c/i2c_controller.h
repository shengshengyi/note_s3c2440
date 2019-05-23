

#ifndef _I2C_CONTROLLER_H
#define _I2C_CONTROLLER_H

typedef struct i2c_msg{
	unsigned int addr; /* 7bit */
	int flags;	/* 0 - write, 1 - read */
	int len;
	int cnt_transferred;
	unsigned char* buf;

}i2c_msg,*p_i2c_msg;

typedef struct i2c_controller{
	int (*int)(void);
	int (*master_xfer)(i2c_msg msgs,int num);
	char *name;
}i2c_conotroller,*p_i2c_conotroller;


#endif

