#include "misc.h"
#include "motors.h"
#include "encoders.h"

volatile unsigned char bot_number=DEFAULT_SBOT_NUMBER;//a global in which the number of the bot is stored; 

///////////////////////////////////////////////////////////////////////////////

//wait specified # of milliseconds
void wait(unsigned int a)
{							
	while(a--)_delay_ms(1);
	return;
}

//wait milliseconds, but keep updating current_speed,
//must be used for waits longer than 10ms (otherwise
//motors get crazy in timer interrupt routine)
void long_wait(unsigned int a)
{							
	while(a > 10)
	{
	  a -= 10;
	  _delay_ms(8);
      //query left encoder for speed
	  current_speedL = -(query_speed_left());
	  wait(1);

      // query right encoder for speed
	  current_speedR = -(query_speed_right());
      wait(1);
	}
	wait(a);

	return;
}



///////////////////////////////////////////////////////////////////////////////


//you input a character, say, 'A' and you get 10. 
unsigned char hex_to_dec(unsigned char in) 
{ 
   if(((in >= '0') && (in <= '9'))) return in-'0'; 
   in |= 0x20; 
   if(((in >= 'a') && (in <= 'f'))) return in-'a' + 10; 
   return HEX_DEC_ERROR; 
}

///////////////////////////////////////////////////////////////////////////////

//gets the sbot nubmer from the EEPROM memory, if there's none, it sets the defauly sbot nubmer
//finished, working
unsigned char get_sbot_number(void)
{
	unsigned char a = eeprom_read_byte(EEPROM_LOCATION_SBOT_NUBMER);
	if(a != 0xFF)
	{
		bot_number = a;
		return bot_number;
	}
	
	eeprom_write_byte(EEPROM_LOCATION_SBOT_NUBMER, DEFAULT_SBOT_NUMBER);
	bot_number = DEFAULT_SBOT_NUMBER;

	return bot_number;
}


///////////////////////////////////////////////////////////////////////////////

//renames the SBot to something else. 
//finished, working
void set_sbot_number(unsigned char what_to)
{
	eeprom_write_byte(EEPROM_LOCATION_SBOT_NUBMER,what_to);
	bot_number = what_to;
	return;
}

///////////////////////////////////////////////////////////////////////////////
