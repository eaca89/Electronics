/*! \file spi.c \brief SPI interface driver. */
//*****************************************************************************
//
// File Name	: 'spi.c'
// Title		: SPI interface driver
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 06/06/2002
// Version		: 0.6
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"

// global variables
volatile u08 spiTransferComplete;

// SPI interrupt service handler
#ifdef SPI_USEINT
ISR(SPI_STC_vect)
{
	spiTransferComplete = TRUE;
}
#endif



//access routines
void spiInit()
{
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega64__)
	// setup SPI I/O pins
	#define 	SPI_DDR   	DDRB
	#define		SPI_MISO	PB3
	#define 	SPI_MOSI   	PB2
	#define 	SPI_SCK   	PB1
	#define 	SPI_SS   	PB0 

	#define SPI_Ctrl	SPCR
	#define SPI_Status	SPSR
	#define SPI_Data	SPDR

#elif defined(__AVR_ATmega8__)
    // setup SPI I/O pins
	#define 	SPI_DDR   	DDRB
	#define		SPI_MISO	PB4
	#define 	SPI_MOSI   	PB3
	#define 	SPI_SCK   	PB5
	#define 	SPI_SS   	PB2 

	#define SPI_Ctrl	SPCR
	#define SPI_Status	SPSR
	#define SPI_Data	SPDR

#elif defined(__AVR_ATtiny26)
    // setup SPI I/O pins of ATtiny26
	#define 	SPI_DDR   	DDRB
	#define		SPI_MISO	PB1
	#define 	SPI_MOSI   	PB0
	#define 	SPI_SCK   	PB2

    //SS is not availabale in tiny26

#else
	// setup SPI I/O pins
	#define 	SPI_DDR   	DDRB
	#define		SPI_MISO	PB6
	#define 	SPI_MOSI   	PB5
	#define 	SPI_SCK   	PB7
	#define 	SPI_SS   	PB4 

	#define SPI_Ctrl	SPCR
	#define SPI_Status	SPSR
	#define SPI_Data	SPDR

#endif
	
// setup SPI interface :
#ifdef MASTER

	#ifdef ATtiny26
		SPI_DDR |= ( _BV(SPI_MOSI) | _BV(SPI_SCK) );
	#else
		SPI_DDR |= ( _BV(SPI_MOSI) | _BV(SPI_SCK) | _BV(SPI_SS) );
	#endif
	
	sbi(SPI_Ctrl, MSTR);

	#if defined(pre4)
		cbi(SPI_Ctrl, SPR0);
		cbi(SPI_Ctrl, SPR1);

	#elif defined(pre16)
		sbi(SPI_Ctrl, SPR0);
		cbi(SPI_Ctrl, SPR1);

	#elif defined (pre64)
		cbi(SPI_Ctrl, SPR0);
		sbi(SPI_Ctrl, SPR1);

	#else
		sbi(SPI_Ctrl, SPR0);
		sbi(SPI_Ctrl, SPR1);
	#endif

	#ifdef DoubleX
		sbi(SPI_Status, SPI2X);
	#endif

#else
	sbi(SPI_DDR, SPI_MISO);
	cbi(SPI_Ctrl, MSTR);
#endif


#ifdef MSB
	cbi(SPI_Ctrl, DORD);
#else
	sbi(SPI_Ctrl, DORD);
#endif

#ifdef Mode0
	cbi(SPI_Ctrl, CPOL);
	cbi(SPI_Ctrl, CPHA);
#elif defined(Mode1)
	cbi(SPI_Ctrl, CPOL);
	sbi(SPI_Ctrl, CPHA);
#elif defined(Mode2)
	sbi(SPI_Ctrl, CPOL);
	cbi(SPI_Ctrl, CPHA);
#else 	//Mode3
	sbi(SPI_Ctrl, CPOL);
	sbi(SPI_Ctrl, CPHA);
#endif

// enable SPI
	sbi(SPI_Ctrl, SPE);

	// clear status
	//to clear SPIF, reading SPDR & SPSR is necessary.
	volatile char S = inb(SPSR);
	volatile char D = inb(SPDR);
	spiTransferComplete = TRUE;

// enable SPI interrupt
#ifdef SPI_USEINT
sbi(SPI_Ctrl, SPIE);
#endif
}

void spiSendByte(u08 data)
{
	// send a byte over SPI and ignore reply
	spiTransferComplete = FALSE;
	outb(SPI_Data, data);
	#ifdef SPI_USEINT
		while( !spiTransferComplete );
	#else
		while( ! (inb(SPI_Status) & _BV(SPIF) ) );
	#endif
	//to clear SPIF, reading SPDR & SPSR is necessary.
	volatile char D = SPI_Data;
}
u08 spiReadByte(void)
{
	spiTransferComplete = FALSE;
	outb(SPI_Data, 0x00);
	#ifdef SPI_USEINT
		while( !spiTransferComplete );
	#else
		while( ! (inb(SPI_Status) & _BV(SPIF) ) );
	#endif
	return SPI_Data;
}
u08 spiTransferByte(u08 data)
{
	u08 D,S;
// send the given data
	spiTransferComplete = FALSE;
	outb(SPI_Data, data);

// wait for transfer to complete
	#ifdef SPI_USEINT
		while( !spiTransferComplete );
	#else
		while( ! ( SPI_Status & _BV(SPIF) ) );
		// *** reading of the SPSR and SPDR are crucial
		// *** to the clearing of the SPIF flag
		// *** in non-interrupt mode
		S = SPI_Status;
		D = SPI_Data;
		// set flag
		spiTransferComplete = TRUE;
	#endif
	// return the received data
	return D;
}

u16 spiTransferWord(u16 data)
{
	u16 rxData = 0;

	// send MS byte of given data
	rxData = ( spiTransferByte( (data >> 8) & 0x00FF) ) << 8;
	// send LS byte of given data
	rxData |= ( spiTransferByte( data & 0x00FF ) );

	// return the received data
	return rxData;
}
