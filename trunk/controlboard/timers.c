#include "timers.h"
#include "receiver.h"

#include <stdio.h>

extern char red_switch_pressed;

static int last_change_PF0 = -1000;
static uint8_t last_PF0 = 0;


ISR(TIMER3_OVF_vect)	//this intterrupt occurs about 4k-times/sec
{
	uint8_t pf0 = PINF & 1;
 	  if (pf0 != last_PF0)
	  {
	     last_PF0 = pf0;
		 if (!pf0)
		 {
		    int tm = TCNT1;
			if (tm < last_change_PF0) tm += ICR1 - last_change_PF0;
			else tm -= last_change_PF0;
		    if (tm < 300)
				remote_override = 0;
            else if (tm < 650)
				remote_override = 1;
         }
		 last_change_PF0 = TCNT1;
      }
	  return;
}

int reset_counter = 0;
#define RESET_TIMEOUT 6000

ISR(TIMER0_COMP_vect)	//this intterrupt occurs 100times/sec
{
	TCNT0 = 0;

	static unsigned char led_blinker = 0;

	if(++led_blinker == 150)set_decoleds();;
	if(led_blinker == 152)
	{
		led_blinker = 0;
		clear_decoleds();
	}
	
	if ((PINF & 64) == 0)
	{
		if (!red_switch_pressed)
	   	{
			red_switch_pressed = 1;
	   		set_leds(0x0F);
		}
    }
    else
	{
		if (red_switch_pressed)
		{
	   		red_switch_pressed = 0;
	   		set_leds(0x08);
		}
	}

	return;
}


