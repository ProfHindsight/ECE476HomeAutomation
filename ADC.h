/*----------------------------------------------------------------------------
**                   http://www.PowerAVR.com
**                   http://www.PowerMCU.com
**--------------File Info-----------------------------------------------------
** File name:           adc.h
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

#include "LPC17xx.H" 
#ifndef __ADC_H
#define __ADC_H

#define ADC_VALUE_MAX 0xFFF

void     ADC_Init    (uint8_t channels);
void     ADC_StartCnv(void);
void     ADC_StopCnv (void);

#endif 
