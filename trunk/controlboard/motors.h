#ifndef __MOTORS_H__
#define __MOTORS_H__

#include <avr/io.h>

#define POSITION_RESET_INTERVAL 15000


extern volatile int8_t speed;
extern volatile int8_t dir;
extern volatile int8_t speed_req;

extern volatile int16_t current_speedL;
extern volatile int16_t current_speedR;

extern int16_t stepL;
extern int16_t stepR;

extern int32_t offsetL;
extern int32_t offsetR;

extern uint8_t blocked;
extern uint8_t obstacle;
extern volatile uint8_t ignore_obstacles;
extern volatile uint8_t red_switch_pressed;

void update_real_spd();

#endif
