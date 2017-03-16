/*----------------------------------------------------------------------------
 * Name:    IRQ.c
 * Purpose: MCB1700 IRQ Handler
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC17xx.H" 
#include "LED.h"
#include "ADC.h"
#include "GLCD.h"
#include "Serial.h"
#include <stdbool.h>
#include "ctp.h"

//Max command length, excluding termination character.
const uint8_t commandLength = 10;

extern uint16_t AD_last[4];  // Last converted value  
extern uint8_t  AD_done;  // AD conversion done flag   
volatile uint32_t ticks;	//Variable to store tics

//Store the last ticks to prevent bouncing
//static uint32_t last_ticks0, last_ticks1, last_ticks2;


//String to export command
char command[commandLength];
//Used to remember where we are at
static uint8_t commandIndex = 0;
//Termination of the command.
const char terminationCharacter = ' ';



//To be determined if command interpretation will be done inside this interrupt.
//Basically errors can happen if two commands are sent in quick succession while
//The LCD is being updated.
void UART2_IRQHandler(void) {
	
	//Record the character. This should clear the flag. No other uart interrupts
	//shoud be enabled. There is a register if you want multiple interrupt
	//sources.
	char temp;
	temp = LPC_UART2->RBR;
	
	
	//If command is done, set the flag and reset command index.
	if (temp == terminationCharacter) {
		//Store it, because the process command function uses it.
		command[commandIndex] = temp;
		uart_process_command(command);
		commandIndex = 0;
	}
	
	//If the command isn't done
	else {
		command[commandIndex++] = temp;
		//Might as well check to not overflow the buffer. Prevent one problem,
		//cause another.
		if(commandIndex >= commandLength) {
			commandIndex = 0;
		}
	}
}



/* Pusbutton 0 interrupt handler */
void EINT0_IRQHandler(void) {
	//Clear the IRQ in the NVIC. Not necessary but good practice
	NVIC_ClearPendingIRQ(EINT0_IRQn);
	
	//Clear the interrupt flag
	LPC_SC->EXTINT = 1 << 0;
	
	/*Pushbutton code here */
	
	
	//Record when this interrupt happened to prevent bouncing
	//last_ticks0 = ticks;
}




/* Pushbutton 1 interrupt handler */
void EINT1_IRQHandler(void) {
	
		//Clear the IRQ in the NVIC. Not necessary but good practice
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	
	//Clear the interrupt flag
	LPC_SC->EXTINT = 1 << 1;
		
	/*Pushbutton code here */
	
	
	//Record when this interrupt happened to prevent bouncing
	//last_ticks1 = ticks;
}

void EINT2_IRQHandler(void) {
	
		//Clear the IRQ in the NVIC. Not necessary but good practice
	NVIC_ClearPendingIRQ(EINT2_IRQn);
	
	//Clear the interrupt flag
	LPC_SC->EXTINT = 1 << 2;
		
	/*Pushbutton code here */
	
	
	//Record when this interrupt happened to prevent bouncing
	//last_ticks2 = ticks;
}
/*----------------------------------------------------------------------------
  SysTick IRQ: Executed periodically
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) // SysTick Interrupt Handler (10ms);
{
  static uint16_t adcTicks = 0;
  // increment ticks.
	ticks++;
	
	//See if 1 second has passed
	if(adcTicks++ >= 500) {
		ADC_StartCnv();
		adcTicks = 0;
	}

}


/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is done
 *----------------------------------------------------------------------------*/
void ADC_IRQHandler(void) 
{
	//Initialize a sweepbit to see which channels we measured
	uint8_t sweepBit = 1, sweep = 0;
	uint8_t i;
	
	//Find the first address for the individual measurements
	const volatile uint32_t *ADCaddress = &LPC_ADC->ADDR0;
	
	//Read the status register to clear the interrupt
	uint32_t adstat = LPC_ADC->ADSTAT;  
	
	//Stop the conversion. This is a different function than the original.
	ADC_StopCnv();
	
  //Sweep through the channels and put them in the last vector
	for(i = 0; i < 4; i++) {
		//If you are on a zero, move left.
		while(!(sweepBit & adstat)) {
			sweepBit = 1 << ++sweep;
		}
		
		//Record that value. 
		AD_last[i] = ADCaddress[sweep] >> 4;
		
		//Make sure to shift left.
		sweepBit = 1 << ++sweep;
	}
	adc_calculate_temp();
	check_climate_control();
	//record conversion finished; this will be seen inside main();
	AD_done = 1;
}
