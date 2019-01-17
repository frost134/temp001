/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 3  Exercise 4
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, B1_on, B1_hold, B0_on, B0_hold} state;
void Tick()
{
	switch(state)
	{
		case Start:		state = B0_hold; break;
		case B0_hold:	state = PINA & 0x01 ? B1_on:B0_hold; break;
		case B1_on:		state = PINA & 0x01 ? B1_on:B1_hold; break;
		case B1_hold:	state = PINA & 0x01 ? B0_on:B1_hold; break;
		case B0_on:		state = PINA & 0x01 ? B0_on:B0_hold; break;
	}
	switch(state)
	{
		case Start:	break;
		case B0_hold:	PORTB = 0x01; break;
		case B1_on:		PORTB = 0x02; break;
		case B1_hold:	PORTB = 0x02; break;
		case B0_on:		PORTB = 0x01; break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0xff; PORTB = 0x00;
	state = Start;
    while (1) 
    {
		Tick();
    }
}
