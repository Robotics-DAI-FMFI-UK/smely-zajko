#ifndef __SERVO_H__ 
#define __SERVO_H__ 

#include <avr/io.h>
#include <compat/deprecated.h>
#include "fcpu.h"

/*
Definitions for the servos
*/

/* from HB-25 manual:

Pulse Input
1.0 ms Full Reverse 
1.5 ms Neutral (Off) 
2.0 ms Full Forward

motors are connected to B,C outputs of timer 1 (OCR1B,OCR1C)

*/


#define PWM_TOP      (unsigned int)((F_CPU/64)/50)	//the pwm top calculation,  timer period = 20 ms

//#define SERVO_STOP	351  //  2820/8
//#define SERVO_FW	400  //  1000/8
//#define SERVO_BW	280  //  4000/8

#define SERVO_STOP	(unsigned int)(3*(F_CPU/64)/2000)  // = 345  1.5ms * timer_ticks_per_millisecond = 1.5 * 14745600/64/1000
#define SERVO_FW    (unsigned int)(2*(F_CPU/64)/1000)  // = 460  2ms * timer_ticks_per_millisecond
#define SERVO_BW    (unsigned int)(1*(F_CPU/64)/1000)  // = 230  1ms * timer_ticks_per_millisecond


#define servo_123_on() cbi(PORTB,4)
#define servo_456_on() cbi(PORTE,2)

#define servo_123_off() sbi(PORTB,4)
#define servo_456_off() sbi(PORTE,2)
/*
turn the power to the servos on and off...
*/

//prototypes
void set_servo(unsigned char which, unsigned int value);
void servos_init();
void slow_set_servo(unsigned char which, unsigned int value);


#endif
