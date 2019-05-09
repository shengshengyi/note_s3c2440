
#define LCD_CONTROLLER_NUM 10

static p_lcd_controller p_array_lcd_controller[LCD_CONTROLLER_NUM];
static p_lcd_controller g_p_lcd_controller_selected;

int register_lcd_controller(p_lcd_controller plcdcon)
{
	int i ;
	for(i = 0;i < LCD_CONTROLLER_NUM;i++)
	{
		if(!p_array_lcd_controller[i])
		{
			p_array_lcd_controller[i] = plcdcon;
			return i;
		}
	}
	return -1;
}

int select_lcd_controller(char *name)
{
	int i;
	for(i = 0;i < LCD_CONTROLLER_NUM;i++)
	{
		/* 传入name来判断数组里哪个是想选中的，逐个找出来设置为选中的 */
		if(p_array_lcd_controller[i] && !strcmp(p_array_lcd_controller[i]->name,name))
		{
			g_p_lcd_controller_selected = p_array_lcd_controller[i];
			return 0;
		}
	}
	return -1;
}
/* 向上: 接收不同LCD的参数
 * 向下: 使用这些参数设置对应的LCD控制器
 */

void lcd_controller_init(p_lcd_parmas plcdparams)
{
	/* 调用2440的LCD控制器的初始化函数 */
	if(g_p_lcd_controller_selected)
	{
		g_p_lcd_controller_selected->init(plcdparams);

		return 0;
	}
	return -1;
}

void lcd_controller_add(void)
{
	s3c2440_lcd_controller_add();
}

