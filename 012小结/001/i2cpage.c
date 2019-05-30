
#include "./lcd/geometry.h"
#include "./lcd/font.h"

int i2cpage(void)
{
	unsigned int fb_base;
	int xres, yres, bpp;
	int x, y,pressure;
	int x1, y1;
	int temp1,temp2;



	get_lcd_params(&fb_base, &xres, &yres, &bpp);


	clear_screen(0x80FFFF);
	
	

	/* Êä³öÎÄ×Ö */
	fb_print_string(xres/2 - 8 * 17, yres/2-10, "i2c test .....", 0xA0);
	
	fb_print_string(xres/2 - 8 * 17, yres/2+6, "please control on pc screen", 0xA0);

	i2c_test();
}






