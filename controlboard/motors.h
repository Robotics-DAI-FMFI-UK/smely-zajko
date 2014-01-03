#ifndef __MOTORS_H__
#define __MOTORS_H__

#include <avr/io.h>

#define POSITION_RESET_INTERVAL 2500


extern volatile int8_t speed;
extern volatile int8_t dir;
extern volatile int8_t speed_req;

extern uint16_t act_speedL;
extern uint16_t act_speedR;

extern uint16_t stepL;
extern uint16_t stepR;

extern int32_t offsetL;
extern int32_t offsetR;

extern uint8_t blocked;
extern uint8_t obstacle;
extern volatile uint8_t ignore_obstacles;
extern volatile uint8_t red_switch_pressed;

#endif
