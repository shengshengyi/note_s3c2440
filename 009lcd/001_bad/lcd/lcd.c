#define LCD_NUM 10

static p_lcd_parmas p_array_lcd[LCD_NUM];
static p_lcd_parmas g_p_lcd_selected;

int register_lcd(p_lcd_parmas plcd)
{
	int i ;
	for(i = 0; i < LCD_NUM;i++)
	{
		if(!p_array_lcd[i])
		{
			p_array_lcd[i] = plcd;
			return i;
		}
	}
	return -1;
}

int select_lcd(char *name)
{
	int i;
	for(i = 0;i < LCD_NUM;i++)
	{
		if(p_array_lcd[i] && !strcmp(p_array_lcd[i]->name,name))
		{
			g_p_lcd_selected = p_array_lcd[i];
			return i;
		}
	}
	return -1;
}

int lcd_init(void)
{
	/* 注册lcd */
	lcd_4_3_add();
	
	/* 注册lcd控制器 */
	lcd_controller_add();

	/* 选择某款lcd */
	select_lcd("lcd_4.3");
	
	/* 选择某款lcd控制器 */
	select_lcd_controller("s3c2440");
	
	/* 使用lcd的参数，初始化lcd控制器 */
	lcd_controller_init(g_p_lcd_selected);
}
