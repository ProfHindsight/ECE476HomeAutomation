/*----------------------------------------------------------------------------
 * Name:    ctp.c
 * Purpose: function to store all my things and things people need
 * Note(s):
 *----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
 
#include <stdio.h>
#include "LPC17xx.H" 
#include "LED.h"
#include "ADC.h"
#include "GLCD.h"
#include "Serial.h"
#include <stdbool.h>
#include "ctp.h"
#include "Menu_Screens.h"


static char text[10];					//Used for sending data/commands
extern uint16_t AD_last[4];  	// Last converted values   
extern uint8_t  AD_done;  		// AD conversion done flag  
static int16_t temp_last[4];	//Used to see if temperature changed.
extern const uint8_t commandLength;
volatile bool update_user_interface = 0;	//Flag to be used to update the user interface
static const int32_t q15_temp_coefs[2] = {-15099,56146609};
static const uint8_t light_pin_map[4] = {21, 23, 20, 24};


//Store the values in the variable CHIP
static chip_values_struct chip = {
	{0,0,0,0},							//temp
	{0,0,0,0},							//lights
	0,											//heating
	0												//cooling
};


//Typedef to keep track of which screen we are on
typedef enum {
	app_screen_main = 0,
	app_screen1 = 1,
	app_screen2 = 2,
	app_screen3 = 3,
	app_screen4 = 4
} app_screens;
app_screens app_current_screen = app_screen_main;

typedef enum {
	ob_screen_main = 0,
	ob_screen1 = 1,
	ob_screen2 = 2
} onboard_screens;
onboard_screens current_ob_screen = ob_screen_main;


static climate_control_struct user = {
	700,					//target temp
	20,					//hysteresis
	720,					//hysteresis upper
	680,					//hysteresis lower
	Room2				//Room to measure
};


/*Get the current set temperature for climate control*/
int16_t get_set_temperature(void) {
	return user.target_temp / 10;
}

/*Get the current temperatures stored on the chip*/
void copy_current_temp(int* send_temp){
	send_temp[0] = chip.temp[0]/10;
	send_temp[1] = chip.temp[1]/10;
	send_temp[2] = chip.temp[2]/10;
	send_temp[3] = chip.temp[3]/10;
}

/*Get the current light statuses */
void copy_current_lights(uint8_t* lights){
	lights[0] = chip.lights[0];
	lights[1] = chip.lights[1];
	lights[2] = chip.lights[2];
	lights[3] = chip.lights[3];
}

/*Get the status of the AC. 1 is on, 0 is off*/
uint8_t get_current_cooling(void) {
	return chip.AC;
}

/*Get the status of the heat. 1 is on, 0 is off*/
uint8_t get_current_heating(void){
	return chip.heat;
}


