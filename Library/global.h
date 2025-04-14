//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: AVRlib project global include 
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifndef F_CPU
#define F_CPU	1000000UL
#endif

#ifndef WIN32
// true/false defines
#define FALSE	0
#define TRUE	1
#endif

// datatype definitions macros
typedef unsigned char  u08;
typedef   signed char  s08;

typedef unsigned int  u16;
typedef   signed int  s16;

typedef unsigned long  u32;
typedef   signed long  s32;

typedef unsigned long long u64;
typedef   signed long long s64;

// maximum value that can be held
// by unsigned data types (8,16,32bits)
#define MAX_U08	255
#define MAX_U16	65535
#define MAX_U32	4294967295

// maximum values that can be held
// by signed data types (8,16,32bits)
#define MIN_S08	-128
#define MAX_S08	127
#define MIN_S16	-32768
#define MAX_S16	32767
#define MIN_S32	-2147483648
#define MAX_S32	2147483647

#ifndef WIN32
	// more type redefinitions
	typedef unsigned char   BOOL;
	typedef unsigned char	BYTE;
	typedef unsigned int	WORD;
	typedef unsigned long	DWORD;

	typedef unsigned char	UCHAR;
	typedef unsigned int	UINT;
	typedef unsigned short  USHORT;
	typedef unsigned long	ULONG;

	typedef char			CHAR;
	typedef int				INT;
	typedef long			LONG;
#endif

// outb(), inb(), BV(), sbi(), cbi()
#ifndef outb
	#define	outb(addr, data)	addr = (data)
#endif

#ifndef inb
	#define	inb(addr)			(addr)
#endif

#ifndef _BV
	#define _BV(bit)			(1<<(bit))
#endif

#ifndef cbi//cbi(PORTB,5)
	#define cbi(reg,bit)	reg &= ~(_BV(bit))
#endif

#ifndef sbi//sbi(DDRC,3)
	#define sbi(reg,bit)	reg |= (_BV(bit))
#endif

#ifndef CHECKBIT//var = CHECKBIT(PINA, 6)
	#define CHECKBIT(reg,bit)	( ( reg & _BV(bit) ) ? 1 : 0)
#endif

#ifndef toggle//toggle(PORTC, 4)
	#define toggle(reg,bit)	( reg ^= _BV(bit))
#endif

// port address helpers
#define DDR(x) ((x)-1)    // address of data direction register of port x
#define PIN(x) ((x)-2)    // address of input register of port x

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

// constants
#define PI		3.14159265359

#endif
