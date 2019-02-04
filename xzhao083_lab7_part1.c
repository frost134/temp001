/*
 * xzhao083_lab7_part1.c
 *
 * Created: 2019/1/31/周四 12:59:04
 * Author : Zinxp
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
int counter;
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

enum States1{Start1, counting} state1;
enum States2{Start2, plus_P, plus_R, minus_P, minus_R, reset} state2;
void Tick1()
{

	switch(state1)
	{
		case Start1: if (!GetBit(PINA, 0) && GetBit(PINA, 1)) {
							state1 = counting;
							counter += 1;break;
						}
					else if (GetBit(PINA, 0) && !GetBit(PINA, 1)) {
							state1 = counting;
							counter -= 1;break;
						}
					else state1 = Start1;break;
		case counting:  if (!GetBit(PINA, 0) && GetBit(PINA, 1)) {
							counter += 1;break;
						}
						else if (GetBit(PINA, 0) && !GetBit(PINA, 1)) {
							counter -= 1;break;
						}
						else if (!GetBit(PINA, 0) && !GetBit(PINA, 1)) {state1 = Start1;break;}
						else state1 = counting;break;
	}
	switch(state1)
	{
		case Start1: 
		case counting:	if (counter < 0) counter = 0;
						if (counter > 9) counter = 9;
						LCD_ClearScreen();
						LCD_WriteData(counter + '0');break;
	}
}

void Tick2()
{

	switch(state2)
	
	{
		case reset:
		case Start2: if (!GetBit(PINA, 0) && GetBit(PINA, 1)) {
						state2 = plus_P;break;
					}
					else if (GetBit(PINA, 0) && !GetBit(PINA, 1)) {
						state2 = minus_P;break;
					}
					else if (!GetBit(PINA, 0) && !GetBit(PINA, 1)) {
						 state2 = reset;break;}
					else state2 = Start2;break;
		case plus_P: if (GetBit(PINA, 0)) {counter += 1;state2 = plus_R; break;}
					 else if (!GetBit(PINA, 0) && !GetBit(PINA, 1)) {state2 = reset;break;}
					 else state2 = plus_P;break;
		case minus_P:if (GetBit(PINA, 1)) {counter -= 1;state2 = minus_R; break;}
				     else if (!GetBit(PINA, 0) && !GetBit(PINA, 1)) {state2 = reset;break;}
					 else state2 = minus_P;break;
		case plus_R: state2 = Start2;break;
		case minus_R:state2 = Start2;break;
					 
	}
	switch(state2)
	{
		case reset: counter = 0;
		case minus_R:
		case plus_R:    if (counter < 0) counter = 0;
						if (counter > 9) counter = 9;
						LCD_ClearScreen();
						LCD_WriteData(counter + '0');break;
		case minus_P:
		case plus_P:
		case Start2: break;
		
	}
}

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	DDRA = 0x00; PORTA = 0xFF;
	TimerSet(1000);
	TimerOn();
	counter = 0;
	LCD_init();
	state1 = counting;
	state2 = Start2;
	while(1){
		Tick1();
		while (!TimerFlag){Tick2();};
		TimerFlag = 0;
	}
}
