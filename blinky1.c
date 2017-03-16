// blinky1.c
// this program turns on and off the LED labeled LD10 from the 
// group of 8 LED on the TigerLand 2.0 board; this is the 
// hello-world program in the context of this course...

//#include "LPC17xx.H" // LPC17xx definitions
#include <stdbool.h>
#include "JOYSTICK.h"
#include "Menu.h"
#include "Menu_Screens.h"

#include "ADC.h"
#include "IRQ.h"
#include "Serial.h"
#include "ctp.h"

extern chip_values_struct chip;
extern int send_temp[4];
extern bool update_user_interface;


// we'll use delay() as a "software delay" function; it should produce 
// about a second or so of delay depending on CCLK; a volatile variable 
// 'temp' is updated each time in the loop so that the compiler will not 
// optimize away the loop;
volatile uint32_t temp;
void delay( uint32_t del)
{
  uint32_t i;
  for ( i=0; i<del; i++)
    temp = i;
}

int main (void)
{
	int tar_temp = 0;
	int delta = 0;
	int data[5];
	unsigned int cursor_line = 0;
	unsigned int num_lines = 0;

	//uint8_t lights_room1 = 0;
	
  // to use any of the LPC1768 peripherals, the general steps to be followed are:
  // 1. Power Up the peripheral to be used
  // 2. Set the Clock Rate for the peripheral
  // 3. Connect necessary pins using Pin Connect Block
  // 4. Initialize the registers of the peripheral
  // in what follows, we do all these four steps for our application, which
  // is to blink an LED only;

  // (1) power up GPIO
  // in chapter 4: Clocking and Power Control of the LPC17xx manual
  // look for register Power Control for Peripherals register (PCONP);
  // it's on page 64; bit 15 is PGPIO; setting the bit to 1 should 
  // power up the GPIO ports; NOTE: that the default value is 1, 
  // which means GPIO is powered up by default on reset; but the 
  // start up code may modify this and it's good practice to make
  // sure we power up the GPIO ports ourselves;
  LPC_SC->PCONP |= ( 1 << 15 );
  LPC_PINCON->PINSEL3 &= ~((3<<18)|(3<<20)|(3<<22)|(3<<24)|(3<<26));
  // (2) give the right clock to the peripheral
  // in the same chapter 4, look for Peripheral Clock Selection register (PCLKSEL1);
  // it's on page 58; bits 3:2 set the clock divider for GPIO; since we're not 
  // using interrupts, we wont change the default value. 
  // NOTE: PCLK refers to Peripheral Clock and CCLK refers to CPU Clock;
  // PCLK is obtained by dividing CCLK; see table 41 on page 58 of the manual;
  
  // (3) by default, GPIO pins are connected to the external pins;
  // so there is no need to change settings in the pin connect block;
  Joystick_Init();
	Menu_Init();
	ctp_init();
	ADC_Init(0xE8);
	SysTick_Config(SystemCoreClock/100);
	SER_init(2,9600);
  // (4) to see how to initialize ports, goto chapter 9 GPIO (on page 131);
  // let's set the corresponding bit of the register to make P2.1 an output pin;
  //LPC_GPIO2->FIODIR |= 1 << 1; // LED on port P2.1 configured as output;
  // NOTE: with the above, the operation will be with all the other LEDs
  // being turned-on, which may be distracting to the eye; to have all LEDs
  // off, we could use alternatively:
  LPC_GPIO2->FIODIR |= 0x000000FF; // LEDs on PORT2 are output;
  //LPC_GPIO1->FIODIR &= ~((1<<25)|(1<<26)|(1<<27)|(1<<28)|(1<<29));
	//LPC_GPIO1->FIOMASK |= JOYSTICK_MASK;
  // (5) turn on/off the LED driven by P2.1; 
  // NOTE: turning the LED on/off could also be achieved via FIOSET and FIOCLR;
  // in fact, using FIOSET and FIOCLR is the recommended method (as done in
  // blinky3.s example); for now, let's just do it this way in order to
  // show that it can be done in different ways;
  while(1)
	{
		tar_temp = get_set_temperature();
		//Update Data
		
		
		cursor_line = Menu_Screen_Get_Cursor_Line();
		
		num_lines = Menu_Screen();
		
		
		
		Menu_Set_Cursor(cursor_line);
		
		
		Update_Joystick();
		while(Check_Joystick())
		{
			if(update_user_interface){
				Menu_Screen_Update_Data(get_set_temperature(), send_temp, chip.lights, get_heat(), get_cooling());
				update_user_interface = 0;
			}
			Update_Joystick();
		}
		
		if(is_joystick(JOY_UP) && cursor_line > 0)
		{
			cursor_line -= 1;
		}
		if(is_joystick(JOY_DOWN) && cursor_line < num_lines)
		{
			cursor_line += 1;
		}
		
		Menu_Screen_Set_Cursor_Line(cursor_line);
		
		if(is_joystick(JOY_CENTER))
		{
			switch(Menu_Get_Current_Screen())
			{
				case 0:
					switch(cursor_line)
					{
						case 0:
							Menu_Change_Screen(1);
							break;
						case 1:
							Menu_Change_Screen(2);
							break;
					}
					break;
				case 1:
					if(cursor_line == 8)
						Menu_Change_Screen(0);
					break;
				case 2:
					switch(cursor_line)
					{
						case 1:
							light_switch(1,!chip.lights[0]);
						break;
						case 2:
							light_switch(2,!chip.lights[1]);
						break;
						case 3:
							light_switch(3,!chip.lights[2]);
						break;
						case 4:
							light_switch(4,!chip.lights[3]);
						break;
						case 5:
							Menu_Change_Screen(0);
					}
			}
		}
		
		if(is_joy_left() || is_joy_right())
		{
			delta = get_joystick_horz();
			switch(Menu_Get_Current_Screen())
			{
				case 1:
					switch(cursor_line){
						case 0:
							tar_temp += delta;
							change_set_temperature(tar_temp);
						break;
					}
					break;
			}
		}
		
		
		
	}
}
