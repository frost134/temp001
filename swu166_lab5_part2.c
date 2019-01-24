/*	Partner(s) Name & E-mail:

 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com

 *  Shuang Wu & frost134@live.com

 *	Lab Section: 022 

 *	Assignment: Lab 5  Exercise 2

 *	Exercise Description: 

 *	I acknowledge all content contained herein, excluding

    template or example code, is my own original work.

 */

#include <avr/io.h>

enum States{Start,reset,plus,minus,wait} state;
int flag = 0;
void Tick()
{

	switch(state)
	{
		case Start :if ((~PINA&0x01)&&(!(~PINA&0x02)))
		{
			state = plus;
		}
		else if ((!(~PINA&0x01))&&(~PINA&0x02))
		{
			state = minus;
		}
		else if((~PINA&0x01)&&(~PINA&0x02))
		{
			state = reset;
		}
		else {}
		break;
		case wait : if ((~PINA&0x01)&&(!(~PINA&0x02)))
		{
			if(flag == 0){state = plus;}
		}
		else if ((!(~PINA&0x01))&&(~PINA&0x02))
		{
			if(flag == 0){state = minus;}
		}
		else if((~PINA&0x01)&&(~PINA&0x02))
		{
			state = reset;
		}
		else if((!(~PINA&0x01))&&(!(~PINA&0x02)))
		{flag = 0;}
		else {}
		break;
		case plus : state = wait; break;
		case minus : state = wait; break;
		case reset : state = wait; break;
	}
	switch(state)
	{
		case Start : PORTB = 0x00; break;
		case wait : break;
		case plus : if (PINB == 0x09){flag = 1;}else{PORTB = PINB +1; flag = 1;} break;
		case minus : if (PINB == 0x00){flag = 1;}else{PORTB = PINB -1; flag = 1;} break;
		case reset : PORTB = 0x00;break;
	}
}


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xff; PORTB = 0x00;
	state = Start;
    while (1) 
    {
		Tick();
    }
}

