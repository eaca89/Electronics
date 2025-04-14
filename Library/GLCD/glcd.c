#include <avr/pgmspace.h>
#include "global.h"
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "glcd.h"
#include "font.h"

//--------Arabic----------
static uint8_t prevLet = 193;		//previous letter, 193: where first farsi letter begins
static uint8_t stat = 0;
static uint8_t prevX = 0;
static uint8_t prevY = 0;


/****************************************Configure**********************************************/
static void trigger() {

    sbi(CTRLPORT, EN);
    _delay_us(E_DELAY);
    cbi(CTRLPORT, EN);
    _delay_us(E_DELAY);
}

void glcd_off() {

	#ifdef CS_ACTIVE_LOW
		cbi(CTRLPORT, CS1);			//active both chips
		cbi(CTRLPORT, CS2);
	#else
		sbi(CTRLPORT, CS1);			//active both chips
		sbi(CTRLPORT, CS2);
	#endif
	//set Command
	cbi(CTRLPORT, RS);
	//write mode
	cbi(CTRLPORT, RW);

	//send command through port
	DATAPORT = 0x3e;
	trigger();
}

static void glcd_on() {

	sbi(CTRLPORT, RST);
    _delay_us(10);

	#ifdef CS_ACTIVE_LOW
		cbi(CTRLPORT, CS1);			//active both chips
		cbi(CTRLPORT, CS2);
	#else
		sbi(CTRLPORT, CS1);			//active both chips
		sbi(CTRLPORT, CS2);
	#endif
    _delay_us(10);

    cbi(CTRLPORT, RS);				//set command
    cbi(CTRLPORT, RW);				//write mode

    DATAPORT = 0x3f;				//send command through port
    trigger();
}

void set_start_line(uint8_t line) {


	#ifdef CS_ACTIVE_LOW
		cbi(CTRLPORT, CS1);			//active both chips
		cbi(CTRLPORT, CS2);
	#else
		sbi(CTRLPORT, CS1);			//active both chips
		sbi(CTRLPORT, CS2);
	#endif

	cbi(CTRLPORT, RS);				//set command
	cbi(CTRLPORT, RW);				//write mode

	DATAPORT= (0xc0 | line);
	trigger();
}

void glcd_init( uint8_t line) {

	DATADDR = 0XFF;
	CTRLDDR |= ( _BV(RST) | _BV(CS2) | _BV(CS1) | _BV(EN) | _BV(RW) | _BV(RS) );
	glcd_on();
	glcd_clrscr();
	set_start_line( line );
}

static void goto_col(uint8_t x) {

	uint8_t pattern;

   cbi(CTRLPORT, RS);			 //RS low --> command
   cbi(CTRLPORT, RW);			 //RW low --> write

   if( x < 64 ) {             	//left section

	#ifdef CS_ACTIVE_LOW
		cbi(CTRLPORT, CS1);			//active chip 1
		sbi(CTRLPORT, CS2);
	#else
		sbi(CTRLPORT, CS1);			//active chip 1
		cbi(CTRLPORT, CS2);
	#endif

        pattern = x;      		//put column address on data port
   }
   else {                		//right section

	#ifdef CS_ACTIVE_LOW
		sbi(CTRLPORT, CS1);			//active chip 2
		cbi(CTRLPORT, CS2);
	#else
		cbi(CTRLPORT, CS1);			//active chip 2
		sbi(CTRLPORT, CS2);
	#endif

        pattern = (x - 64);   	//put column address on data port
   }
   pattern = ( pattern | 0x40 ) & 0x7F;  //Command format
   DATAPORT = pattern;
   trigger();
}

static void goto_row(uint8_t y) {

	uint8_t pattern;

    cbi(CTRLPORT, RS);				//RS low --> command
    cbi(CTRLPORT, RW);				//RW low --> write

    pattern = ( y | 0xB8 ) & 0xBF; 	//put row address on data port set command
    DATAPORT = pattern;
    trigger();
}

void goto_xy(uint8_t x,uint8_t y) {

    goto_col(x);
    goto_row(y);
}

