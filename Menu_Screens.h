#include "LPC17xx.H" 


unsigned int Menu_Screen(void);

// { target temp, room temps[4], room lights [4], uint8_t heat, uint8_t cool }
void Menu_Screen_Update_Data(int target_temp, int* room_temps, uint8_t* room_lights, uint8_t heat, uint8_t cool);
void Menu_Set_Current_Screen(unsigned int);
unsigned int Menu_Get_Current_Screen(void);
void Menu_Screen_Set_Cursor_Line(unsigned int);
unsigned int Menu_Screen_Get_Cursor_Line(void);
void Menu_Change_Screen(unsigned int);
