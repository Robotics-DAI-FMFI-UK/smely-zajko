#include "main.h"
#include "i2c.h"
#include "srf08.h"
#include "receiver.h"
#include "servo.h"
#include "motors.h"

// ==============================
// addresses of SRF08 US sensors:
//
//
//      E8   E6    E4
//   EA               E2
//          front
//
// ==============================

#define SRF08_ADDR 0xE2

volatile int8_t THRESHOLD_OBSTACLE_SIDE = 25;
volatile int8_t THRESHOLD_OBSTACLE_LR = 30;
volatile int8_t THRESHOLD_OBSTACLE_M = 80;

static uint16_t dist[5];


void unblock()
{
	sprintf(prnbuf, "!red_switch_released\n\r");
	usart1_putstr();
	wait(10);
	blocked = 0;
}

void initialization(void)
{
    int i;

	io_init();

    wait(500);

    setup_motors();

	i2cInit();

	wait(700);

    for (i = 0; i < 5; i++)
	{
		srf08_set_range(SRF08_ADDR + i * 2, (uint8_t)100);
    	srf08_set_gain(SRF08_ADDR + i * 2, (uint8_t)9);
		wait(3);
	}

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
	static uint8_t last_obstacle_override = 2;

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
	
	if (obstacle_override != last_obstacle_override)
	{
	  	if (obstacle_override) 
		{
			sprintf(prnbuf, "!r_ig_OB\n\r");
			usart1_putstr();
			speed_req = 1;
		}
		else 
		{
			sprintf(prnbuf, "!r_ni_OB\n\r");
			usart1_putstr();
			speed_req = 1;
        }
		last_obstacle_override = obstacle_override;
	}
}


void obstacle_avoidance(void)
{
    static uint8_t which_sensor = 0;	    
	static int8_t seen_obstacle = 0;

	srf08_sample(SRF08_ADDR + 4);  // always sample center sensor
	wait(14);
	dist[2] = srf08_echos[0];
	if (dist[2] < 25) dist[2] = 200;

    // and one of the other 4 (round robin)
	srf08_sample(SRF08_ADDR + which_sensor * 2);
	wait(14);
	dist[which_sensor] = srf08_echos[0];
	if (dist[which_sensor] < 10) dist[which_sensor] = 200;

	which_sensor++;
	if (which_sensor == 2) which_sensor++;
	else if (which_sensor == 5) which_sensor = 0;

	//detect obstacle
	if ((dist[0] < THRESHOLD_OBSTACLE_SIDE) || (dist[1] < THRESHOLD_OBSTACLE_LR) || (dist[2] < THRESHOLD_OBSTACLE_M)
	    || (dist[3] < THRESHOLD_OBSTACLE_LR) || (dist[4] < THRESHOLD_OBSTACLE_SIDE))
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
				   obstacle, dist[0], dist[1], dist[2], dist[3], dist[4]);

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
