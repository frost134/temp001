/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.co
 *	Lab Section: 022 
 *	Assignment: Lab 3  Exercise 2
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */

#include <avr/io.h>
// Bit-access functions
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ?  (x | (0x01 << k))  :  (x & ~(0x01 << k)) );
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char tmpA = 0;
	unsigned char tmpC = 0x00;
    while (1) 
    {
		tmpC = 0x00;
		tmpA = PINA & 0x0F;
		switch(tmpA)
		{
			case 15:
			case 14:
			case 13: tmpC = SetBit(tmpC, 0, 1);
			case 12:
			case 11:
			case 10: tmpC = SetBit(tmpC, 1, 1);
			case 9: 
			case 8:
			case 7: tmpC = SetBit(tmpC, 2, 1);
			case 6:
			case 5: tmpC = SetBit(tmpC, 3, 1);
			case 4:
			case 3: tmpC = SetBit(tmpC, 4, 1);
			case 2:
			case 1: tmpC = SetBit(tmpC, 5, 1); break;
			case 0: tmpC = 0x00; break;
		}
		if (GetBit(PINA, 4) && GetBit(PINA, 5) && !GetBit(PINA, 6)) tmpC = SetBit(tmpC, 7, 1);
		if (tmpA <= 4) tmpC = SetBit(tmpC, 6, 1);
		PORTC = tmpC; 
    }
	return 0;
}