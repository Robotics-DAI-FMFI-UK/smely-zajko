#ifndef MAIN_H 
#define MAIN_H 

#include "fcpu.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <compat/deprecated.h>
#include <ctype.h>
#include <avr/wdt.h>


#include "misc.h"
#include "servo.h"
#include "adc.h"
#include "usart0.h"
#include "usart1.h"
#include "timers.h"


static inline void io_init(void);


///////////////////////////////////////////////////////////////////////////////

static inline void io_init(void)
{
	wdt_disable();
	DDRG = _BV(1);	//the decorational LEDs
	PORTG = _BV(0);//the port whose sole purpose is to have a gpio jumper

	DDRC = 64 + 128;  // US1 & US2 sample request pins (0 = no sample, 1 (min.20 uS) = sample)
	PORTC = 32;    // big red switch pull-up ON

	DDRA = _BV(4) | _BV(5) | _BV(6) | _BV(7);
	PORTA = _BV(0) | _BV(1) | _BV(2) | _BV(3);
	//the port with the bumpers and the general leds
	
	DDRB = _BV(1) | _BV(2) | _BV(4) | _BV(5) | _BV(6) | _BV(7);	//set the outputs for the servo pwms. and the power
	PORTB = _BV(3) | _BV(4);	//enable the pullup on miso. adn disable power for  teh servos
	//here are the servos and the SPI bus. as of new, the PINB0 is connected to the LED on the Bluetooth
	//it makes it easier to determine wheter we are connected to a device

	PORTD = _BV(4) | _BV(5) | _BV(6) | _BV(7);//the inputs for the GP buttons are inputs with pullups
	DDRD = _BV(3) | _BV(4);	//the TXD and reset for the bluetooth are outputs.
	
	//DDRE = _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5);//enable the pwm signals for the second set of servos as outputs. and also the servo power enabler
	//DDRE = _BV(2) | _BV(3) | _BV(4) | _BV(5);//enable the pwm signals for the second set of servos as outputs. and also the servo power enabler
//	PORTE = _BV(0) | _BV(2) | _BV(6) | _BV(7);//the interrupts will have pullups. and servos are off by default
	//PORTE = _BV(2) | _BV(6) | _BV(7);//the interrupts will have pullups. and servos are off by default

    DDRE = _BV(2);//enable the pwm signals for the servo power enabler only
    PORTE = 0;

	DDRF = 8;    // relay for switching off US distance sensors
	PORTF = 64;  // red switch on pin6 has pullup on, relay is ON in 0 position

    // INT4 and INT5 (remote control receiver) generate INT4/5 on toggle
	// (INT4 = OC3B = PE4)
	// (INT5 = OC3C = PE5)
    EICRB = 0b00000101;
	EIMSK = 0b00110000;

	// PWM mode 14 (fast PWM, TOP=ICR1)
	TCCR1A = (1 << WGM11);
	TCCR1B = ((1 << WGM13) | (1 << WGM12));

	TCCR3A = (1 << WGM11);
	TCCR3B = ((1 << WGM13) | (1 << WGM12));


	// Start timers 
    TCCR1B |= (1 << CS11);
    TCCR1B |= (1 << CS10);   //prescale 64

	TCCR3B |= (1 << CS30); //prescale 1

	// Clear OC1A/OC1B/OC1C pins on each timer
	// TCCR1A |= ((1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1));
	TCCR1A |= (1 << COM1B1) | (1 << COM1C1);
	// TCCR3A |= ((1 << COM3A1) | (1 << COM3B1) | (1 << COM3C1));

	// Set register values
	ICR1 = PWM_TOP;
	ICR3   = 4000;

	ETIMSK |= (1 << TOIE3);

	OCR1A  = SERVO_STOP;
	OCR1B  = SERVO_STOP;
	OCR1C  = SERVO_STOP;
	OCR3A  = SERVO_STOP;
	OCR3B  = SERVO_STOP;
	OCR3C  = SERVO_STOP;
	
	TCCR0 = _BV(CS00) | _BV(CS01) | _BV(CS02);	//the prescaller of the timer0 will be 1024, this will ensure we
									//have 100 ticks per second
	OCR0 = 0x8F;	//100 ticks per second

	TIMSK = _BV(OCIE0);

	//uncomment the following block if you need ADC
	//the settings for the ADC
//	adc_select_ref(ADC_REF_256V);//set the reference of the ADC to 2.56V
//	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	//interupt enabled for the ADC, prescaler is 128
//	start_adc();
	//we start the ADC
	
	//UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	//UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0A = 0;
	UCSR0B = _BV(RXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UBRR0L = ((F_CPU / (16 * USART_0_BAUD)) - 1) & 0xFF; 

	UCSR1B = _BV(RXCIE1) | _BV(RXEN1) | _BV(TXEN1) | _BV(TXCIE1);
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);
	UBRR1L = ((F_CPU / (16 * USART_1_BAUD)) - 1) & 0xFF; 
	//setting up the USARTs. 8 data, 1 stop, no parity. The baud rate is defined by 
	//the USART_0_BAUD definition somewhere above

	sei();
	//enable interrupts...
	return;
}	

///////////////////////////////////////////////////////////////////////////////


#endif
