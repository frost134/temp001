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
enum States{Start, P_X, P_Y, P_hashtag, R_hashtag, R_X, R_Y, UNLOCK, UNLOCK_perv, LOCK_R_X, LOCK_R_Y, LOCK_R_hashtag, LOCK_Start, LOCK_prev} state;
extern int I;
int I = 0;
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

void Code_check()
{
	int code[] = {3, 1, 2, 1};
	if (state == code[I]) I++;
	if (I == 4)
	{
		I = 0;
		state = GetBit(PINB, 0)?LOCK_prev:UNLOCK_perv;
	}
}
void Tick()
{
	switch(state)
	{
		case Start:		if (GetBit(PINA, 0)) state = P_X;
						else if (GetBit(PINA, 1)) state = P_Y;
						else if (GetBit(PINA, 2)) state = P_hashtag;
						else if (GetBit(PINA, 7)) state = LOCK_Start;
						else state = Start;
						break;
		case P_X:		state = GetBit(PINA, 0) ? P_X:R_X; break;
		case P_Y:		state = GetBit(PINA, 1) ? P_Y:R_Y; break;
		case P_hashtag:	state = GetBit(PINA, 2) ? P_hashtag:R_hashtag; break;
		case R_X:		if (GetBit(PINA, 0)) state = P_X;
						else if (GetBit(PINA, 1)) state = P_Y;
						else if (GetBit(PINA, 2)) state = P_hashtag;
						else if (GetBit(PINA, 7)) state = LOCK_R_X;
						else state = R_X;
						break;
		case R_Y:		if (GetBit(PINA, 0)) state = P_X;
						else if (GetBit(PINA, 1)) state = P_Y;
						else if (GetBit(PINA, 2)) state = P_hashtag;
						else if (GetBit(PINA, 7)) state = LOCK_R_Y;
						else state = R_Y;
						break;
		case R_hashtag:	if (GetBit(PINA, 0)) state = P_X;
						else if (GetBit(PINA, 1)) state = P_X;
						else if (GetBit(PINA, 2)) state = P_hashtag;
						else if (GetBit(PINA, 7)) state = LOCK_R_hashtag;
						else state = R_hashtag;
						break;
		case UNLOCK_perv: state = UNLOCK; break;
		case UNLOCK: state = Start; break;
		case LOCK_R_hashtag: state = R_hashtag; break;
		case LOCK_R_X: state = R_X; break;
		case LOCK_R_Y: state = R_Y; break;
		case LOCK_Start: state = Start; break;
		case LOCK_prev: state = LOCK_Start; break;
	}
	
	
	switch(state)
	{
		case UNLOCK:	PORTB = PINB | 0x01; break;
		case LOCK_Start:		
		case LOCK_R_hashtag:
		case LOCK_R_X:
		case LOCK_R_Y: PORTB = PINB & 0xFE; break;
		case P_hashtag:
		case P_X:
		case P_Y:	Code_check();break;
		case LOCK_prev:
		case UNLOCK_perv: 
		case Start:
		case R_Y:
		case R_X:
		case R_hashtag: break;
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