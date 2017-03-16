
#include "LPC17xx.H"

#define JOY_UP 0x10
#define JOY_DOWN 0x02
#define JOY_LEFT 0x04
#define JOY_RIGHT 0x08
#define JOY_CENTER 0x01

void Joystick_Init(void);
void Run_Joystick(void);
uint8_t Check_Joystick(void);
uint8_t get_joystick(void);
int get_joystick_horz(void);
uint8_t is_joystick(uint8_t);
int get_joystick_vert(void);
uint8_t is_joy_left(void);
uint8_t is_joy_right(void);








