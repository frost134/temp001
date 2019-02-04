/*
 * xzhao083_lab7_part2.c
 *
 * Created: 2019/2/3/周日 21:19:53
 * Author : Zinxp
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
int score;
volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States1{Start1, right_P, right_R} state1;
enum States2{Start2, wrong_P, wrong_R} state2;
void Tick1()
{
	switch(state1)
	{
		case Start1: if (!GetBit(PINA, 0)) {state1 = right_P;break;}
					 else break;
		case right_P: if (GetBit(PINA, 0)) {state1= right_R;break;}
					  else break;
		case right_R: score++; state1 = Start1;break;
		}
	switch(state1)
	{
		case right_R:if (score < 0) score = 0;
					if (score > 9) {
						score = 9;
						LCD_ClearScreen();
						LCD_DisplayString(1, "!!!Victory!!!");break;
					}
					LCD_ClearScreen();
					LCD_WriteData(score + '0');break;
		case right_P:
		case Start1: break;
	}
}
void Tick2()
{
	switch(state2)
	{
		case Start2: if (!GetBit(PINA, 0)) {state2 = wrong_P;break;}
					 else break;
		case wrong_P: if (GetBit(PINA, 0)) {state2= wrong_R;break;}
					  else break;
		case wrong_R: score--; state2 = Start2;break;
	}
	switch(state2)
	{
		case wrong_R:if (score < 0) score = 0;
				 	 if (score > 9) {
						  score = 9;
						  LCD_ClearScreen();
						  LCD_DisplayString(1, "!!!Victory!!!");break;
					  }
					 LCD_ClearScreen();
					 LCD_WriteData(score + '0');break;
		case wrong_P:
		case Start2: break;
	}
}
int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	DDRA = 0x00; PORTA = 0xFF;
	TimerSet(500);
	TimerOn();
	score = 4;
	LCD_init();
	state1 = Start1;
	state2 = Start2;
	int i;
	i = 0;
	while(1){
			if (i%2 == 0){
				PORTB = 0x01;
				while (!TimerFlag){Tick1();};
				TimerFlag = 0;
				i++;
				}
			else
				{
				PORTB = 0x02;
				while (!TimerFlag){Tick2();};
				TimerFlag = 0;
				i++;
				}
	}
}