static void glcd_write(uint8_t w) {

	sbi(CTRLPORT, RS);			//RS high --> data
	cbi(CTRLPORT, RW);			//RW low --> write
	DATAPORT = w;    			//put data on data port
	_delay_us(1);
	trigger();
}

void glcd_clrln(uint8_t ln) {

    goto_xy(0,ln);      //At start of line of left side
    goto_xy(64,ln);     //At start of line of right side (Problem)

	#ifdef CS_ACTIVE_LOW
		cbi(CTRLPORT, CS1);			//active chip 1
	#else
		sbi(CTRLPORT, CS1);			//active chip 1
	#endif

    for(uint8_t i = 0; i < 65; ++i)
        glcd_write(0);
}

void glcd_clrscr() {

   for(uint8_t i = 0; i < 8; ++i)
      glcd_clrln(i);
}

/*static void glcd_write_address(uint8_t columns, uint8_t page, uint8_t data) {

	//select chip
	if(columns >= 64) {

	#ifdef CS_ACTIVE_LOW
		sbi(CTRLPORT, CS1);			//active chip 2
		cbi(CTRLPORT, CS2);
		#else
			cbi(CTRLPORT, CS1);			//active chip 2
			sbi(CTRLPORT, CS2);
		#endif
	}
	else {
		#ifdef CS_ACTIVE_LOW
			cbi(CTRLPORT, CS1);			//active chip 1
			sbi(CTRLPORT, CS2);
		#else
			sbi(CTRLPORT, CS1);			//active chip 1
			cbi(CTRLPORT, CS2);
		#endif
    }

    cbi(CTRLPORT, RS);			//set command
    cbi(CTRLPORT, RW);			//write mode

    DATAPORT=0xB8 | page;
    trigger();

    cbi(CTRLPORT, RS);			//set command
    cbi(CTRLPORT, RW);			//write mode

    DATAPORT=0x40 | columns;
    trigger();

    sbi(CTRLPORT, RS);
    cbi(CTRLPORT, RW);

    DATAPORT=data;
    trigger();
}*/

/*unsigned char is_busy()
{
    unsigned status = 0;        //Read data here

    EN = 0;                 //Low Enable
    delay_us(1);            //tf
    RW = 1;                 //Read
    RS = 0;                 //Status
    delay_us(1);            //tasu
    EN = 1;                 //High Enable
    delay_us(5);            //tr + max(td,twh)->twh

    //Dummy read
    EN = 0;                 //Low Enable
    delay_us(5);            //tf + twl + chineese error

    EN = 1;                 //High Enable
    delay_us(1);            //tr + td

    status = DATAPIN;    //Input data
    EN = 0;                 //Low Enable
    delay_us(1);            //tdhr
    return (status & 0x80);
}*/

uint8_t glcd_read(uint8_t column) {

    uint8_t read_data = 0; 			//Read data here
    DATADDR = 0x00;     			//Input

    //while(is_busy());
    sbi(CTRLPORT, RW);				//Read
    sbi(CTRLPORT, RS);				//Data

    if( column < 64) {
		#ifdef CS_ACTIVE_LOW
			cbi(CTRLPORT, CS1);			//active chip 1
			sbi(CTRLPORT, CS2);
		#else
			sbi(CTRLPORT, CS1);			//active chip 1
			cbi(CTRLPORT, CS2);
		#endif
    }
    else {
		#ifdef CS_ACTIVE_LOW
			sbi(CTRLPORT, CS1);			//active chip 2
			cbi(CTRLPORT, CS2);
		#else
			cbi(CTRLPORT, CS1);			//active chip 2
			sbi(CTRLPORT, CS2);
		#endif
    }
    _delay_us(1);        //tasu
	sbi(CTRLPORT, EN);             //Latch RAM data into ouput register
    _delay_us(1);        //twl + tf

    //Dummy read
    //while(is_busy());
    cbi(CTRLPORT, EN);             //Low Enable
    _delay_us(20);       //tf + twl + chineese error

    sbi(CTRLPORT, EN);             //latch data from output register to data bus
    _delay_us(1);        //tr + td(twh)

    read_data = DATAPIN;    //Input data
    cbi(CTRLPORT, EN);             //Low Enable to remove data from the bus
    _delay_us(1);        //tdhr

    DATADDR = 0xFF;     //Output again
    return read_data;
}

