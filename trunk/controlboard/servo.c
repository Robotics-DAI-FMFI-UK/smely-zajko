#include "servo.h"
#include "misc.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned int srvo[7];

void slow_set_servo(unsigned char which, unsigned int value)
{
    unsigned int delta = srvo[which]<value?1:-1;
	while (srvo[which] != value)
	{
		set_servo(which, srvo[which] + delta);
		wait(10);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void set_servo(unsigned char which, unsigned int value)	//this function sets the value of the PWM cycle
{			//for the servos. 
	if(value > PWM_TOP) value = value - PWM_TOP;

	srvo[which] = value;

	switch(which)
	{
		case 0:	//this sets all of the servos to that particular value. 
			OCR1A = value;
			OCR1B = value;
			OCR1C = value;
			OCR3A = value;
			OCR3B = value;
			OCR3C = value;
			break;

		case 1:
			OCR1A = value;
			break;

		case 2:
			OCR1B = value;
			break;

		case 3:
			OCR1C = value;
			break;

		case 4:
			OCR3A = value;
			break;

		case 5:
			OCR3B = value;
			break;

		case 6:
			OCR3C = value;
			break;
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
