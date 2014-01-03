#include <string.h>
#include "usart1.h"

#include "receiver.h"

/*
USART1 is the Bluetooth usart, through which the robot receives its commands from the Bluetooth master. 
*/


extern volatile char speed_req;
extern volatile char blocked;
extern volatile char speed;
extern volatile char dir;
extern volatile char reset_steps;
extern volatile char ignore_obstacles;
char prnbuf[125];

extern volatile char THRESHOLD_OBSTACLE_LR;
extern volatile char THRESHOLD_OBSTACLE_M;

#define OUTBUFLEN 125
char outbuf[OUTBUFLEN];
volatile char *outlen = outbuf;
volatile char *outptr = outbuf;

volatile int usart1_ready = 1;


//////////////////////////////////////////////////////////////////////////////////////////////////////

FILE usart1 = FDEV_SETUP_STREAM(usart1_putchar, usart1_getchar, _FDEV_SETUP_RW);

//////////////////////////////////////////////////////////////////////////////////////////////////////

void process_char(char c);
void usart1_onechar();

ISR(USART1_RX_vect)
{
	process_char(usart1_getchar(0));	
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
The output vector
*/

ISR(USART1_TX_vect)
{
    if (outptr < outlen) usart1_onechar();
	else usart1_ready = 1;
	return;
}

int usart1_getchar(FILE *stream) 	//wait for the char to be received and then return it
{
	loop_until_bit_is_set(UCSR1A,RXC);
	return UDR1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

int usart1_putchar(char c, FILE *stream)
{
	loop_until_bit_is_set(UCSR1A, UDRE);
	UDR1 = c;
	return 0;
}

void usart1_onechar(void)
{
  loop_until_bit_is_set(UCSR1A, UDRE);
  UDR1 = *(outptr++);
}

void usart1_putstr()
{
  char cnt;
  while (!usart1_ready) cnt++;
  usart1_ready = 0;

  uint8_t ln = strlen(prnbuf);

  if (ln > OUTBUFLEN) ln = OUTBUFLEN;
  outlen = outbuf + ln;
  strncpy(outbuf, prnbuf, ln);
  outptr = outbuf;

  usart1_onechar();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void process_char(char c)
{
  static char expect_command = 1;
  static char expect_value = 0;
  static int value;
  static char sign;
  static char command;
  static char lds;

  if (c == ';') 
  {
    expect_command = 1;
	// execute the previous command
	switch (command)
	{
	  case 's': if (value > 10) value = 10;
	            value *= sign;
				if ((speed != value) && (!remote_override))
				{
				  	speed = value;
				  	speed_req = 1;
                }
				expect_value = 0;
				set_leds(0);
				break;
	  case 'd': if (value > 80) value = 80;
	            value *= sign;
				if ((value != dir) && (!remote_override))
				{ 
				  dir = value;
				  speed_req = 1;
                }
				expect_value = 0;
				set_leds(0);
				break;	
	  case 'c': THRESHOLD_OBSTACLE_LR = value;
	            set_leds(0);
				expect_value = 0;
				break;
      case 'b': THRESHOLD_OBSTACLE_M = value;
	            set_leds(0);
				expect_value = 0;
				break;
	}
  }
  else if (expect_command)
  {
	  switch (c)
	  {
	    case 's': command = 's';
		          expect_command = 0;
				  value = 0;
				  expect_value = 1;
				  sign = 1;
				  set_leds(0x01);
				  lds = 1;
		          break; 
        case 'd': command = 'd';
		          expect_command = 0;
				  value = 0;
				  expect_value = 1;
				  sign = 1;
				  break;
        case 'c': command = 'c';
		          expect_command = 0;
				  value = 0;
				  expect_value = 1;
				  break;
        case 'b': command = 'b';
		          expect_command = 0;
				  value = 0;
				  expect_value = 1;
				  break;
        case 'u': blocked = 0;
		          command = ' ';
		          break;
        case 'r': reset_steps = 1;
		          command = ' ';
				  break;
        case 'i': ignore_obstacles = 1;
		          break;
        case 'o': ignore_obstacles = 0;
		          break;
        default: command = ' ';
		           break;
	  }
  }
  else if (expect_value)
  {
	  if (c == '-') sign = -1;
	  else if ((c >= '0') && (c <= '9'))
	  {
	    value *= 10;
        value += c - '0';
      }
      set_leds(++lds);
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void reset_bluetooth(void)
{
	cbi(PORTD,4);
	wait(1000);
	sbi(PORTD,4);

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


