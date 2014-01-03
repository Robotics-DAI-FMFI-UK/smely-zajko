#ifndef USART0_H 
#define USART0_H 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>

#include "usart1.h"

#define USART_0_BAUD 19200UL

int usart0_putchar(char c, FILE *stream);
int usart0_getchar(FILE *stream); 	//wait for the char to be received and then return it

extern FILE usart0;
#endif
