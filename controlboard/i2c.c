/*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "i2c.h"
#include "misc.h" 

#include "uart2.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

// I2C state and address variables
static volatile eI2cStateType I2cState;
static u08 I2cDeviceAddrRW;
// send/transmit buffer (outgoing data)
static u08 I2cSendData[I2C_SEND_DATA_BUFFER_SIZE];
static u08 I2cSendDataIndex;
static u08 I2cSendDataLength;
// receive buffer (incoming data)
static u08 I2cReceiveData[I2C_RECEIVE_DATA_BUFFER_SIZE];
static u08 I2cReceiveDataIndex;
static u08 I2cReceiveDataLength;

// functions
void i2cInit(void)
{
	// set pull-up resistors on I2C bus pins
	sbi(PORTD, 0);	// i2c SCL on ATmega128,64
	sbi(PORTD, 1);	// i2c SDA on ATmega128,64

	// set i2c bit rate to 100KHz
	i2cSetBitrate(100);
	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);
	// set state
	I2cState = I2C_IDLE;
	// do not enable TWI interrupt and enable slave address ACK
	cbi(TWCR, TWIE);

	sbi(TWCR, TWEA);
	// enable interrupts
	sei();
}

void i2cSetBitrate(u16 bitrateKHz)
{
	u08 bitrate_div;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	#ifdef TWPS0
		// for processors with additional bitrate division (mega128)
		// SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
		// set TWPS to zero
		cbi(TWSR, TWPS0);
		cbi(TWSR, TWPS1);
	#endif
	// calculate bitrate division	
	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;
	outb(TWBR, bitrate_div);
}

inline void i2cSendStart(void)
{
	// send start condition
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
}

inline void i2cSendStop(void)
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
}

inline uint8_t i2cWaitForComplete(void)
{
	// wait for i2c interface to complete operation
	int waiting = 32000;
	while(( !(inb(TWCR) & BV(TWINT)) ) && (--waiting));
	if (waiting <= 0)
	{
	   PORTF = 16; // turn off US distance sensors
	   long_wait(200);
	   PORTF = 0; // turn them ON again
	   return 0;
    }
	return 1;
}

inline void i2cSendByte(u08 data)
{
	// save data to the TWDR
	outb(TWDR, data);
	// begin send
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
}

inline void i2cReceiveByte(u08 ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
}

inline u08 i2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return( inb(TWDR) );
}

inline u08 i2cGetStatus(void)
{
	// retieve current i2c status from i2c TWSR
	return( inb(TWSR) );
}

u08 i2cMasterSendNI(u08 deviceAddr, u08 length, u08* data)
{
	int waitin;
	u08 retval = I2C_OK;

	// send start condition
	i2cSendStart();
	if (!i2cWaitForComplete()) 
	{
		return I2C_ERROR_NODEV;
	}

	// send device address with write
	i2cSendByte( deviceAddr & 0xFE );
	if (!i2cWaitForComplete())
	{
		return I2C_ERROR_NODEV;
	}

	// check if device is present and live
	if( inb(TWSR) == TW_MT_SLA_ACK)
	{
		// send data
		while(length)
		{
			i2cSendByte( *data++ );
			if (!i2cWaitForComplete())
			{
				return I2C_ERROR_NODEV;
			}
			length--;
		}
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();

	waitin = 10000;
	while( (!(inb(TWCR) & BV(TWSTO))) && (waitin--));

	if (waitin <= 0) 
	{
		PORTF = 16;   // turn US distance sensors OFF
		long_wait(200);
		PORTF = 0;   // and back ON again
		retval = I2C_ERROR_NODEV;
	}

	//while( !(inb(TWCR) & BV(TWSTO)));

	return retval;
}

u08 i2cMasterReceiveNI(u08 deviceAddr, u08 length, u08 *data)
{
	u08 retval = I2C_OK;

	// send start condition
	i2cSendStart();
	if (!i2cWaitForComplete())
	{
		return I2C_ERROR_NODEV;
	}

	// send device address with read
	i2cSendByte( deviceAddr | 0x01 );
	if (!i2cWaitForComplete())
	{
		return I2C_ERROR_NODEV;
	}

	// check if device is present and live
	if( inb(TWSR) == TW_MR_SLA_ACK)
	{
		// accept receive data and ack it
		while(length > 1)
		{
			i2cReceiveByte(TRUE);
			if (!i2cWaitForComplete())
			{
				return I2C_ERROR_NODEV;
			}
			*data++ = i2cGetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		i2cReceiveByte(FALSE);
		if (!i2cWaitForComplete())
		{
			return I2C_ERROR_NODEV;
		}
		*data++ = i2cGetReceivedByte();
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();

	return retval;
}


eI2cStateType i2cGetState(void)
{
	return I2cState;
}
