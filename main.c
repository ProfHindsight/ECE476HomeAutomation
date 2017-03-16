/*----------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: ECE476 Final Project
 * Note(s): In progress.
 *            
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "LPC17xx.H" 

#include "LED.h"
#include "ADC.h"
#include "GLCD.h"
#include "Serial.h"
#include <stdbool.h>
#include "ctp.h"

#define ADCchannels 			0xE8




extern uint16_t AD_last[4];  // Last converted values   
extern uint8_t  AD_done;  // AD conversion done flag  
extern uint32_t ticks;		//System ticks

extern chip_values_struct chip;


void pushbutton_init(void) {
	//Set the pin to the EINT0 location
	LPC_PINCON->PINSEL4 |= (01 <<20);
	//Set the interrupt to be edge sensitive
	LPC_SC->EXTMODE |= 1 << 0;
	//Clear the EINT0 interrupt
	NVIC_ClearPendingIRQ(EINT0_IRQn);
	//Enable the EINT0 interrupt
	NVIC_EnableIRQ(EINT0_IRQn);
	//Set the pin to the EINT1 location
	LPC_PINCON->PINSEL4 |= (01 <<22);
	//Set the interrupt to be edge sensitive
	LPC_SC->EXTMODE |= 1 << 1;
	//Clear the EINT1 interrupt
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	//Enable the EINT1 interrupt
	NVIC_EnableIRQ(EINT1_IRQn);
	//Set the pin to the EINT2 location
	LPC_PINCON->PINSEL4 |= (01 <<24);
	//Set the interrupt to be edge sensitive
	LPC_SC->EXTMODE |= 1 << 2;
	//Clear the EINT2 interrupt
	NVIC_ClearPendingIRQ(EINT2_IRQn);
	//Enable the EINT2 interrupt
	NVIC_EnableIRQ(EINT2_IRQn);
}

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) 
{
   
  // (1) initializations;
  SystemInit();
  
	
	//Initialize these four channels. This is a different adc init function than usual,
	//and behaves much differently. Ask me about it. It uses the systick to initiate
	//a sequence of conversions @ 10Hz
	ADC_Init(ADCchannels);
	
  // Generate interrupt each 10 ms;
  SysTick_Config(SystemCoreClock/100);  
  
  // LED Initialization
  LED_Init(); 
	
	// Pushbutton Interrupt Initialization
  pushbutton_init();
	
	ctp_init();
  
  // UART2 initialization
  SER_init(2, 9600); // uart number, baud rate
	
  while (1) {

  }
 
}
