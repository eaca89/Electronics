/*! \file spi.h \brief SPI interface driver. */
//*****************************************************************************
//
// File Name	: 'spi.h'
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
///	\ingroup driver_avr
/// \defgroup spi SPI (Serial Peripheral Interface) Function Library (spi.c)
/// \code #include "spi.h" \endcode
/// \par Overview
///		Provides basic byte and word transmitting and receiving via the AVR
///	SPI interface.� Due to the nature of SPI, every SPI communication operation
/// is both a transmit and simultaneous receive.
///
///	\note Currently, only MASTER mode is supported.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SPI_H
#define SPI_H

#include "global.h"

// Define the SPI_USEINT key if you want SPI bus operation to be
// interrupt-driven.  The primary reason for not using SPI in
// interrupt-driven mode is if the SPI send/transfer commands
// will be used from within some other interrupt service routine
// or if interrupts might be globally turned off due to of other
// aspects of your program
//
// Comment-out or uncomment this line as necessary

//#define SPI_USEIN

//define master mode
#define MASTER

//define clock frequency's pre-scaler
//#define pre4
//#define  pre16
//#define pre64
#define pre128

//define double transfer speed
//#define DoubleX

//define Data Order
#define MSB
//define SPI Mode
#define Mode0
//#define Mode1
//#define Mode2
//#define Mode3

// function prototypes

// SPI interface initializer
extern void spiInit(void);

// spiSendByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  This command
// does not receive anything.

extern void spiSendByte(u08 data);
//u08 spiReadByte(void) puts 0 in SPDR and wait until the transfer is completed.
//then retuens 1 byte that is available in SPDR.

extern u08 spiReadByte(void);
// spiTransferByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  The function also
// returns the byte that was received during transmission.

extern u08 spiTransferByte(u08 data);

// spiTransferWord(u08 data) works just like spiTransferByte but
// operates on a whole word (16-bits of data).

extern u16 spiTransferWord(u16 data);

#endif
