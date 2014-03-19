#include "main.h"
#include "i2c.h"
#include "srf08.h"
#include "receiver.h"
#include "servo.h"
#include "motors.h"

#define SRF08_ADDR 0xE8

volatile int8_t THRESHOLD_OBSTACLE_LR = 45;
volatile int8_t THRESHOLD_OBSTACLE_M = 80;

static uint8_t distL = 255;
static uint8_t distM = 255;
static uint8_t distR = 255;
static uint8_t distSL = 255;
static uint8_t distSR = 255;


void unblock()
{
	sprintf(prnbuf, "!red_switch_released\n\r");
	usart1_putstr();
	wait(10);
	blocked = 0;
}

void initialization(void)
{
	io_init();

    wait(500);

    setup_motors();

	i2cInit();

	wait(100);

	srf08_set_range(SRF08_ADDR, (uint8_t)100);
    srf08_set_gain(SRF08_ADDR, (uint8_t)9);

	set_leds(0x01); wait(50);
	set_leds(0x03); wait(60);
	set_leds(0x02); wait(70);
	set_leds(0x06); wait(80);
	set_leds(0x04); wait(90);
	set_leds(0x0C); wait(100);
	set_leds(0x08); wait(110);
	set_leds(0x00);

	get_sbot_number();
	stdout = &usart1;
	stdin = &usart1;

	red_switch_pressed = 0;

	sprintf(prnbuf, "! smely zajko\n\r");
	usart1_putstr();

	wait(800);
}

void remote_controll(void)
{
	static uint8_t last_remote_override = 2;

	if (remote_override != last_remote_override)
	{
	  	if (remote_override) 
		{
			sprintf(prnbuf, "!rON\n\r");
			usart1_putstr();
			speed_req = 1;
		}
		else 
		{
			sprintf(prnbuf, "!rOFF\n\r");
			usart1_putstr();
			speed = 0;
			dir = 0;
			stop();
        }
		last_remote_override = remote_override;
	}			
}


void obstacle_avoidance(void)
{
	int8_t seen_obstacle = 0;

	srf08_sample(SRF08_ADDR);
	wait(14);

	//load sampled distance values
	distL = 200; 
	distR = 200; 
	distSL = 200;
	distSR = 200;
	distM = srf08_echos[0];

	if (distM < 25) distM = 200;

	//detect obstacle
	//if ((distL < THRESHOLD_OBSTACLE_LR) || (distR < THRESHOLD_OBSTACLE_LR) || (distM < THRESHOLD_OBSTACLE_M))
	if ((distR < THRESHOLD_OBSTACLE_LR) || (distM < THRESHOLD_OBSTACLE_M))
		seen_obstacle += 5;
	else 
	if (seen_obstacle) seen_obstacle --;

	if (seen_obstacle > 15) 
	{
		seen_obstacle = 15;
		if ((!ignore_obstacles) && (!obstacle))
		{
			stop();
			obstacle = 1;
			sprintf(prnbuf, "!obs\n\r");
			usart1_putstr();
		}
	}
	else if ((!seen_obstacle) && (obstacle)) 
	{
		obstacle = 0;
		compute_update_spd();
	}

	wait(1);
}

void status_reporting()
{
	  //sprintf(prnbuf, "!Of %ld %d; %ld %d\n\r", offsetL, stepL, offsetR, stepR);
	  //usart1_putstr();
	  
	  // produce regular polling output
	  int32_t sumL = offsetL;
	  sumL += stepL;
	  int32_t sumR = offsetR;
	  sumR += stepR;

      sprintf(prnbuf, "@%ld %ld %d %d %d %d %u %u %u %u %u\n\r", 
	               sumL, sumR, current_speedL, current_speedR, blocked,
				   obstacle, distL, distM, distR, distSL, distSR);

      // send it (in the background)
	  usart1_putstr();       
}

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
	int i;
	initialization();

/*	for (i = SERVO_STOP; i < SERVO_FW; i++)
	{
		wait(4);
		set_servo(2, i);
		set_servo(3, i);
		steering();
		status_reporting();
    }

	for (i = 0; i < 10; i++)
	{
		wait(1000);
  	    steering();
	    status_reporting();
    }

    for (i = SERVO_FW; i > SERVO_STOP; i--)
	{
		wait(4);
		set_servo(2, i);
	    set_servo(3, i);
  	    steering();
	    status_reporting();
    }
*/
	while(1)
	{
	  remote_controll();
	  obstacle_avoidance();
	  steering();
	  status_reporting();
	}
}

///////////////////////////////////////////////////////////////////////////////

