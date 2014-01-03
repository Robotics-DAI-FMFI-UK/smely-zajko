#include "usart0.h"

/*
USART0 is the generic usart. It is the one that's not connected to the Bluetooth module, but is there for 
further expansion of the device.
*/

FILE usart0 = FDEV_SETUP_STREAM(usart0_putchar, usart0_getchar, _FDEV_SETUP_RW);


int usart0_putchar(char c, FILE *stream)
{
	UCSR0B &= ~_BV(RXEN0);	//disables the UART receiver 
	UCSR0B |= _BV(TXEN0);	//enables the UART transmitter
	
	loop_until_bit_is_set(UCSR0A, UDRE);

	UDR0 = c;

	loop_until_bit_is_set(UCSR0A, TXC0);
	//usart0_getchar(0);
	
	UCSR0B &= ~_BV(TXEN0);		//disables the UART transmitter
	UCSR0B |= _BV(RXEN0);	//re-enables the UART receiver 
	
	return 0;
}

int usart0_getchar(FILE *stream) 	//wait for the char to be received and then return it
{
	loop_until_bit_is_set(UCSR0A,RXC);
	return UDR0;
}