/****************************************Text Writing**********************************************/


static void putIt(uint8_t c, uint8_t negative, uint8_t x, uint8_t y) {

	goto_xy(x, y);
	for(uint8_t i = 0; i < 8; ++i) {

		if( !negative) glcd_write( pgm_read_byte( & font[( 8 * c ) + i ] ) );
		if( negative)  glcd_write( ~ pgm_read_byte( & font[ ( 8 * c ) + i ] ) );          //to select
	}
}

static void enlarge(uint8_t * large, uint8_t c, uint8_t size) {

	uint8_t k = 0, temp;
	for(uint8_t i = 0; i < size; ++i) {

		for(uint8_t j = 0; j < 8; ++j) {

			large[i] >>= 1;
			temp = c & 0x01;
			if(temp)
			//write 1 according to LSB of C
				large[i] |= 0x80;
			//shift C (8/sz) time in each loop
			//exmp: sz = 1  => in each loop(j loop) C is shifted 8 times
			//exmp: sz = 2  => in each loop(j loop) C is shifted 4 times
			//this is used to copy each bit sz time
			//exmp: sz = 2  ==>each bit will be copied 2 times
			if(++k == size) {
			// shift C by one for making new LSB
				c >>= 1;
				k = 0;
			}
		}
	}
}

static void putItSz(uint8_t c, uint8_t x, uint8_t y, uint8_t sz, uint8_t negative) {

	uint8_t large[8][sz];

	goto_xy(x, y);
	for(uint8_t i = 0; i < 8; ++i) {

		//store new resized byte(for each column) in new array
		if( ! negative)     enlarge(  large[i], pgm_read_byte( & font[( 8 * c) + i ]), sz);		//c here;GCC (c-1)
		if(negative)      enlarge( large[i], ~ pgm_read_byte( & font[( 8 * c ) + i ] ), sz);		//c here;GCC (c-1)
	}
	for(uint8_t j = 0; j < sz; ++j) {

		for(uint8_t i = 0; i < 8; ++i) {

			//increasing width
			for(uint8_t k = 0; k < sz; ++k) {

				if(x + k + (sz * i) == 64)	goto_xy(64, y);		//Problem 1: Forgot to change 3 to sz
				glcd_write(large[i][j]);
			}
		}
		goto_xy(x,++y);
	}
}

void glcd_putchar(uint8_t c, uint8_t x, uint8_t y, uint8_t l, uint8_t sz, uint8_t negative) {

	if(l == 1) {

		switch(c) {

			case 129:
				c = 250;
				break;
			case 144:
				c = 251;
				break;
			case 152:
				c = 252;
				break;
			case 142:
				c = 253;
				break;
			case 141:
				c = 254;
				break;
			case 48:
				c = 1;			//farsi number 0
				break;
			case 49:
				c = 2;			//farsi number 1
				break;
			case 50:
				c = 3;			//farsi number 2
				break;
			case 51:
				c = 4;			//farsi number 3
				break;
			case 52:
				c = 5;			//farsi number 4
				break;
			case 53:
				c = 6;			//farsi number 5
				break;
			case 54:
				c = 7;			//farsi number 6
				break;
			case 55:
				c = 8;			//farsi number 7
				break;
			case 56:
				c = 9;			//farsi number 8
				break;
			case 57:
				c = 10;			//farsi number 9
				break;
		}

		if( (c >= 193) && (prevLet >= 193) && ( pgm_read_byte ( & map[prevLet-193][5] ) ) && ( pgm_read_byte ( & map[c-193][4] ) ) ) {

			putIt( pgm_read_byte ( & map[prevLet-193][stat+1] ), negative,prevX,prevY);
			stat = 2;
		}else stat = 0;

		if(c >= 193)	putItSz( pgm_read_byte ( & map[c-193][stat] ), x, y, sz, negative);
		else putItSz(c, x, y, sz, negative);

		prevLet = c;
		prevX = x;
		prevY = y;

	}
	else putItSz( c, x, y, sz, negative);
}