void ctp_init(void) {
	
	//Clear the output pins first
	LPC_GPIO1->FIOCLR = (3 << 20) | (3 << 23);
	LPC_GPIO3->FIOCLR = (3 << 25);
	
	//Set them as GPIO
	//light1 pin1.21
	LPC_PINCON->PINSEL3 &= ~(0UL<<10); 
	LPC_PINCON->PINSEL3 |=  (0UL<<10); 
	//light2 pin1.23
	LPC_PINCON->PINSEL3 &= ~(0UL<<14); 
	LPC_PINCON->PINSEL3 |=  (0UL<<14); 
	//light3 pin1.20
	LPC_PINCON->PINSEL3 &= ~(0UL<<8); 
	LPC_PINCON->PINSEL3 |=  (0UL<<8); 
	//light3 pin1.24
	LPC_PINCON->PINSEL3 &= ~(0UL<<16); 
	LPC_PINCON->PINSEL3 |=  (0UL<<16); 
	//heat pin3.25
	LPC_PINCON->PINSEL7 &= ~(0UL<<18); 
	LPC_PINCON->PINSEL7 |=  (0UL<<18); 
	//AC pin3.26
	LPC_PINCON->PINSEL7 &= ~(0UL<<20); 
	LPC_PINCON->PINSEL7 |=  (0UL<<20); 

	
	//Finally, set the pins as outputs.
	LPC_GPIO1->FIODIR |= (3 << 20) | (3 << 23);
	LPC_GPIO3->FIODIR |= (3 << 25);
}
//Check to see if we need to update the app. We passthrough the change and the 
//room number if need be.
void update_app(update_app_passthrough change, uint8_t number) {
	
	update_user_interface = 1;
	
	switch(change) {
		
		//The set temperature changed
		case update_set_temperature:
				sprintf(text, "s:%d ", user.target_temp / 10);
				SER_putString(2, text);
			break;
		
		//The temperature measurement changed
		case update_temperature:
			if((app_current_screen == app_screen_main) | (app_current_screen == number)) {
				sprintf(text, "t%d:%d ",number,  (chip.temp[number-1] / 10));
				SER_putString(2, text);
			}
			break;
			
			//One of the lights changed
		case update_light:
			if((app_current_screen == app_screen_main) | (app_current_screen == number)) {
				sprintf(text, "l%d:%d ",number, chip.lights[number-1]);
				SER_putString(2, text);
			}
			break;
			
		case update_heat:
			sprintf(text, "h:%d ", chip.heat);
			SER_putString(2, text);
			break;
		
		case update_cooling:
			sprintf(text, "c:%d ", chip.AC);
			SER_putString(2, text);
			break;
	}
			
}
/*Change this function later so it talks in degrees F instead of numbers */
void change_set_temperature(int16_t temp) {
	temp = temp * 10;
	user.target_temp = temp;
	user.hysteresis_high = temp + user.target_temp_hysteresis;
	user.hysteresis_low = temp -  user.target_temp_hysteresis;
	update_app(update_set_temperature, 0); 
}

/*Another function to modify. Iron this out when you know the conversion */
void change_temperature_hysteresis(int16_t hyst) {
	user.target_temp_hysteresis = hyst;
	user.hysteresis_high = user.target_temp + user.target_temp_hysteresis;
	user.hysteresis_low = user.target_temp -  user.target_temp_hysteresis;
}


/*Function to turn on a light. */
void light_switch(uint8_t light, bool state) {
	

	//Check to see if we are already there
	if(chip.lights[light-1] == state) return;
	
	//If you turned on a light that isn't supported, return 
	if(light > 4) return;
	

	//Else, output the state of the light requested
	if(state) {
		LPC_GPIO1->FIOSET = ( 1 << light_pin_map[light - 1]);
	}
	else {
		LPC_GPIO1->FIOCLR = ( 1 << light_pin_map[light - 1]);
	}

	
	//Update the current values structure. 
	chip.lights[light-1] = state;
	
	//Update the app of the change. 
	update_app(update_light, light);
}

/* This function switches the state of the heating. Will fail if it tries to 
turn both the heating and cooling on.*/
bool heat_switch(bool state) {

	//Check to see if we are already there
	if(chip.heat == state) return 1;
	
	if(state) {
		
		//Do an AC check.
		if(!chip.AC) {
			LPC_GPIO3->FIOSET = 1 << 25;
		}
		else return 0;
	}
	else {
			LPC_GPIO3->FIOCLR = 1 << 25;
	}
	
	//Update the current values structure.
	chip.heat = state;
	
	//Notify the app of the heating change
	update_app(update_heat, 0);
	return 1;
}

/* This function switches the state of the heating. Will fail if it tries to 
turn both the heating and cooling on.*/
bool AC_switch(bool state) {

	//Check to see if we are already there
	if(chip.AC == state) return 1;
	
	
	if(state) {
		
		//Do a heat check.
		if(!chip.heat) {
			
			//Turn on the AC
			LPC_GPIO3->FIOSET = 1 << 26;
		}
		else return 0;
	}
	
	else {
		
		//Turn off the AC
		LPC_GPIO3->FIOCLR = 1 << 26;
	}
	
	//Update the current values structure.
	chip.AC = state;
	
	//Notify the app of the heating change
	update_app(update_cooling, 0);
	return 1;
}

