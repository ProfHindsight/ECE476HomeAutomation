#include "LPC17xx.H"

void Menu_Init(void);
void Menu_Clear(void);
void Menu_Set_Line(uint8_t* menu_item, unsigned int menu_line);
void Menu_Set_Data(int,unsigned int);
void Menu_Set_TextData(uint8_t* menu_data,unsigned int menu_line);
void Menu_Set_Cursor(unsigned int menu_line);
void Menu_Set_OnOffData(uint8_t, unsigned int menu_line);
void Menu_Change_Data(int,unsigned int);