void glcd_puts(const char *c, uint8_t x, uint8_t y, uint8_t l, uint8_t sz, uint8_t space, uint8_t negative) {

	uint8_t j = 0, z = 0, font = 6;
	char special = 0;
	uint8_t num[15], fnum[15];

	uint8_t i = 0;
	//English writing
	while( ( i < strlen(c) ) && l == 0 ){

		glcd_putchar(* (c + i), x, y, 0, sz, negative);

		if( * ( c + i ) == 32)				//If space (i.e. new word)
			x += (2 + space) * sz;			//Space between words
		else
			x += (6 + space) * sz;

		if(x > 128 - 7 * sz) {

			 x = 0;
			 y += sz;
		}
		i++;
	}
	//Farsi/Arabic writing
	while( ( i < strlen( c ) ) && l == 1 ) {

		//correct writing persian numbers on GLCD
		//Persian numbers should be saved in an array inverse.
		//in while loop persian numbers will be gathered in an array called num
		//j is nums's subscript
		while((*(c+i))==46 || (*(c+i))==48 || (*(c+i))==49 || (*(c+i))==50 || (*(c+i))==51 ||
			(*(c+i))==52 || (*(c+i))==53 || (*(c+i))==54 || (*(c+i))==55 || (*(c+i))==56 || (*(c+i))==57) {

			num[j]=( *( c + i ) );
			i++;
			j++;
		}
		//check if anything has been saved to num array
		if(j > 0) {

			z=0;
			//because one extra increament in while loop it should be decreased
			j--;
			//inverse num attay and save it in fnum array
			for( ; j >= 0; --j) {

				fnum[z] = *(num + j);
				z++;
			}
			j=0;
			for( ; j <= z; j++) {

				glcd_putchar(* (fnum + j), x - font * sz, y, 1, sz, negative);
				x -= font * sz;
			}
			//reset both arrays
			for(j = 0; j < 15; j++) {
				*(num + j) = 0;
				*(fnum + j) = 0;
			}
			j = 0; z = 0;
		}

		if( ( * (c + i) == 225) && ( * (c + i + 1) == 199))			special = 249;
		else if( ( * (c + i) == 225) && ( * (c + i + 1) == 195))	special = 231;
		else if( ( * ( c + i) == 225) && ( * (c + i + 1) == 194) )	special = 232;
		else if( ( * (c + i) == 225) && ( * (c + i + 1) == 197))	special = 233;

		if(special) {

			glcd_putchar(special, x-font * sz, y, 1, sz, negative);
			i += 2;
			x -= font * sz;
			special = 0;
		} else {

			glcd_putchar( * ( c + i ), x - font * sz, y, l, sz, negative);
			if( * ( c + i ) == 32) {						//If space (i.e. new word)
				x -= (0 + space) * sz;				//Space between words
			}
			else {

				x -= font * sz;
			}
			i++;
		}
		if(x < font * sz + 1) {

		x = 128 - font * sz;
		y += sz;
	}
	}
	prevLet = 193;
}

/****************************************Animation*************************************************/
/*
void shiftdown( uint8_t down, uint8_t speed ) {
	uint8_t i;

	for(i = 63; i >= down; i -= speed) {

		set_start_line( i );
		_delay_ms(100);
	}
}

void shiftup(uint8_t up, uint8_t speed) {

	for( uint8_t i = 0; i <= up; i += speed) {

		set_start_line( i );
		_delay_ms(100);
    }
}
*/
/****************************************BMP Display***********************************************/
//void bmp_disp(const uint8_t *bmp, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
//
	//uint8_t x, y;
	//for( y = y1; y <= y2; y++) {
//
		//for( x = x1; x <= x2; x++) {
//
			//goto_xy(x, y);
			//glcd_write( pgm_read_byte( &bmp[(128 * y) + x] ) );
		//}
	//}
//}

