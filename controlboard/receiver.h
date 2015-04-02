#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>
#include <ctype.h>

extern volatile uint8_t remote_override;
extern volatile uint8_t obstacle_override;
extern volatile uint8_t extra_flag;
extern volatile int16_t MAXSPEED;

#endif
