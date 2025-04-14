/*
 _
|_|
|_|

*/

#ifndef SEVEN_SEGCODE_H_
#define SEVEN_SEGCODE_H_

//a is most important bit
//g and DP are least important bits
//common cathode
//uint8_t Codes[] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6};
/*                0      1     2     3     4      5     6     7    8      9*/
//g is most important bit
//a and DP are least important bits
//common cathode
uint8_t Codes[] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x0E, 0xFE, 0xDE};
/*                0      1     2     3     4      5     6     7    8      9*/

#endif /* KEYPAD_H_ */