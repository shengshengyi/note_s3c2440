
#include "./lcd/geometry.h"
#include "./lcd/font.h"
#include "s3c2440_soc.h"
int tslibpage(void)
{
	unsigned int fb_base;
	int xres, yres, bpp;
	int x, y,pressure;
	int x1, y1;
	int temp1,temp2;


	ts_calibrate_first();

	
	get_lcd_params(&fb_base, &xres, &yres, &bpp);




	/* »­Ô² */
	draw_circle(xres-60, yres-60 , 40, 0x800000);
	fb_print_string(xres-60-24, yres-60-8, "Return", 0xA0);

	
	while (1)
	{
		if (ts_read(&x, &y, &pressure) == 0)
		{

			if (pressure)
			{

				do {
					ts_read_raw(&x1, &y1, &pressure); 
				} while (pressure);
				
				temp1 = x - (xres-60);
				temp2 = y - (yres-60);
				if( (temp1 * temp1 + temp2 * temp2) - 40*40 < 0 )
				{
					mainpage();
					return 0;
				}

			}
		}
	}
}







