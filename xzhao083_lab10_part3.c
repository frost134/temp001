/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 10  Exercise 3
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
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

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		else if (frequency > 31250) { OCR0A = 0x0000; }
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT0 = 0;
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	set_PWM(0);
}
void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

unsigned char B0,B4,B2,B3 = 0;

typedef struct task {
	int state; // Current state of the task
	unsigned long period; // Rate at which the task should tick
	unsigned long elapsedTime; // Time since task's previous tick
	int (*TickFct)(int); // Function to call for task's tick
} task;


task tasks[4];
const unsigned char tasksNum = 4;
const unsigned long tasksPeriodGCD = 100;
const unsigned long periodBlinkLED = 1000;
const unsigned long periodThreeLEDs = 300;

enum BL_States { BL_SMStart, BL_s1 };
int TickFct_BlinkLED(int state);

enum TL_States { TL_SMStart, TL_s1, TL_s2, TL_s3};
int TickFct_ThreeLEDs(int state);

enum Comb_States {C_SMStart, C_s1};
int TickFct_CombLEDs(int state);

enum Sound_States {S_SMstart, S_press, S_release};
int TickFct_SoundGenerate(int state);

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

int main() {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	unsigned char i=0;
	tasks[i].state = BL_SMStart;
	tasks[i].period = periodBlinkLED;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkLED;
	++i;
	tasks[i].state = TL_SMStart;
	tasks[i].period = periodThreeLEDs;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_ThreeLEDs;
	++i;
	tasks[i].state = C_SMStart;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_CombLEDs;
	++i;
	tasks[i].state = S_SMstart;
	tasks[i].period = tasksPeriodGCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_SoundGenerate;
	TimerSet(tasksPeriodGCD);
	TimerOn();
	PWM_on();
	while(1) {}
	return 0;
}
int TickFct_SoundGenerate(int state) {
	switch(state) {
		case S_SMstart:
		if (!GetBit(PINA, 2))state = S_press;
		
		break;
		case S_press:
		if (GetBit(PINA, 2))state = S_release;
		break;
		case S_release: state = S_SMstart;
		break;
		default:
		state = S_SMstart;
	}
	switch(state) {
		case S_press:
		set_PWM(250);
		break;
		case S_release:
		case S_SMstart:
		default:
		set_PWM(0);
		break;
	}
	return state;
}

int TickFct_BlinkLED(int state) {
	switch(state) { // Transitions
		case BL_SMStart: // Initial transition
		state = BL_s1;
		break;
		case BL_s1:
		state = BL_s1;
		break;
		default:
		state = BL_SMStart;
	} // Transitions

	switch(state) { // State actions
		case BL_s1:
		B0 = !B0;
		break;
		default:
		break;
	} // State actions
	return state;
}

int TickFct_ThreeLEDs(int state) {
	switch(state) { // Transitions
		case TL_SMStart: // Initial transition
		state = TL_s1;
		break;
		case TL_s1:
		state = TL_s2;
		break;
		case TL_s2:
		state = TL_s3;
		break;
		case TL_s3:
		state = TL_s1;
		break;
		default:
		state = TL_SMStart;
	} // Transitions

	switch(state) { // State actions
		case TL_s1:
		B2 = 1; B3 = 0; B4 = 0;
		break;
		case TL_s2:
		B2 = 0; B3 = 1; B4 = 0;
		break;
		case TL_s3:
		B2 = 0; B3 = 0; B4 = 1;
		break;
		default:
		break;
	} // State actions
	return state;
}

int TickFct_CombLEDs(int state)
{
	switch(state)
	{
		case C_SMStart: state = C_s1; break;
		case C_s1: state = C_s1; break;
	}
	switch(state)
	{
		case C_s1: PORTB = (B0<<4)+(B2<<5)+(B3<<6)+(B4<<7); break;
		default: break;
	}
	return state;
}