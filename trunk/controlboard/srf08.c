//srf08.c  - functions for controlling SRF08 ultrasonic sensor from AVR Atmega64 (Cerebot) using I2C bus

#include <inttypes.h>
#include <util/delay.h>

// this is included from AVRlib (link with i2c.c)
#include "i2c.h"

//we store the echos here
uint16_t srf08_echos[17];

// This samples sonar at given I2C address. However, it will actively wait (it takes about 60ms) until the value
// will become available. A more efficient might be to request sampling, and while program is doing something else,
// we check for the value every once and then
void srf08_sample(uint8_t adr)
{
  uint8_t buf[2];
  uint8_t result1, result2;
  uint8_t i;
  uint8_t waiting = 0;
  srf08_echos[0] = 255;

  buf[0] = 0;     // command register
  buf[1] = 0x51;  // Real Ranging Mode - Result in centimeters
  i2cMasterSendNI(adr, 2, buf);
  
  _delay_ms(1);

  // we read I2C bus until result will arrive
  do {
    buf[0] = 0;     // command register
	i2cMasterSendNI(adr, 1, buf);
    i2cMasterReceiveNI(adr, 1, buf);
	_delay_ms(1);
	waiting++;
  } while ((waiting<18) && (buf[0] == (uint8_t)0x00)); //while the value is not ready, we will read 0x00 from the sensor 

  if (waiting >= 18) return;  

  //for (i = 0; i < 17; i++)
  for (i = 0; i < 1; i++)
  {
    buf[0] = (i << 1) + 3;   // ranging result - low byte
    i2cMasterSendNI(adr, 1, buf);
    i2cMasterReceiveNI(adr, 1, &result1);

    buf[0] = (i << 1) + 2;   // ranging result - hi byte
	
	i2cMasterSendNI(adr, 1, buf);
    i2cMasterReceiveNI(adr, 1, &result2);

    srf08_echos[i] = (uint16_t)result1 + ((uint16_t)result2 << 8);
    if (!srf08_echos[i]) break; //the last echo is terminated by 0
  }
}

//set the gain register (values 0-31, default is maximum gain 31)
void srf08_set_gain(uint8_t adr, uint8_t gain)
{
  uint8_t buf[2];

  buf[0] = 1;  // gain register
  buf[1] = gain;
  i2cMasterSendNI(adr, 2, buf);
}

//Set the range register (0-255, default is maximum range 255 (11 metres))
//this controls how long time it takes to complete ranging, i.e. decreasing this value is a good idea.
//The range is ((Range Register x 43mm) + 43mm)
//The maximum real range is 6m (value 140). Range of 1m is value 24.
//If you set the range lower, you probably should decrease the gain.
void srf08_set_range(uint8_t adr, uint8_t range)
{
  uint8_t buf[2];

  buf[0] = 2;  // range register
  buf[1] = range;
  i2cMasterSendNI(adr, 2, buf);
}

// change the address of the the I2C device (must be one of 0xE0, 0xE2, 0xE4, 0xE6,
// 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC or 0xFE)
void srf08_change_address(uint8_t adr, uint8_t newadr)
{
  uint8_t buf[2];

  // send string over I2C
  buf[0] = 0;     // command register
  buf[1] = 0xA0;  // 1st in sequence to change I2C address
  i2cMasterSendNI(adr, 2, buf);
  buf[1] = 0xAA;  // 2nd in sequence to change I2C address
  i2cMasterSendNI(adr, 2, buf);
  buf[1] = 0xA5;  // 3rd in sequence to change I2C address
  i2cMasterSendNI(adr, 2, buf);
  buf[1] = newadr;
  i2cMasterSendNI(adr, 2, buf);
}

//verify if there is a sonar sensor at the specified address, returns 1 if yes, 0 if not
uint8_t srf08_device_at_adr(uint8_t adr)
{
  uint8_t buf[1];
  buf[0] = 0;     // software revision
  i2cMasterSendNI(adr, 1, buf);
  //read software revision byte
  i2cMasterReceiveNI(adr, 1, buf);
  if (buf[0] == 0xFF) return 0;  //0xFF indicates no byte read
  return buf[0];
}
