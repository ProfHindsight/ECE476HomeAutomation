
#include "LPC17xx.H" // LPC17xx definitions
#include "JOYSTICK.h"

uint32_t JOYSTICK_MASK = 0x1F; //0x3E;
uint8_t JOYSTICK = 0;
uint8_t prev_JOYSTICK;


void Joystick_Init()
{//Set pins 1.25-1.29 for input
	LPC_GPIO1->FIODIR &= ~((1<<25)|(1<<26)|(1<<27)|(1<<28)|(1<<29));
}

void Run_Joystick()
{
	prev_JOYSTICK = JOYSTICK;
	while(prev_JOYSTICK == JOYSTICK){
		
		JOYSTICK = ~(LPC_GPIO1->FIOPIN >> 25) & JOYSTICK_MASK;
	}
}

uint8_t Check_Joystick()
{
	if(prev_JOYSTICK == JOYSTICK)
		return 1;
	else
		return 0;
}

uint8_t get_joystick()
{//Returns the raw joystick value
	return JOYSTICK;
}

int get_joystick_horz()
{//Returns -1 if the Joystick is pushed left, and +1 if the joystick is pushed right.
	if(prev_JOYSTICK < JOYSTICK)
	{
		if(JOYSTICK & JOY_LEFT)
		{
			return -1;
		}
		else if(JOYSTICK & JOY_RIGHT)
		{
			return 1;
		}
	}
	return 0;
}

int get_joystick_vert()
{//Returns -1 if the Joystick is pushed up, and +1 if the joystick is pushed down.
	if(prev_JOYSTICK < JOYSTICK)
	{
		if(JOYSTICK & JOY_UP)
		{
			return -1;
		}
		else if(JOYSTICK & JOY_DOWN)
		{
			return 1;
		}
	}
	return 0;
}

uint8_t is_joystick(uint8_t d)
{//Checks if the joystick was pushed in direction d.
	if(prev_JOYSTICK < JOYSTICK)
	{
		if(JOYSTICK & d)
		{
			return 1;
		}
	}
	return 0;
}

uint8_t is_joy_left()
{
	if(prev_JOYSTICK < JOYSTICK)
	{
		if(JOYSTICK & JOY_LEFT)
		{
			return 1;
		}
	}
	return 0;
}


uint8_t is_joy_right()
{
	if(prev_JOYSTICK < JOYSTICK)
	{
		if(JOYSTICK & JOY_RIGHT)
		{
			return 1;
		}
	}
	return 0;
}