void bmp_disp(const uint8_t *bmp, uint8_t x1, uint8_t y1, uint8_t dx, uint8_t dy)
{

	uint8_t x, y;
	for( y = y1; y < y1 + (dy/8); y++) {

		for( x = x1; x < x1  + dx; x++)
		{
			goto_xy(x, y);
			glcd_write( pgm_read_byte( &bmp[(dx) * (y - y1) + (x - x1)] ) );
		}
	}
}


void bmp_disp_exit(const uint8_t *bmp)
{
	bmp_disp(bmp, 0, 0, 127, 63);
	uint8_t delta = 0, x = 0;
	for (uint8_t dx = 127 ; dx != 255 ; dx--) //range of the column
	{
		for (x = 0 ; x <= dx ; x++) //change the column
		{
			for (uint8_t y = 0 ; y <= 7 ; y++)  // write a column
			{
				goto_xy(delta , y);
				glcd_write(pgm_read_byte( &bmp[128*y + x] ));
				//_delay_ms(5);
			}
			delta++;
		}
		delta = 127 - x;
	}
}

void bmp_disp_enter(const uint8_t *bmp) {
	
	for (uint8_t dx = 127 ; dx != 255 ; dx--) //range of the column
	{
		uint8_t delta = 0;
		for (uint8_t x = dx ; x < 128 ; x++) //change the column
		{
			for (uint8_t y = 0 ; y <= 7 ; y++) //write a column
			{
				goto_xy(delta , y);
				glcd_write(pgm_read_byte( &bmp[128*y + x] ));
				//_delay_ms(5);
			}
			delta++;
		}
	}
}


void bmp_disp_mirror(const uint8_t *bmp, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

	uint8_t x, y;
	for( y = y1; y <= y2; y++) {

		for( x = x1; x < 128; ++x) {

			goto_xy(127 - x, y);
			glcd_write( pgm_read_byte( &bmp[(128 * y) + x] ) );
		}
	}
}
/***************************************Geometric Shapes*******************************************/


static void point_at(uint8_t x, uint8_t y, uint8_t color) {

	uint8_t pattern = 0;
	goto_xy( x, (int)(y / 8) );
	switch (color)
	{
		case 0:         //Light spot
			pattern = ~ ( 1 << (y % 8) ) & glcd_read(x);
			break;
		case 1:         //Dark spot
			pattern = ( 1 << (y % 8) ) | glcd_read(x);
			break;
	}
	goto_xy(x, (int)(y / 8) );
	glcd_write(pattern);
}

void h_line(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c) {
	uint8_t i;
    for(i=x; i < (l+x); i += (uint8_t)(s+1))
        point_at(i,y,c);
}

void v_line(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c) {
	uint8_t i;
    for(i=y; i<(y+l); i += (uint8_t)(s+1))
        point_at(x,i,c);
}

void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s, uint8_t c) {

	uint8_t i;
	uint8_t y01;

	uint8_t temp;

    float a;
    float b;

    uint8_t y00;
    uint8_t y010;

    if(x1 == x2) {
        v_line(x1, MIN(y1,y2), abs(y2-y1)+1, s, c);
    }
    else if(y1 == y2) {
        h_line(MIN(x1,x2), y1, abs(x2-x1)+1, s, c);
    }
    else
    {
        if(x1>x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
            temp = y1;
            y1 = y2;
            y2 = temp;
        }

        a = (float)(signed)(y2-y1)/(x2-x1);
        b = y1 - a*x1;
        y00 = a*x1 + b;
        y010 = y00;

        for(i=(x1+1); i<=x2; i ++)
        {
            y01 = a*i + b + 0.5; //+ 0.5 to approximate to the nearest integer

            if( (signed char)(y01 - y010) > 1 )
            {
                //-------Subject to error
                v_line((i-1),(uint8_t)(y010+1),(y01-y010-1),s,c);
            }
            else if( (signed char)(y010 - y01) > 1 )
            {
                //-------Subject to error
                v_line((i-1),(uint8_t)(y01+1),(y010-y01-1),s,c);
            }

            if((i==x2) && (y00 == y01)) h_line(x1,y01,(x2-x1),0,c);
            y010 = y01;
            if( y00 ==  y01) continue;
            h_line(x1,y00,(i-x1),0,c);
            x1 = i;
            y00 = y01;
        }
        point_at(x2,y2,c);
    }

}

