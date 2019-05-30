
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"
#include "./app/display.h"
#include "./lcd/lcd.h"

int main(void)
{
	int menu_select;

	timer_init();	
	
	lcd_init2();	
	if(is_ts_have() == 1)

	{
		touchscreen_init();
		ts_calibrate_nand();
		//display_test();
		mainpage();
		printf("ts have2");
	}

	else
	{
		printf("ts_not_have\n\r");
		ts_calibrate_first();
		mainpage();
		//display_test();
	}
	




	
	while(1)
	{
		menu_select = read_menu();
		printf("menu_select = %d\n\r",menu_select);

		switch(menu_select)
		{
			case 0:
				ledpage();
				break;
			

			case 1:
				keypage();
				break;
			
			case 2:
				lcdpage();
				break;

			case 3:
				touchscreenpage();
				break;

			case 4:
				i2cpage();
				break;

			case 5:
				tslibpage();
				break;
		}
	
	}
	return 0;
}

