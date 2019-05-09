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
	/* ע��lcd */
	lcd_4_3_add();
	
	/* ע��lcd������ */
	lcd_controller_add();

	/* ѡ��ĳ��lcd */
	select_lcd("lcd_4.3");
	
	/* ѡ��ĳ��lcd������ */
	select_lcd_controller("s3c2440");
	
	/* ʹ��lcd�Ĳ�������ʼ��lcd������ */
	lcd_controller_init(g_p_lcd_selected);
}
