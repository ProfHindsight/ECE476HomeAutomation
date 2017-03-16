/*----------------------------------------------------------------------------
**                   http://www.PowerAVR.com
**                   http://www.PowerMCU.com
**--------------File Info-----------------------------------------------------
** File name:           adc.c
** Last modified Date:  2010-05-12
** Last Version:        V1.00
** Descriptions:        
**
**----------------------------------------------------------------------------
** Created by:          PowerAVR
** Created date:        2010-05-10
** Version:             V1.00
** Descriptions:        
**
**----------------------------------------------------------------------------*/

#include "lpc17xx.h"   
#include "ADC.h"


uint16_t AD_last[4];     // Last converted value    
uint8_t  AD_done = 0; // AD conversion done flag  


/*----------------------------------------------------------------------------
  initialize ADC Pins
 *----------------------------------------------------------------------------*/
void ADC_Init(uint8_t channels) 
{
	uint8_t sweepBit = 0x80;
  // enable power to ADC 
  LPC_SC->PCONP |= (1<<12);
//Figure out which channels to open.	
	if(channels & (1 << 0)) {
		LPC_PINCON->PINSEL1 &= ~(1UL<<14); 
		LPC_PINCON->PINSEL1 |=  (1UL<<14); // P0.23 is AD0.0
	}
	if(channels & (1 << 1)) {
		LPC_PINCON->PINSEL1 &= ~(1UL<<16); 
		LPC_PINCON->PINSEL1 |=  (1UL<<16); // P0.24 is AD0.1
	}
	if(channels & (1 << 2)) {
		LPC_PINCON->PINSEL1 &= ~(1UL<<18);
		LPC_PINCON->PINSEL1 |=  (1UL<<18); // P0.25 is AD0.2
	}
	if(channels & (1 << 3)) {
		LPC_PINCON->PINSEL1 &= ~(1UL<<20);
		LPC_PINCON->PINSEL1 |=  (1UL<<20); // P0.26 is AD0.3
	}
	if(channels & (1 << 4)) {
		LPC_PINCON->PINSEL3 &= ~(3UL<<28);
		LPC_PINCON->PINSEL3 |=  (3UL<<28); // P1.30 is AD0.4
	}
	if(channels & (1 << 5)) {
		LPC_PINCON->PINSEL3 &= ~(3UL<<30);
		LPC_PINCON->PINSEL3 |=  (3UL<<30); // P1.31 is AD0.5
	}
	if(channels & (1 << 6)) {
		LPC_PINCON->PINSEL0 &= ~(2UL<<06);
		LPC_PINCON->PINSEL0 |=  (2UL<<06); // P0.3 is AD0.6
	}
	if(channels & (1 << 7)) {
		LPC_PINCON->PINSEL0 &= ~(2UL<<04);
		LPC_PINCON->PINSEL0 |=  (2UL<<04); // P0.2 is AD0.7
	}
	
  LPC_ADC->ADCR        =  (channels) |  // select AD0.5 pin 
                          (4<< 8) |  // ADC clock is 25MHz/5 
                          (1<<21);   // enable ADC 
	
	//We need to find the leftmost interrupt to stop the burst scan.
	while(!(sweepBit&channels)) sweepBit = sweepBit >> 1;
	
  LPC_ADC->ADINTEN  = (sweepBit); // global enable interrupt
  // enable ADC as source of interrupts in NVIC
  NVIC_EnableIRQ(ADC_IRQn); // enable ADC Interrupt 
}

/*----------------------------------------------------------------------------
  start ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_StartCnv (void) 
{
	//Start the burst 
	LPC_ADC->ADCR |= 1 << 16;
}


/*----------------------------------------------------------------------------
  stop ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_StopCnv (void) 
{
  LPC_ADC->ADCR &= ~(1<<16); // stop conversion 
}

