/*----------------------------------------------------------------------------
 * Name:    LED.c
 * Purpose: low level LED functions
 * Note(s):
 *----------------------------------------------------------------------------*/

#include "LPC17xx.H" 
#include "LED.h"


// LEDs on the LandTiger board;
const unsigned long led_mask[] = { 1UL<<0, 1UL<<1, 1UL<<2, 1UL<<3,
                                   1UL<<4, 1UL<<5, 1UL<<6, 1UL<<7 };

/*----------------------------------------------------------------------------
  initialize LED Pins
 *----------------------------------------------------------------------------*/
void LED_Init (void) {

  // (1) enable power to GPIO & IOCON
  LPC_SC->PCONP     |= (1 << 15); 

  // (2) P2.0...P2.7 Output LEDs on PORT2 defined as Output
  LPC_GPIO2->FIODIR |= 0x000000ff; 
  
  // (3) Configure the LCD Control pins; is this redundant?
  LPC_GPIO0->FIODIR   |= 0x03f80000;
  LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*----------------------------------------------------------------------------
  Function that turns on requested LED
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int num) {

  LPC_GPIO2->FIOPIN |= led_mask[num];
}

/*----------------------------------------------------------------------------
  Function that turns off requested LED
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int num) {

  LPC_GPIO2->FIOPIN &= ~led_mask[num];
}

/*----------------------------------------------------------------------------
  Function that outputs value to LEDs
 *----------------------------------------------------------------------------*/
void LED_Out(unsigned int value) {
  int i;

  for (i = 0; i < LED_NUM; i++) {
    if (value & (1<<i)) {
      LED_On (i);
    } else {
      LED_Off(i);
    }
  }
}
