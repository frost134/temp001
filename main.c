/*
 * xzhao083_lab9_part1.c
 *
 * Created: 2019/2/7/周四 11:39:28
 * Author : Zinxp
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392.00
#define A5 440.00
#define B5 493.88
#define C5 523.25
#define D5 587.33
#define E5 659.25
#define F5 698.46
#define G5 783.99
#define A6 880
#define B6 987.77
#define C6 1046.50
#define D6 1174.66
#define E6 1318.51
#define F6 1396.91

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
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
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
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
unsigned char i;
unsigned char sequence = 0;
float note[] = {G5, A6, D5, C5, D5, C5, G5, A6, D5, C5, D5, C5, G5, A6, D5, C5, D5, C5, F5, E5, F5, E5, D5, C5,
				G5, A6, D5, C5, D5, C5, G5, A6, D5, C5, D5, C5, G5, A6, C6, F6, E6, F6, E6, D6, C6, A6,
				G5, A6, D5, C5, D5, C5, G5, A6, D5, C5, D5, C5, G5, A6, D5, C5, D5, C5, F5, E5, F5, E5, D5, C5,
				D5, F5, A6, C6, F6, E6, C6, A6, G5, F5, G5, A6, D5, D5, C5, D5};
unsigned char hold[] = {50, 50, 25, 25, 25, 25, 50, 50, 25, 25, 25, 25, 50, 50, 25, 25, 25, 25, 50, 25, 25, 25, 25, 50,
						50, 50, 25, 25, 25, 25, 50, 50, 25, 25, 25, 25, 50, 50, 50, 50, 25, 25, 25, 25, 50, 50,
						50, 50, 25, 25, 25, 25, 50, 50, 25, 25, 25, 25, 50, 50, 25, 25, 25, 25, 50, 25, 25, 25, 25, 50,
						25, 25, 25, 25, 25, 25, 25, 25, 50, 50, 50, 50, 50, 100, 50, 100};
unsigned char wait[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25};
unsigned char lenth = sizeof(note) / sizeof(note[0]);
enum States{sound_off, sound_on, wait_for_next} state;
void Tick()
{

	switch(state)
	{
		case sound_off: if (!GetBit(PINA, 0)) {sequence = 0;state = sound_on; break;}
		case sound_on: if (sequence >= lenth) {state = wait_for_next;} break;
		case wait_for_next: if (GetBit(PINA, 0)) state = sound_off;break;
	}
	switch(state)
	{
		case sound_off:
		case wait_for_next: set_PWM(0);break;
		case sound_on:  set_PWM(note[sequence]);
						for (i = 1; i <= hold[sequence]; i++)
						{
							while (!TimerFlag){};
							TimerFlag = 0;
						}
						set_PWM(0);
						for (i = 1; i <= wait[sequence]; i++)
							{
								while (!TimerFlag){};
								TimerFlag = 0;
							}
						sequence += 1;
						break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x08; PORTB = 0x00;
	PWM_on();
	TimerSet(5);
	TimerOn();
	state = sound_off;
    while (1) 
    {
		Tick();
    }
}