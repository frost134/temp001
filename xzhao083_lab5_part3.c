/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 5  Exercise 3
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, state1, state2, state3, state4, state5, state6, state7, state1R, state2R, state3R, state4R, state5R, state6R, state7R} state;
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
void Tick()
{
	switch(state)
	{
		case Start:		state = !GetBit(PINA, 0)?state1:Start;break;
		case state1:	state = !GetBit(PINA, 0)?state1:state1R;break;
		case state1R:	state = !GetBit(PINA, 0)?state2:state1R;break;
		case state2:	state = !GetBit(PINA, 0)?state2:state2R;break;
		case state2R:	state = !GetBit(PINA, 0)?state3:state2R;break;
		case state3:	state = !GetBit(PINA, 0)?state3:state3R;break;
		case state3R:	state = !GetBit(PINA, 0)?state4:state3R;break;
		case state4:	state = !GetBit(PINA, 0)?state4:state4R;break;
		case state4R:	state = !GetBit(PINA, 0)?state5:state4R;break;
		case state5:	state = !GetBit(PINA, 0)?state5:state5R;break;
		case state5R:	state = !GetBit(PINA, 0)?state6:state5R;break;
		case state6:	state = !GetBit(PINA, 0)?state6:state6R;break;
		case state6R:	state = !GetBit(PINA, 0)?state7:state6R;break;
		case state7:	state = !GetBit(PINA, 0)?state7:state7R;break;
		case state7R:	state = !GetBit(PINA, 0)?state1:state7R;break;
	}
	switch(state)
	{
		
		case Start:		PORTB = 0x00; break;
		case state1:	PORTB = 0x21; break;
		case state2:	PORTB = 0x12; break;
		case state3:	PORTB = 0x0C; break;
		case state4:	PORTB = 0x1E; break;
		case state5:	PORTB = 0x3F; break;
		case state6:	PORTB = 0x2A; break;
		case state7:	PORTB = 0x15; break;

		case state1R:
		case state2R:
		case state3R:
		case state4R:
		case state5R:
		case state6R:
		case state7R: break;
	}

}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = Start;
    while (1) 
    {
		Tick();
    }
}
