#include "receiver.h"

#include <stdio.h>

extern volatile char speed_req;
extern volatile char speed;
extern volatile char dir;

static int last_t1, last_t2;

volatile uint8_t remote_override = 0;
volatile uint8_t obstacle_override = 0;
volatile uint8_t extra_flag = 0;
volatile int16_t MAXSPEED = 10;

ISR(INT4_vect)	//receiver fwd/bwd
{
	int tm;
	int t1 = TCNT1;
	if (t1 < last_t1) tm = t1 + (ICR1 - last_t1);
	else tm = t1 - last_t1;
	last_t1 = t1;

    //fwd-bwd: 444=bwd, 264=fwd, 353 = center

	if (remote_override)
    	if ((PINE & 16) == 0)
		{
    		tm -= 353;
			if (tm > 150) tm = 0;
			if (tm < -150) tm = 0;
			tm *= -1;
			int new_speed = tm * MAXSPEED / 120; //tm / 6;
			if (new_speed != speed)
			{
    			speed = new_speed;
				speed_req = 1;
			}
		}

	return;
}

ISR(INT5_vect)  //receiver left/right
{
    int tm;
	int t2 = TCNT1;
	if (t2 < last_t2) tm = t2 + (ICR1 - last_t2);
	else tm = t2 - last_t2;
	last_t2 = t2;

	//left-right: 246=fwd, 466=bwd, 356=center

    if (remote_override)
    	if ((PINE & 32) == 0)
		{
			tm -= 350;
			//tm -= 353;
			if (tm > 140) tm = 0;
			if (tm < -140) tm = 0;
			tm *= -1;
			//int new_dir = tm * 40 / 110;
			int new_dir = (tm * abs(tm) + 50*tm)/180;

			if (new_dir != dir)
			{
				dir = new_dir;
				speed_req = 1;
			}
		}

	return;
}

