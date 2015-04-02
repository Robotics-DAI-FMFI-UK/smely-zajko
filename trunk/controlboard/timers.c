#include "timers.h"
#include "receiver.h"
#include "motors.h"

#include <stdio.h>

static int last_change_PF0 = -1000;
static int last_change_PF1 = -1000;
static int last_change_PF2 = -1000;
static int last_change_PF3 = -1000;
static uint8_t last_PF = 0;
static uint8_t last_PF0 = 0;
static uint8_t last_PF1 = 0;
static uint8_t last_PF2 = 0;
static uint8_t last_PF3 = 0;
static uint8_t pf, pf0, pf1, pf2, pf3;
static int tm;
static int16_t newMAXSPEED = 10;

ISR(TIMER3_OVF_vect)	//this intterrupt occurs about 4k-times/sec
{
	pf = PINF;

	if (pf != last_PF);
	{
	  last_PF = pf;

	  pf0 = PINF & 1;
	  pf1 = PINF & 2;
	  pf2 = PINF & 4;
	  pf3 = PINF & 8;

 	  if (pf0 != last_PF0)
	  {
	     last_PF0 = pf0;
		 if (!pf0)
		 {
		    tm = TCNT1;
			if (tm < last_change_PF0) tm += ICR1 - last_change_PF0;
			else tm -= last_change_PF0;
		    if (tm < 300)
				remote_override = 0;
            else if (tm < 650)
				remote_override = 1;
         }
		 last_change_PF0 = TCNT1;
      }
	  
	  if (pf1 != last_PF1)
	  {
	     last_PF1 = pf1;
		 if (!pf1)
		 {
		    tm = TCNT1;
			if (tm < last_change_PF1) tm += ICR1 - last_change_PF1;
			else tm -= last_change_PF1;
		    if (tm < 270)
				extra_flag = 0;
            else if ((tm > 410) && (tm < 650))
				extra_flag = 1;
         }
		 last_change_PF1 = TCNT1;
      }

	  if (remote_override)
  	    if (pf2 != last_PF2)
	    {
	     last_PF2 = pf2;
		 if (!pf2)
		 {
		    tm = TCNT1;
			if (tm < last_change_PF2) tm += ICR1 - last_change_PF2;
			else tm -= last_change_PF2;
		    if (tm < 270)
				obstacle_override = 0;
            else if ((tm > 410) && (tm < 650))
				obstacle_override = 1;
         }
		 last_change_PF2 = TCNT1;
        }

	  if (remote_override)
	    if (pf3 != last_PF3)
	    {
	     last_PF3 = pf3;
		 if (!pf3)
		 {
		    tm = TCNT1;
			if (tm < last_change_PF3) tm += ICR1 - last_change_PF3;
			else tm -= last_change_PF3;
		    if (tm < 650)
			{
				newMAXSPEED = (tm - 100) / 8;
				if (abs(newMAXSPEED - MAXSPEED) > 4)
			    	MAXSPEED = newMAXSPEED;
			}	
          }
		  last_change_PF3 = TCNT1;
        }

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
	
	if ((PINC & RED_SWITCH_PIN) == 0)
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

    update_real_spd();
	return;
}


