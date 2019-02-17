/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 10  Exercise 5
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


typedef struct task {
	int state; // Current state of the task
	unsigned long period; // Rate at which the task should tick
	unsigned long elapsedTime; // Time since task's previous tick
	int (*TickFct)(int); // Function to call for task's tick
} task;

task tasks[2];

const unsigned char tasksNum = 2;
const unsigned long tasksPeriodGCD = 100;
const unsigned long periodContinue = 200;

enum CNT_States {CNT_Start, CNT_press};
int TickFct_Counting(int state);

enum CON_States {CON_Start, CON_press};
int TickFct_Continue(int state);

unsigned long holding_time;
unsigned char counter;
void TimerISR() {
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
int main() {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	unsigned char i=0;
	counter = 0;
	tasks[i].state = CNT_Start;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Counting;
	++i;
	tasks[i].state = CON_Start;
	tasks[i].period = periodContinue;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_Continue;
	++i;
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	while(1) {
		
	}
	return 0;
}
int TickFct_Continue(int state)
{
	switch(state)
	{
		case CON_Start:holding_time = 0;
		case CON_press:
		if (!GetBit(PINA, 0) && !GetBit(PINA, 1))
		{
			state = CON_Start;
			counter = 0;
		}	
		else if (GetBit(PINA, 0) && GetBit(PINA, 1)) state = CON_Start;
		else state = CON_press;
		break;
	}
	switch(state)
	{
		case CON_press:
		if (holding_time <= 3000)
		{
			if (!GetBit(PINA, 0) && GetBit(PINA, 1))
			{
				holding_time += periodContinue;
				if (holding_time % 1000 == 0)
				{
					if (counter == 9) counter = 8;
					counter += 1;
				}
			}
			if (GetBit(PINA, 0) && !GetBit(PINA, 1))
			{
				holding_time += periodContinue;
				if (holding_time % 1000 == 0)
				{
					if (counter == 0) counter = 1;
					counter -= 1;
				}
			}
		}
		else if (holding_time > 3000)
		{
			if (!GetBit(PINA, 0) && GetBit(PINA, 1))
			{
				holding_time += periodContinue;
				if (holding_time % 400 == 0)
				{
					if (counter == 9) counter = 8;
					counter += 1;
				}
			}
			if (GetBit(PINA, 0) && !GetBit(PINA, 1))
			{
				holding_time += periodContinue;
				if (holding_time % 400 == 0)
				{
					if (counter == 0) counter = 1;
					counter -= 1;
				}
			}
		}
		
		
		
		case CON_Start:PORTB = counter;break;
	}
	return state;
}

int TickFct_Counting(int state)
{
	switch(state)
	{
		case CNT_Start: if (!GetBit(PINA, 0) && GetBit(PINA, 1))
		{
			if (counter == 9) counter = 8;
			counter += 1;
			state = CNT_press;
			}
		else if (GetBit(PINA, 0) && !GetBit(PINA, 1))
		{
			if (counter == 0) counter = 1;
			counter -= 1;
			state = CNT_press;
			}
		else if (!GetBit(PINA, 0) && !GetBit(PINA, 1))
		{
			counter = 0;
			state = CNT_Start;
		}
		break;
		case CNT_press:if (GetBit(PINA, 0) && GetBit(PINA, 1)) state = CNT_Start;
	}
	switch(state)
	{
		case CNT_Start:
		case CNT_press:	PORTB = counter;break;
	}
	return state;
}

