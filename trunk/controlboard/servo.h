#ifndef SERVO_H 
#define SERVO_H 

#include <avr/io.h>
#include <compat/deprecated.h>

/*
Definitions for the servos
*/


#define PWM_TOP      ((F_CPU/64)/50)	//the pwm top calculation

#define ROBOT_FULL_REVERSE  ((F_CPU/64)/1000)
#define ROBOT_FULL_FORWARD  ((F_CPU/64)/500)
#define ROBOT_STOP  ((ROBOT_FULL_REVERSE + ROBOT_FULL_FORWARD) / 2)
#define ROBOT_TENTH ((ROBOT_FULL_FORWARD - ROBOT_STOP) / 10)

#define SERVO_STOP	351  //  2820/8
#define SERVO_FW	400  //  1000/8
#define SERVO_BW	280  //  4000/8

#define servo_123_on() cbi(PORTB,4)
#define servo_456_on() cbi(PORTE,2)

#define servo_123_off() sbi(PORTB,4)
#define servo_456_off() sbi(PORTE,2)
/*
turn the power to the servos on and off...
*/

//prototypes
void set_servo(unsigned char which, unsigned int value);


#endif
