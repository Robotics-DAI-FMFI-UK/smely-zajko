#include "servo.h"
#include "misc.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned int srvo[7];

void servos_init()
{
	int i = 0;
	while (i <= 6)
		srvo[i++] = SERVO_STOP;
}

void slow_set_servo(unsigned char which, unsigned int value)
{
    if (which == 0)  // case 2 servos only
	{
		int delta2 = (srvo[2]<value)?1:-1;
		int delta3 = (srvo[3]<value)?1:-1;
		int d2 = abs(srvo[2] - value);
		int d3 = abs(srvo[3] - value);
		int d;
		long i;
		if (d2 > d3) d = d3;
		else d = d2;
		int s2 = srvo[2];
		int s3 = srvo[3];
		for (i = 0; i < d; i++)
		{
			long news = s2 + delta2 * i * d2 / d;
			set_servo(2, (int)news);
			news = s3 + delta3 * i * d3 / d;
			set_servo(3, (int)news);
			wait(12);
		}
		set_servo(0, value);
	}
	else 
	{
		int delta = srvo[which]<value?1:-1;
	
		while (srvo[which] != value)
		{
			set_servo(which, srvo[which] + delta);
			wait(12);
		}
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
		//	OCR1A = value;
			OCR1B = value;
			OCR1C = value;
		//	OCR3A = value;
		//	OCR3B = value;
		//	OCR3C = value;
		    srvo[2] = value;
			srvo[3] = value;
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
