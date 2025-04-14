
#ifndef GLCD_H_
#define GLCD_H_

#include <avr/io.h>

#define DATAPORT    PORTC
#define DATADDR     DDRC
#define DATAPIN     PINC

#define CTRLPORT	PORTD
#define CTRLDDR		DDRD
/*****************************/
#define RST     0
#define CS1     1
#define CS2     2
#define EN      3
#define RW      4
#define RS      5
/*****************************/
#define E_DELAY  10

//#define CS_ACTIVE_LOW   1   //Define this if your GLCD CS
                              //is active low (refer to datasheet)

/****************************************Configure**********************************************/

extern void glcd_off();

extern void set_start_line(uint8_t line);

extern void glcd_init( uint8_t line);

extern void goto_xy(uint8_t x,uint8_t y);

extern void glcd_clrln(uint8_t ln);

extern void glcd_clrscr();

extern uint8_t glcd_read(uint8_t column);

/****************************************Text Writing**********************************************/

extern void glcd_putchar(uint8_t c, uint8_t x, uint8_t y, uint8_t l, uint8_t sz, uint8_t negative);

extern void glcd_puts(const char *c, uint8_t x, uint8_t y, uint8_t l, uint8_t sz, uint8_t space, uint8_t negative);

/****************************************Animation*************************************************/
/*
 * down is between 0 and 63 and also speed
*/
extern void shiftdown( uint8_t down, uint8_t speed );

extern void shiftup( uint8_t up, uint8_t speed );

/****************************************BMP Display***********************************************/
// x is between 0 to 127 and y between 0 to 7
extern void bmp_disp(const uint8_t *bmp, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

extern void bmp_disp_exit(const uint8_t *bmp);

void bmp_disp_enter(const uint8_t *bmp);

extern void bmp_disp_mirror(const uint8_t *bmp, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/***************************************Geometric Shapes*******************************************/

extern void h_line(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c);

extern void v_line(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c);

extern void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s, uint8_t c);

extern void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s,uint8_t c);

extern void cuboid(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t x21, uint8_t y21,
			uint8_t x22, uint8_t y22, uint8_t s, uint8_t c);

extern void h_parallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l,uint8_t s,uint8_t c);

extern void v_parallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l,uint8_t s,uint8_t c);

extern void h_parallelepiped(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t l1,
					  uint8_t x21, uint8_t y21,uint8_t x22, uint8_t y22, uint8_t l2, uint8_t s, uint8_t c);

extern void v_parallelepiped(uint8_t x11,uint8_t y11, uint8_t x12,uint8_t y12,uint8_t l1,
					  uint8_t x21,uint8_t y21, uint8_t x22,uint8_t y22,uint8_t l2,uint8_t s,uint8_t c);

extern void circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t s, uint8_t c);

#endif /* GLCD_H_ */
