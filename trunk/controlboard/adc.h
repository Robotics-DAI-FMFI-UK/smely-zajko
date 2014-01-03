#ifndef ADC_H 
#define ADC_H 

#include <avr/io.h>
#include <compat/deprecated.h>
#include <avr/interrupt.h>

#include "misc.h"

#define ADC_CHANNELS 8

#define BATTERY_CHANNEL 7
/*
The battery channel. This channel is hooked up to the Vcc through a voltage divider. 
The voltage is equal to 0.319 of the Vcc, and therefore the battery voltage.
*/


extern volatile unsigned int adc_vals[ADC_CHANNELS];
/*
The storage for all of the channels of the ADC. 
*/


#define start_adc() sbi(ADCSRA, ADSC)

#define ADC_REF_256V _BV(REFS0) | _BV(REFS1)
#define ADC_REF_AVCC _BV(REFS0)
#define adc_select_ref(x) {ADMUX &= ~(_BV(REFS0) | _BV(REFS0)); ADMUX |= x;}

#define MINIMAL_BAT_VOLTAGE 535	//approx. 4.2V


#endif

