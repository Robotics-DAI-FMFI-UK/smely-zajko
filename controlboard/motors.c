#include "motors.h"
#include "servo.h"
#include "usart1.h"

volatile int8_t speed;
volatile int8_t dir;

volatile uint8_t reset_steps = 0;

int real_speedL = 0;
int real_speedR = 0;

int deltaL = 0;
int deltaR = 0;

int tooslowcounter = 0;

volatile int8_t speed_req = 0;

static int16_t speedL = 0;
static int16_t speedR = 0;

static int8_t dirL = 0;
static int8_t dirR = 0;

uint16_t act_speedL = 0;
uint16_t act_speedR = 0;

uint16_t stepL = 0;
uint16_t stepR = 0;

int32_t offsetL = 0;
int32_t offsetR = 0;


uint8_t blocked = 0;
uint8_t obstacle = 0;
volatile uint8_t ignore_obstacles = 0;
volatile uint8_t red_switch_pressed = 0;


void update_real_spd()
{
    int actL = act_speedL;
	int actR = act_speedR;

    int sL = real_speedL;
	int sR = real_speedR;

	if (actL > 127) actL = 256 - act_speedL;
	else actL *= -1;
	if (actR > 127) actR = 256 - act_speedR;
	else actR *= -1;

	actR *= 4;
	actL *= 4;

    if (abs(real_speedL) > abs(actL))
	{ 
	  int toAdd = 20;
	  if (tooslowcounter > 200) toAdd += 12;
	  if (real_speedL > 0) sL += toAdd;
	  else sL -= toAdd;
	  tooslowcounter ++;
    }
	else tooslowcounter = 0;

    if (abs(real_speedR) > abs(actR)) 
	{
	  int toAdd = 20;
	  if (tooslowcounter > 200) toAdd += 12;
	  if (real_speedR > 0) sR += toAdd;
	  else sR -= toAdd;
    }
	       
	set_servo(2, SERVO_STOP + sL);
	set_servo(3, SERVO_STOP + sR);
}

void update_spd(void)
{
    int8_t newdirL, newdirR;

	speed_req = 0;

    if ((obstacle && (!ignore_obstacles)) || blocked) return;

    real_speedL = speedL;
	real_speedR = speedR;

    if (speedL > 0) newdirL = 1;
	else if (speedL < 0) newdirL = -1;
	else newdirL = 0;

    if (speedR > 0) newdirR = 1;
	else if (speedR < 0) newdirR = -1;
	else newdirR = 0;

    dirL = newdirL;
    dirR = newdirR;
    update_real_spd();
}


void compute_update_spd(void)
{
  if (dir >= 0)
  {
    speedL = 40;
	speedR = 40 - dir;
  }
  else
  {
    speedL = 40 + dir;
	speedR = 40;
  }
  speedL *= speed;
  speedL /= 15;
  speedR *= speed;
  speedR /= 15;

  //sprintf(prnbuf, "! spd %d %d\n\r", speedL, speedR);
  usart1_putstr(prnbuf);

  update_spd();
}


volatile int8_t speed;
volatile int8_t dir;

void stop_immediatelly(void)
{
  usart0_putchar(0x28, 0);
  speedL = 0;
  speedR = 0;
  dirL = 0;
  dirR = 0;
}

void stop(void)
{
  slow_set_servo(2, SERVO_STOP);
  slow_set_servo(3, SERVO_STOP);
  speedL = 0;
  speedR = 0;
  dirL = 0;
  dirR = 0;
}

void setup_motors()
{
	offsetL = (int32_t)0;
	offsetR = (int32_t)0;
    set_servo(2, SERVO_STOP);
	set_servo(3, SERVO_STOP);
	usart0_putchar(0x32, 0);  // reverse direction of motor 1
    wait(1);
	usart0_putchar(0x28, 0);  // all motors reset position and halt
    wait(1);
}

uint16_t query_position_left(void)
{
  uint16_t p;

  usart0_putchar(9, 0);
  usart0_getchar(0);
  p = usart0_getchar(0) << 8;
  p += usart0_getchar(0);

  return p;
}

uint16_t query_position_right(void)
{
  uint16_t p;

  usart0_putchar(10, 0);
  usart0_getchar(0);
  p = usart0_getchar(0) << 8;
  p += usart0_getchar(0);

  return p;
}

uint16_t query_speed_left(void)
{
  uint16_t p;

  usart0_putchar(17, 0);
  usart0_getchar(0);
  p = usart0_getchar(0) << 8;
  p += usart0_getchar(0);

  return p;
}

uint16_t query_speed_right(void)
{
  uint16_t p;

  usart0_putchar(18, 0);
  usart0_getchar(0);
  p = usart0_getchar(0) << 8;
  p += usart0_getchar(0);

  return p;
}

void halt_request(void)
{ 
	if (blocked) return;

    stepL = query_position_left();
    wait(1);
	stepR = query_position_right();
	wait(1);
    
	stop_immediatelly();
	blocked = 1;

    if (stepL < 0) offsetL -= (-stepL);
	else offsetL += stepL;
	if (stepR < 0) offsetR -= (-stepR);
	else offsetR += stepR;

	stepL = 0;
	stepR = 0;
    
    // red_switch_pressed = 0;
	sprintf(prnbuf, "!red switch pressed\n\r");
	usart1_putstr(prnbuf);
	wait(50);    
}


void forced_stop(void)
{
	int backup_speedL = speedL;
	int backup_speedR = speedR;

    stop();
	sprintf(prnbuf, "!need a break\n\r");
	usart1_putstr();
	wait(3000); //wait until it acutally stops

    stepL = query_position_left();
	wait(1);
 	stepR = query_position_right();

    sprintf(prnbuf, "!reset pos to 0\n\r");
	usart1_putstr();
    stop_immediatelly();
	wait(100);

	if (stepL < 0) offsetL -= (-stepL);
	else offsetL += stepL;
	if (stepR < 0) offsetR -= (-stepR);
	else offsetR += stepR;

	speedL = backup_speedL;
	speedR = backup_speedR;

    stepL = query_position_left();
	wait(1);
	stepR = query_position_right();
    wait(1);

    sprintf(prnbuf, "!resume\n\r");
    usart1_putstr();
	update_spd();
}


void steering(void)
{
      //query left encoder for position	  
	  stepL = query_position_left();
      wait(1);
	
	  //query right encoder for position
	  
	  stepR = query_position_right();
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
	  act_speedL = query_speed_left();
      wait(1);

      // query right encoder for speed
	  act_speedR = query_speed_right();
      wait(1);

  	  if (speed_req) compute_update_spd();		    //REACT
	  else update_real_spd();
}



