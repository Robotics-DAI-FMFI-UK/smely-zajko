//srf08.h  - functions for controlling SRF08 ultrasonic sensor from AVR Atmega64 (Cerebot) using I2C bus

#ifndef __srf08_h__
#define __srf08_h__

extern uint16_t srf08_echos[17];
extern uint8_t srf08_light;

// This samples sonar at given I2C address. However, it will actively wait (it takes about 60ms) until all echo values
// will become available. A more efficient might be to request sampling, and while program is doing something else,
// we check for the value every once and then; or the values could be filled by a background call from a timer and
// global flag could be set when ranging is finished
void srf08_sample(uint8_t adr);

// change the address of the the I2C device (must be one of 0xE0, 0xE2, 0xE4, 0xE6,
// 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC or 0xFE)
void srf08_change_address(uint8_t adr, uint8_t newadr);

//verify if there is a sonar sensor at the specified address, returns 1 if yes, 0 if not
uint8_t srf08_device_at_adr(uint8_t adr);

//set the gain register (values 0-31, default is maximum gain 31)
void srf08_set_gain(uint8_t adr, uint8_t gain);

//Set the range register (0-255, default is maximum range 255 (11 metres))
//this controls how long time it takes to complete ranging, i.e. decreasing this value is a good idea.
//The range is ((Range Register x 43mm) + 43mm)
//The maximum real range is 6m. If you set the range lower, you probably should decrease the gain.
void srf08_set_range(uint8_t adr, uint8_t range);

#endif
