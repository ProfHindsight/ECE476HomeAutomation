/*----------------------------------------------------------------------------
 * Name:   ctp.h
 * Purpose: used for communicating with the app and the onboard UI
 * Note(s):
 *---------------------------------------------------------------------------*/


#ifndef __CTP_H
#define __CTP_H

//Flag to set when an onboard UI update is available
//volatile bool update_user_interface = 0;


//Quick enumeration typedef to keep track of which room to monitor
typedef enum {
	Room1 = 0,
	Room2 = 1,
	Room3 = 2,
	Room4 = 3,
	Average = 4
} temperature_room_selection;


//Structure to store climate control. Mostly for sake of sanity, but can also
//have different users with different preferences.
typedef struct {
	int16_t target_temp;
	int16_t target_temp_hysteresis;
	int16_t hysteresis_high;
	int16_t hysteresis_low;
	temperature_room_selection sensor;
} climate_control_struct;

//Structure to store the current values
typedef struct {
	int16_t temp[4];
	uint8_t lights[4];
	uint8_t heat;
	uint8_t AC;
} chip_values_struct;

typedef enum {
	update_set_temperature,
	update_temperature,
	update_light,
	update_heat,
	update_cooling
} update_app_passthrough;

//Function to initialize inputs and outputs
void ctp_init(void);

//Functions for Matt to call
void change_set_temperature(int16_t temp);
void light_switch(uint8_t light, bool state);
int16_t get_set_temperature(void);
void copy_current_temp(int* send_temp);
void copy_current_lights(uint8_t* lights);
uint8_t get_current_cooling(void);
uint8_t get_current_heating(void);


//Functions for interrupts to call
void check_climate_control(void);
void adc_calculate_temp(void);
void uart_process_command(char* command);

//Functions to keep my sanity. Pass through what was updated and the room number
void update_app(update_app_passthrough change, uint8_t number);


#endif
