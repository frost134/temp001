/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 3  Exercise 1
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */

#include <avr/io.h>
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
int main(void)
{
	DDRB = 0x06; PORTB = 0xF9;
	DDRD = 0x00; PORTD = 0xFF;
    unsigned short weight;
	unsigned char tmpB;
    while (1) 
    {
		tmpB = 0x00;
		weight = (PIND << 1) | GetBit(PINB, 0);
		
		if (weight >= 70) {
			tmpB = SetBit(tmpB, 1, 1);
			}
		else if (weight >= 5) {
			tmpB = SetBit(tmpB, 2, 1);
			}
		PORTB =  GetBit(PINB, 0) | tmpB;
    }
}