/*Need to calculate the temperature and see if we need to update the app/UI */
void adc_calculate_temp(void) {
	uint8_t i;
	for(i = 0; i < 4; i++){
		
		//Use Y=AX+B to approximate the temperature around room temperature
		chip.temp[i] = (q15_temp_coefs[0] * AD_last[i] + q15_temp_coefs[1]) >> 15;
		
		//Check to see if it made a difference. Don't like /10 everywhere, but whatev
		if((chip.temp[i]/10) != (temp_last[i]/10)) {
			update_app(update_temperature, i+1);
		}
		
		//Finally record the last value for next time
		temp_last[i] = chip.temp[i];
	}
}


/*Function to check to see if the temperature is outside the bounds, then do 
something. */
void check_climate_control(void) {
	
	/*Choose which temperature to compare against */
	int16_t measured_temp;
	if(user.sensor == 4) {
		measured_temp = (chip.temp[0] + chip.temp[1] + chip.temp[2] + chip.temp[3]) >> 2;
	}
	else measured_temp = chip.temp[user.sensor];
	
	//Check to see if the AC brought the temp down to the target
	if((measured_temp < user.target_temp) & chip.AC) {
		AC_switch(0);
	}
	
	//Check to see if the heat brought the temp up to the target
	if((measured_temp > user.target_temp) & chip.heat) {
		heat_switch(0);
	}
	
	//Check to see if we need to turn the heat on
	if((measured_temp < user.hysteresis_low) &  !chip.heat) {
		heat_switch(1);
	}
	
	//Check to see if we need to turn the AC on
	if((measured_temp > user.hysteresis_high)& !chip.AC) {
		AC_switch(1);
	}
}

/*Function to process the UART command received from the BLE module. These 
should be set commands, so no messing with these without changing the app
code too. Look in the drive for appropriate commands */

void uart_process_command(char* command) {
	int16_t number;
	bool state;
	uint8_t index;
	switch(command[0]) {
		
		//Expecting ln:b where n is an int and b is binary
		case 'l':
			number = command[1] - '0';
			state = command[3] - '0';
			light_switch(number, state);
		break;
		
		//Expecting a query of a room. update the screen
		case 'q':
			//If just a q was sent, update everything
			if(command[1] == ' '){ app_current_screen = app_screen_main;
					update_app(update_cooling,0);
					update_app(update_heat,0);
					for(number = 1; number < 5; number++) {
						update_app(update_light, number);
						update_app(update_temperature, number);
						update_app(update_set_temperature, 0);
					}
				}
			else { 
				number = command[1] - '0';
				if(number == 1) app_current_screen = app_screen1;
				if(number == 2) app_current_screen = app_screen2;
				if(number == 3) app_current_screen = app_screen3;
				if(number == 4) app_current_screen = app_screen4;
				update_app(update_cooling,0);
				update_app(update_heat,0);
				update_app(update_light, number);
				update_app(update_temperature, number);
				update_app(update_set_temperature, 0);
				}
		break;
		
				//Expecting a set temperature s:dd in degrees fahrenheit (x10)
		case 's' :
			//Number to store what we read
			number = 0;
		
			//Index of the command
			index = 2;
		
			//State will be used to determine sign
			state = 0;
		
			while(command[index] != ' '){
				
				if(command[index] == '.') break;
				if(command[index] == '-'){state = 1; index++;}
					
				
				number = (number * 10) + command[index++] - '0';
				if(number > 150) break;
			}
			
			//Clip the max temp to 150. It's a good number, brent
			if(number > 150) number = 150;
			
			//We have the number, now add another digit for the chip
			number = number * 10;
			if(state) number = -number;
			
			//Check to see if the negative number caused the number to be too small.
			//This is after the scale x10
			if(number <= 350) number = 350;
			
			change_set_temperature(number);
		break;
		
	}
		//reset the command for kicks and giggles.
		for(number = 0; number < commandLength; number++) command[number] = 0;
}
