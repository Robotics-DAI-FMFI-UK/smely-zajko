#include "fcpu.h"
#include "motors.h"
#include "servo.h"
#include "usart1.h"
#include "receiver.h"

#define F_CPU 14745600UL


volatile int8_t speed;
volatile int8_t dir;

volatile uint8_t reset_steps = 0;

volatile int16_t pwm_speedL = 0;
volatile int16_t pwm_speedR = 0;

int deltaL = 0;
int deltaR = 0;

int tooslowcounter = 0;

volatile int8_t speed_req = 0;

volatile int16_t current_speedL = 0;
volatile int16_t current_speedR = 0;

volatile int16_t target_speedL = 0;
volatile int16_t target_speedR = 0;

int16_t stepL = 0;
int16_t stepR = 0;

int32_t offsetL = 0;
int32_t offsetR = 0;


uint8_t blocked = 0;
uint8_t obstacle = 0;
volatile uint8_t ignore_obstacles = 0;
volatile uint8_t red_switch_pressed = 0;

/* idea of the steering algorithm:

the desired direction and speed are:

 dir     -80..80   where -80/80 turn left/right at the spot, 0=move forward, otherwise move along some circle
 speed   -50..50   (realistic speeds with robot up to +/- 20) 
                   the speed is = to what is measured by the encoder
				   encoder reports speed in units/0.5s, there are 36 units/wheel revolution

when the above are changed, speed_req variable is set to 1, 
and set back to 0 after request has been considered

from the above, we infer the target wished speed for both wheels (units/0.5s):

target_speedL
target_speedR

and we maintain the current speed of both wheels (units/0.5s)

current_speedL  
current_speedR  

the input variables variables to the motor controllers however are (PWM timer value):

pwm_speedL
pwm_speedR

these are from the interval SERVO_BW...SERVO_STOP...SERVO_FW (230...345...460)
we want the speed control to be smooth, i.e. we modify the variables only
slightly in each iteration

*/


#define UPDATE_FREQUENCY 3

static volatile uint8_t next_updateL;
static volatile uint8_t next_updateR;

void update_real_spd()
{
	if (next_updateL) next_updateL--;
	else
	{
	    if (target_speedL > current_speedL)
		{
		   if (pwm_speedL < SERVO_FW) pwm_speedL++;
		   next_updateL = UPDATE_FREQUENCY;
		}
		else if (target_speedL < current_speedL)
		{
			if (pwm_speedL > SERVO_BW) pwm_speedL--;
			next_updateL = UPDATE_FREQUENCY;
		}
	}

	if (next_updateR) next_updateR--;
	else
	{
		if (target_speedR > current_speedR)
		{
			if (pwm_speedR < SERVO_FW) pwm_speedR++;
			next_updateR = UPDATE_FREQUENCY;
		}
		else if (target_speedR < current_speedR)
		{
			if (pwm_speedR > SERVO_BW) pwm_speedR--;
			next_updateR = UPDATE_FREQUENCY;
		}
	}
	       
	set_servo(3, pwm_speedR);
	set_servo(2, pwm_speedL);
}


void compute_update_spd(void)
{
  if ((obstacle && 
      ((!ignore_obstacles) && 
	   (!obstacle_override))) || 
	  blocked) 
	return;
  
  if (dir >= 0)
  {
    target_speedL = 40;
	target_speedR = 40 - dir;
  }
  else
  {
    target_speedL = 40 + dir;
	target_speedR = 40;
  }
  target_speedL *= speed;
  target_speedL += 20;
  target_speedL /= 40;
  target_speedR *= speed;
  target_speedR += 20;
  target_speedR /= 40;

  //sprintf(prnbuf, "! spd %d %d\n\r", speedL, speedR);
  //usart1_putstr(prnbuf);

  speed_req = 0;

  //printf("! SET  dir=%d, spd=%d, targetL=%d, targetR=%d,  realL=%d, realR=%d, currL=%d, currR=%d\n", (int)dir, (int)speed, target_speedL, target_speedR, pwm_speedL, pwm_speedR, current_speedL, current_speedR);
}


volatile int8_t speed;
volatile int8_t dir;

void stop_immediatelly(void)
{
  encoders_clear_position();
  target_speedL = 0;
  target_speedR = 0;
}

void stop(void)
{
  slow_set_servo(0, SERVO_STOP);
  target_speedL = 0;
  target_speedR = 0;
}

void stop_now(void)
{
  set_servo(0, SERVO_STOP);
  target_speedL = 0;
  target_speedR = 0;
  pwm_speedL = SERVO_STOP;
  pwm_speedR = SERVO_STOP;
}

void setup_motors()
{
	next_updateL = 0;
	next_updateR = 0;
	offsetL = (int32_t)0;
	offsetR = (int32_t)0;
	pwm_speedL = SERVO_STOP;
	pwm_speedR = SERVO_STOP;
	servos_init();
    set_servo(2, SERVO_STOP);
	set_servo(3, SERVO_STOP);
    encoders_reverse_motor1();
    wait(1);
	encoders_clear_position();
    wait(1);
}



void halt_request(void)
{ 
	if (blocked) return;

    stepL = -query_position_left();
    wait(1);
	stepR = -query_position_right();
	wait(1);
    
	stop_immediatelly();
	blocked = 1;

    offsetL += stepL;
	offsetR += stepR;

	stepL = 0;
	stepR = 0;
    
    // red_switch_pressed = 0;
	sprintf(prnbuf, "!red switch pressed\n\r");
	usart1_putstr(prnbuf);
	long_wait(50);    
}


void forced_stop(void)
{
	int backup_speedL = target_speedL;
	int backup_speedR = target_speedR;

    sprintf(prnbuf, "!need a break\n\r");
	usart1_putstr();
	stop();
	long_wait(500); //wait until it acutally "stops"

    stepL = -query_position_left();
	wait(1);
 	stepR = -query_position_right();

    //sprintf(prnbuf, "!reset pos to 0\n\r");
	//usart1_putstr();
    stop_immediatelly();
	wait(5);

	offsetL += stepL;
	offsetR += stepR;

	current_speedL = 0;
	current_speedR = 0;
    //forced_print = 1;

    stepL = -query_position_left();
	wait(1);
	stepR = -query_position_right();
    wait(1);

//	target_speedL = backup_speedL;
//	target_speedR = backup_speedR;

//    sprintf(prnbuf, "!resume\n\r");
//    usart1_putstr();
}


void steering(void)
{
	 int cnt = 0;

      //query left encoder for position	  
	  stepL = -query_position_left();
      wait(1);
	
	  //query right encoder for position	  
	  stepR = -query_position_right();
      wait(1);

      if (red_switch_pressed) halt_request();
	  else if (blocked) unblock();       

      /* master requested reset of odometry */
	  if (reset_steps)
	  {
	    offsetL = (int32_t)(-stepL);
		offsetR = (int32_t)(-stepR);
		reset_steps = 0;
		sprintf(prnbuf, "!reset\n\r");
		usart1_putstr();
	  }

      /* forced stop and reset of encoders */
	  
	  if ((abs(stepL) > POSITION_RESET_INTERVAL) || (abs(stepR) > POSITION_RESET_INTERVAL))
	  {
	  	forced_stop();
      }

      //query left encoder for speed
	  current_speedL = -(query_speed_left());
	  wait(1);

      // query right encoder for speed
	  current_speedR = -(query_speed_right());
      wait(1);

  	  if (speed_req) compute_update_spd();		    //REACT
}
