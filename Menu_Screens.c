
#include "Menu.h"

unsigned int num_lines = 0;
unsigned int menu_current_screen,menu_screen_cursor_line;

unsigned int Menu_Screen()
{
	switch(menu_current_screen)
	{
		case 0:
			Menu_Set_Line("Temperature >",0);
			Menu_Set_Line("Lighting >", 1);
			num_lines = 2;
		break;
		case 1:
			Menu_Set_Line("Set Temp: ",0);
			Menu_Set_Line("Temperatures:",1);
			Menu_Set_Line(" Room 1:",2);
			Menu_Set_Line(" Room 2:",3);
			Menu_Set_Line(" Room 3:",4);
			Menu_Set_Line(" Room 4:",5);
			Menu_Set_Line("Heating: ", 6);
			Menu_Set_Line("Cooling: ", 7);
			Menu_Set_Line("Back <",8);
			num_lines = 9;
		break;
		case 2:
			Menu_Set_Line("Lighting",0);
			Menu_Set_Line(" Room 1:",1);
			Menu_Set_Line(" Room 2:",2);
			Menu_Set_Line(" Room 3:",3);
			Menu_Set_Line(" Room 4:",4);
			Menu_Set_Line("Back <",5);
			num_lines = 6;
		break;
	}
	return num_lines - 1;
}

void Menu_Screen_Update_Data(int target_temp, int* room_temps, uint8_t* room_lights, uint8_t heat, uint8_t cool)
{
	switch(menu_current_screen)
	{
		case 0:
			//No data to display
		break;
		case 1:
			Menu_Change_Data(target_temp,0);
			Menu_Set_Data(room_temps[0],2);
			Menu_Set_Data(room_temps[1],3);
			Menu_Set_Data(room_temps[2],4);
			Menu_Set_Data(room_temps[3],5);
			Menu_Set_OnOffData(heat,6);
			Menu_Set_OnOffData(cool,7);
		break;
		case 2:
			Menu_Set_OnOffData(room_lights[0],1);
			Menu_Set_OnOffData(room_lights[1],2);
			Menu_Set_OnOffData(room_lights[2],3);
			Menu_Set_OnOffData(room_lights[3],4);
		break;
	}
}

void Menu_Set_Current_Screen(unsigned int screen)
{
	menu_current_screen = screen;
}

unsigned int Menu_Get_Current_Screen()
{
	return menu_current_screen;
}

void Menu_Screen_Set_Cursor_Line(unsigned int cursor_line)
{
	menu_screen_cursor_line = cursor_line;
}

unsigned int Menu_Screen_Get_Cursor_Line()
{
	return menu_screen_cursor_line;
}

void Menu_Change_Screen(unsigned int current_screen)
{
	Menu_Clear();
	Menu_Set_Current_Screen(current_screen);
	Menu_Screen_Set_Cursor_Line(0);
}
