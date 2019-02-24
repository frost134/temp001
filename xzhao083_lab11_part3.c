/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 11  Exercise 1
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */
#include <avr/io.h>
#include <bit.h>
#include <keypad.h>
#include "io.c"
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0;

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	TCCR1B 	= 0x0B;	
	OCR1A 	= 125;
	TIMSK1 	= 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00;
}

typedef struct task {
	int state; // Current state of the task
	unsigned long period; // Rate at which the task should tick
	unsigned long elapsedTime; // Time since task's previous tick
	int (*TickFct)(int); // Function to call for task's tick
} task;
task tasks[1];
const unsigned char tasksNum = 1;
const unsigned long tasksPeriodGCD = 50;
void TimerISR() {
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if (tasks[i].elapsedTime >= tasks[i].period) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}
ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--; 			// Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { 	// results in a more efficient compare
		TimerISR(); 				// Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
unsigned char x;
enum States{start, _0, _1, _2 ,_3 ,_4, _5, _6, _7, _8, _9, A, B, C, D, hashtag, star, NONE};
int Tick(int state);


int main()
{
	DDRB = 0xF0; PORTB = 0x0F; // PORTB set to output, outputs init 0s
	DDRC = 0xFF; PORTC = 0x00; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;
	tasks[0].state = start;
	tasks[0].period = tasksPeriodGCD;
	tasks[0].elapsedTime = 0;
	tasks[0].TickFct = &Tick;
	LCD_init();
	LCD_ClearScreen();
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	while(1) {}
	return 0;
}


int Tick(int state)
{
	switch (state) {
		case start:
		x = GetKeypadKey();
		switch (x) {
			case '\0':state = NONE; break; // All 5 LEDs on
			case '1': state = _1; break; // hex equivalent
			case '2': state = _2; break;
			case '3': state = _3; break;
			case '4': state = _4; break;
			case '5': state = _5; break;
			case '6': state = _6; break;
			case '7': state = _7; break;
			case '8': state = _8; break;
			case '9': state = _9; break;
			case 'A': state = A; break;
			case 'B': state = B; break;
			case 'C': state = C; break;
			case 'D': state = D; break;
			case '*': state = star; break;
			case '0': state = _0; break;
			case '#': state = hashtag; break;
			default: break; // Should never occur. Middle LED off.
		}break;
		case NONE: // All 5 LEDs on
		case _1:
		case _2:
		case _3:
		case _4:
		case _5:
		case _6:
		case _7:
		case _8:
		case _9:
		case A:
		case B:
		case C:
		case D:
		case star:
		case _0:
		case hashtag: state = start; break;
		default:state = start; break; // Should never occur. Middle LED off.
	}
	switch (state) {
		case start:
		case NONE: break; // All 5 LEDs on
		case _1: LCD_DisplayString(1, "1"); break;
		case _2: LCD_DisplayString(1, "2"); break;
		case _3: LCD_DisplayString(1, "3"); break;
		case _4: LCD_DisplayString(1, "4"); break;
		case _5: LCD_DisplayString(1, "5"); break;
		case _6: LCD_DisplayString(1, "6"); break;
		case _7: LCD_DisplayString(1, "7"); break;
		case _8: LCD_DisplayString(1, "8"); break;
		case _9: LCD_DisplayString(1, "9"); break;
		case A: LCD_DisplayString(1, "A"); break;
		case B: LCD_DisplayString(1, "B"); break;
		case C: LCD_DisplayString(1, "C"); break;
		case D: LCD_DisplayString(1, "D"); break;
		case star: LCD_DisplayString(1, "*"); break;
		case _0: LCD_DisplayString(1, "0"); break;
		case hashtag: LCD_DisplayString(1, "#"); break;
		default: break; // Should never occur. Middle LED off.
	}
	return state;
}