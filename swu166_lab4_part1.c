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

enum States{Start,B0off_p,B0off_r,B0on_p, B0on_r} state;
void Tick()
{
	switch(state)
	{
		case Start : state = B0on_r; break;
		case B0on_r : if(PINA&0x01){state = B0off_p;}; break;
		case B0off_p : if(!(PINA&0x01)){state = B0off_r;}; break;
		case B0off_r : if(PINA&0x01){state = B0on_p;}; break;
		case B0on_p : if(!(PINA&0x01)){state = B0on_r;}; break;
	}
	switch(state)
	{
		case Start : PORTB = 0x01; break;
		case B0on_r : PORTB = 0x01; break;
		case B0off_p : PORTB = 0x02; break;
		case B0off_r : PORTB = 0x02; break;
		case B0on_p : PORTB = 0x01;break;
	}
}


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0xff; PORTB = 0x00;
    while (1) 
    {
		Tick();
    }
}

