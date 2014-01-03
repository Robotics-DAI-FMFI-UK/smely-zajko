#include "adc.h"
volatile unsigned int adc_vals[ADC_CHANNELS];

ISR(ADC_vect)
{
	static unsigned char adc_channel_being_read = 0, battery_alert_blinker = 0;

	adc_vals[adc_channel_being_read] = ADC;	//store the adc value into the array.

	if(adc_channel_being_read == BATTERY_CHANNEL)
	{
		if(adc_vals[7] < MINIMAL_BAT_VOLTAGE)
		{
			battery_alert_blinker++;
			if(battery_alert_blinker == 100) set_leds(0x0F);
			if(battery_alert_blinker == 200)
			{
				set_leds(0x00);
				battery_alert_blinker = 0;
			}
		}
	}

	adc_channel_being_read++;
	if(adc_channel_being_read == ADC_CHANNELS) 
	{
		adc_channel_being_read = 0;;
    }
	if(adc_channel_being_read == BATTERY_CHANNEL)
	{
		adc_select_ref(ADC_REF_256V);
	}
	else
	{
		adc_select_ref(ADC_REF_AVCC);
	}
	ADMUX = (ADMUX & 0xE0) | adc_channel_being_read;
	start_adc();  //starts the new conversion

	return; 
}
