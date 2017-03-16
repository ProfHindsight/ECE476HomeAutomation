

#include "GLCD.h"
#include "Menu.h"
#include "stdio.h"

const unsigned int menu_x_pos = 32;
const unsigned int menu_y_pos = 32;
unsigned int menu_x_pos_data = menu_x_pos + 120;
unsigned int menu_line_spacing = 24; //Spacing between lines in pixels
unsigned int menu_cursor_line = 0;
unsigned int menu_cursor_current_line = 0;

unsigned int menu_cursor_x = menu_x_pos - 16;
uint8_t on_off[2][5] = {"Off\0","On\0"};

void Menu_Init(void)
{
	LCD_Initialization();
	LCD_Clear(Black);
}

void Menu_Clear()
{
	LCD_Clear(Black);
}

void Menu_Set_Line(uint8_t* menu_item, unsigned int menu_line)
{
	LCD_PutText(menu_x_pos, menu_y_pos + menu_line_spacing * menu_line,  menu_item, White, Black);
}

void Menu_Set_Data(int menu_data, unsigned int menu_line)
{
	char data[4];
	sprintf(data,"%d",menu_data);
	LCD_PutText(menu_x_pos_data, menu_y_pos+menu_line_spacing*menu_line, data, White ,Black);
}

void Menu_Set_TextData(uint8_t* menu_data, unsigned int menu_line)
{
	LCD_PutText(menu_x_pos_data, menu_y_pos+menu_line_spacing*menu_line, "      ", White ,Black);
	LCD_PutText(menu_x_pos_data, menu_y_pos+menu_line_spacing*menu_line, menu_data, White ,Black);
}

void Menu_Set_Cursor(unsigned int menu_line)
{
	LCD_PutChar(menu_cursor_x, menu_y_pos + menu_line_spacing * menu_cursor_current_line, ' ', White, Black);
	LCD_PutChar(menu_cursor_x, menu_y_pos + menu_line_spacing * menu_line, '>', White, Black);
	menu_cursor_current_line = menu_line;
}

void Menu_Set_OnOffData(uint8_t data, unsigned int menu_line)
{
	Menu_Set_TextData(on_off[data],menu_line);
}

void Menu_Change_Data(int menu_data, unsigned int menu_line)
{
	char data[10];
	sprintf(data,"<- %d +>",menu_data);
	LCD_PutText(menu_x_pos_data, menu_y_pos+menu_line_spacing*menu_line, data, White ,Black);
}