void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s,uint8_t c) {
    h_line(x1,y1,(x2-x1),s,c);
    h_line(x1,y2,(x2-x1),s,c);
    v_line(x1,y1,(y2-y1),s,c);
    v_line(x2,y1,(y2-y1+1),s,c);
}

void cuboid(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t x21, uint8_t y21,
			uint8_t x22, uint8_t y22, uint8_t s, uint8_t c)
{
    rectangle(x11,y11,x12,y12,s,c);
    rectangle(x21,y21,x22,y22,s,c);
    line(x11,y11,x21,y21,s,c);
    line(x12,y11,x22,y21,s,c);
    line(x11,y12,x21,y22,s,c);
    line(x12,y12,x22,y22,s,c);
}

void h_parallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l,uint8_t s,uint8_t c) {
    h_line(x1,y1,l,s,c);
    h_line((x2-l+1),y2,l,s,c);
    line(x1,y1,(x2-l+1),y2,s,c);
    line((x1+l-1),y1,x2,y2,s,c);
}

void v_parallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l,uint8_t s,uint8_t c) {
    v_line(x1,y1,l,s,c);
    v_line(x2,(y2-l+1),l,s,c);
    line(x1,y1,x2,(y2-l+1),s,c);
    line(x1,(y1+l-1),x2,y2,s,c);
}

void h_parallelepiped(uint8_t x11, uint8_t y11,
					  uint8_t x12, uint8_t y12, uint8_t l1,
					  uint8_t x21, uint8_t y21,
					  uint8_t x22, uint8_t y22, uint8_t l2,
                      uint8_t s, uint8_t c)
{
    h_parallelogram(x11,y11,x12,y12,l1,s,c);
    h_parallelogram(x21,y21,x22,y22,l2,s,c);
    line(x11,y11,x21,y21,s,c);
    line(x12,y12,x22,y22,s,c);
    line((x11+l1-1),y11,(x21+l2-1),y21,s,c);
    line((x12-l1+1),y12,(x22-l2+1),y22,s,c);
}

void v_parallelepiped(uint8_t x11,uint8_t y11,
					  uint8_t x12,uint8_t y12,uint8_t l1,
					  uint8_t x21,uint8_t y21,
					  uint8_t x22,uint8_t y22,uint8_t l2,
                      uint8_t s,uint8_t c)
{
    v_parallelogram(x11,y11,x12,y12,l1,s,c);
    v_parallelogram(x21,y21,x22,y22,l2,s,c);
    line(x11,y11,x21,y21,s,c);
    line(x12,y12,x22,y22,s,c);
    line(x11,(y11+l1-1),x21,(y21+l2-1),s,c);
    line(x12,(y12-l1+1),x22,(y22-l2+1),s,c);
}

void circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t s, uint8_t c) {

    uint8_t i, y = 0, y00;
    y00 = r;            //Point (0,r) is the 1st point
    for(i=0; i<r; i++)
    {
        y = sqrt((int)r*r - (int)i*i);
        point_at((x0+i),(y0+y),c);
        point_at((x0+i),(y0-y),c);
        point_at((x0-i),(y0+y),c);
        point_at((x0-i),(y0-y),c);
        if(abs(y00-y)>1)
        {
            v_line(x0+(i-1),y0+MIN(y00,y)+1,abs(y00-y),s,c);
            v_line(x0+(i-1),y0-MAX(y00,y),abs(y00-y),s,c);
            v_line(x0-(i-1),y0+MIN(y00,y)+1,abs(y00-y),s,c);
            v_line(x0-(i-1),y0-MAX(y00,y),abs(y00-y),s,c);
        }
        y00 = y;
    }
    v_line(x0+(i-1) ,y0,y ,s,c);
    v_line(x0+(i-1) ,y0-y ,y,s,c);
    v_line(x0-(i-1) ,y0,y ,s,c);
    v_line(x0-(i-1) ,y0-y ,y,s,c);
}
