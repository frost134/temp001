/*	Partner(s) Name & E-mail:

 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com

 *  Shuang Wu & frost134@live.com

 *	Lab Section: 022 

 *	Assignment: Lab 6  Exercise 2

 *	Exercise Description: 

 *	I acknowledge all content contained herein, excluding

    template or example code, is my own original work.

 */
#include <avr/io.h>
#include <avr/interrupt.h>

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

void main()
{
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	TimerSet(300);
	TimerOn();
	unsigned char tmpB = 0x01;
	unsigned int flag = 0;
	while(1) {
		if (!flag){tmpB = tmpB << 1;}
		if (tmpB == 0x08){tmpB = 0x01;} 
		//if(flag){PORTB = tmpB; continue;}
		
		PORTB = tmpB;
		
		while (!TimerFlag);    // Wait 1 sec
		while(!(PINA&0x01)){TimerFlag = 1;};
		TimerFlag = 0;
		
	}
}


