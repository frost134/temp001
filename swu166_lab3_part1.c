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
// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0x00;
	DDRC = 0xff; PORTC = 0x00;
	unsigned char countC = 0;
    while (1) 
    {
		countC = 0;
		for (unsigned char i = 0; i <8; i++)
		{
			countC = countC + GetBit(PINA,i) + GetBit(PINB,i);
		}
		PORTC = countC;
    }
}

