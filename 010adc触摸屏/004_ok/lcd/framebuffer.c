
#include "lcd.h"
/* 实现画点 */

/* 获得LCD参数 */
static unsigned int fb_base;
static int xres,yres,bpp;

void fb_get_lcd_params(void)
{
	get_lcd_params(&fb_base,&xres,&yres,&bpp);
}

unsigned short convert32bppto16bpp(unsigned int rgb)
{
	int r = (rgb >> 16) & 0xff;	// 0xRR   0xrrrr rrrr
	int g = (rgb >> 8) & 0xff;  // 0xGG   0xgggg gggg
	int b = rgb & 0xff;			// 0xBB   0xbbbb bbbb 

	/* rgb565 */
	r = r >> 3;		//0xrrrr rrrr    0xr rrrr
	g = g >> 2;		//0xgggg gggg    0xgg gggg
	b = b >> 3;		//0xbbbb bbbb    0xb bbbb

	return ((r<<11) | (g<<5) | (b));	//0xrrrr rggg gggb bbbb   rgb565	
}

/* color : 32bit, 0x00RRGGBB
 *
 */

void fb_put_pixel(int x,int y,unsigned int color)
{
	unsigned char 	*pc;	/* 8bpp */
	unsigned short 	*pw;	/* 16bpp */
	unsigned int  	*pdw;	/* 32bpp */

	unsigned int pixel_base = fb_base + (xres * bpp/8)*y + (x * bpp/8);

	switch (bpp)
	{
		case 8:
				pc = (unsigned char *)pixel_base;
				*pc = color;
				break;
		case 16:
				pw = (unsigned short *)pixel_base;
				*pw = convert32bppto16bpp(color);
				break;
		case 32:
				pdw = (unsigned int *)pixel_base;
				*pdw  = color;
				break;
	}
}

void clear_screen(unsigned int color)
{
	int x,y;
	unsigned char* p0;
	unsigned short *p;
	unsigned int *p2;

	if(bpp ==8)
	{
		p0 = (unsigned char*)fb_base;
		for(x = 0;x < xres;x++)
			for(y = 0;y < yres;y++)
				*p0++ = color;
	}

	else if(bpp == 16)
	{
		p = (unsigned short *)fb_base;
		for(x = 0;x < xres;x++)
			for(y = 0;y < yres;y++)
				*p++ = convert32bppto16bpp(color);
	}

	
	if(bpp == 32)
	{
		p2 = (unsigned int *)fb_base;
		for(x = 0;x < xres;x++)
			for(y = 0;y < yres;y++)
				*p2++ = color;
	}
}

