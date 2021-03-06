#ifndef USART1_H 
#define USART1_H 

//////////////////////////////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <compat/deprecated.h>
#include "misc.h"
#include "servo.h"
#include "timers.h"
#include "usart0.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////

#define COM_BUFFER_LENGTH 16


#define USART_1_BAUD 115200UL
//the baud rate of the bluetooth module

//////////////////////////////////////////////////////////////////////////////////////////////////////

extern char prnbuf[];

//prototypes 
int usart1_putchar(char c, FILE *stream);
void usart1_putstr();
int usart1_getchar(FILE *stream);
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////

//Global variables 
extern FILE usart1;
/*
The file we can make as stdout.
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
