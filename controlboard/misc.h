#ifndef MISC_H 
#define MISC_H 

#include <avr/eeprom.h> 
#include <util/delay.h>


#define DEFAULT_SBOT_NUMBER '1'
#define EEPROM_LOCATION_SBOT_NUBMER 0
#define EEPROM_LOCATION_CHANGED_NAME 1
#define ALL_ROBOTS_NAME '*'
#define ROBOT_FW_VERSION 1
extern volatile unsigned char bot_number;//a global in which the nubmer of the bot is stored; 
///////////////////////////////////////////////////////////////////////////////

//set the leds to the value in binary
#define set_leds(stat) PORTA = (PORTA & 0x0F) | ((stat * 0x10) & 0xF0)

//returns the state of all leds
#define get_leds() ((PORTA & 0xF0)/0x10)

//the frontal leds functions. set = turn on, clear = off, toggle = toggle
#define set_decoleds() cbi(PORTG,1)
#define clear_decoleds() sbi(PORTG,1)
#define toggle_decoleds() PORTG ^= _BV(1)


#define nop() asm volatile ("nop")


/*
returns 0 if all of the selected bumpers are switched (colision), or 
returns the value of all of the bumpers if one is not in colision.
This can be used to detect collisions like if(bumper_stat(BUMPER_ALL) != 0x0F))colision();
*/
#define bumper_stat(which_bumper)  (PINA & which_bumper)
#define BUMPER_0 0x01
#define BUMPER_1 0x02
#define BUMPER_2 0x04
#define BUMPER_3 0x08
#define BUMPER_ALL 0x0F


/*
gets the status of the general purpose jumper. if the jumper is not shorted, it returns 0
*/
#define gp_jumper_short() bit_is_clear(PING,0)



/*
returns wheter the requested button is pressed. 
*/
#define GP_BUTTON_0 5
#define GP_BUTTON_1 6
#define GP_BUTTON_2 7
#define button_pressed(x) bit_is_clear(PIND,x)


#define debounce(x) {wait(20);while(x);wait(20);}

//the error output for the hex to dec function
#define HEX_DEC_ERROR 0xFF 


//prototypes:
void wait(unsigned int a);
unsigned char hex_to_dec(unsigned char in);
unsigned char get_sbot_number(void);
void set_sbot_number(unsigned char what_to);

#endif

