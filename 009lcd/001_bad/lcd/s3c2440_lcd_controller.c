#define HCLK 100

void jz2440_lcd_pin_init(void)
{
	/* gpb0背光引脚 */
	GPBCON &= ~0x03;
	GPBCON |= 0x01;

	/* lcd用引脚 */
	GPCCON = 0xaaaaaaaa;
	GPDCON = 0xaaaaaaaa;

	/* pwren */
	GPGCON |= (3<<8);	
}

void s3c2440_lcd_controller_init(p_lcd_parmas plcdparam)
{
	int pixelplace;
	unsigned int addr;

	jz2440_lcd_pin_init();

	/* [17:8]: clkval, vclk = HCLK / [(CLKVAL+1) x 2]
	 *                   9   = 100M /[(CLKVAL+1) x 2], clkval = 4.5 = 5
	 *                 CLKVAL = 100/vclk/2-1
	 * [6:5]: 0b11, tft lcd
	 * [4:1]: bpp mode
	 * [0]  : LCD video output and the logic enable/disable
	 */
	int clkval = (double) HCLK/plcdparam->time_seq.vclk/2 - 1 + 0.5;
	int bppmode = plcdparam->bpp == 8 ? 0xb	 :\
				  plcdparam->bpp == 16 ? 0xc :\
				  0xd;/* 24bpp */
	LCDCON1 = (clkval<<8) | (3<<5) | (bppmode<<1);


	/* [31:24] : VBPD    = tvb - 1
	 * [23:14] : LINEVAL = line - 1
	 * [13:6]  : VFPD    = tvf - 1
	 * [5:0]   : VSPW    = tvp - 1
	 */
	 LCDCON2 = ((plcdparam->time_seq.tvb - 1)<<24)	|\
	 		   ((plcdparam->yres - 1)<<14)			|\
	 		   ((plcdparam->time_seq.tvf - 1)<<6)	|\
	 		   ((plcdparam->time_seq.tvp - 1)<<0);


	/* [25:19] : HBPD	= thb - 1
	 * [18:8]  : HOZVAL	= 列 - 1
	 * [7:0]   : HFPD	= thf - 1
	 */
	 LCDCON3 = ((plcdparam->time_seq.thb - 1)<<19)	|\
	 		   ((plcdparam->xres - 1)<<8)			|\
	 		   ((plcdparam->time_seq.thf - 1)<<0);

    /* 
     * [7:0]   : HSPW	= thp - 1
	 */
	 LCDCON4 = ((plcdparam->time_seq.thp - 1)<<0);


	 /* 用来设置引脚极性, 设置16bpp, 设置内存中象素存放的格式
	  * [12] : BPP24BL
	  * [11] : FRM565, 1-565
	  * [10] : INVVCLK, 0 = The video data is fetched at VCLK falling edge
	  * [9]  : HSYNC是否反转
	  * [8]  : VSYNC是否反转
	  * [7]  : INVVD, rgb是否反转
	  * [6]  : INVVDEN
	  * [5]  : INVPWREN
	  * [4]  : INVLEND
	  * [3]  : PWREN, LCD_PWREN output signal enable/disable
	  * [2]  : ENLEND
	  * [1]  : BSWP
	  * [0]  : HWSWP
	  */

	 pixelplace = plcdparam->bpp == 24 ? (0): |\
	 			  plcdparam->bpp == 16 ? (1): |\
	 			  (1<<1);//8BPP

	 LCDCON5 = (plcdparam->pins_pol.vclk<<10) |\
	 		   (plcdparam->pins_pol.hsync<<9) |\
	 		   (plcdparam->pins_pol.vsync<<8) |\
	 		   (plcdparam->pins_pol.rgb<<7)	  |\
	 		   (plcdparam->pins_pol.de<<6)	  |\
	 		   (plcdparam->pins_pol.pwren<<5) |\
	 		   (1<<11) | pixelplace;
	 		   


     /* framebuffer地址 */
     /*
      * [29:21] : LCDBANK, A[30:22] of fb
	  * [20:0]  : LCDBASEU, A[21:1] of fb
	  */
	  addr = plcdparam->fb_base & ~(1<<31);
	 LCDSADDR1 = (addr >> 1);

	 /* 
	  * [20:0] : LCDBASEL, A[21:1] of end addr
	  */
	 addr = plcdparam->fb_base + plcdparam->xres * plcdparam->yres * plcdparam->bpp/8;
	 addr >>= 1;
	 addr &= 0x1fffff;
	 LCDSADDR2 = addr;
		   
}

void s3c2440_lcd_controller_enable(void)
{
	/* 背光引脚 */
	GPBCON 	|= (1<<0);
	
	/* pwren	: 给LCD提供AVDD    */
	LCDCON5 |= (1<<3);
	
	/* LCDCON1'BIT 0 : 设置LCD控制器是否输出信号	*/
	LCDCON1 |= (1<<0);
}

void s3c2440_lcd_controller_disable(void)
{
	/* 背光引脚 */
	GPBCON 	&= ~(1<<0); 
	
	/* pwren	: 给LCD提供AVDD	  */
	LCDCON5 &= ~(1<<3);
	
	/* LCDCON1'BIT 0 : 设置LCD控制器是否输出信号 */
	LCDCON1 &= ~(1<<0);
}

struct lcd_controller s3c2440_lcd_controller = {
	.name    = "s3c2440",
	.init = s3c2440_lcd_controller_init,
	.enable = s3c2440_lcd_controller_enable,
	.disable = s3c2440_lcd_controller_disable,

};

void s3c2440_lcd_controller_add(void)
{
	register_lcd_controller(&s3c2440_lcd_controller);
}
