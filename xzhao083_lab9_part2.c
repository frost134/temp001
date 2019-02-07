/*
 * xzhao083_lab9_part1.c
 *
 * Created: 2019/2/7/周四 11:39:28
 * Author : Zinxp
 */ 

#include <avr/io.h>
float note[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char scale = 0;
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}
enum States{sound_off, up_P, up_R, down_P, down_R, sound_on} state;
void Tick()
{

	switch(state)
	{
		case sound_off: if (!GetBit(PINA, 0)&&GetBit(PINA, 1)&&GetBit(PINA, 2)) {state = up_P; break;}
					else if (GetBit(PINA, 0)&&!GetBit(PINA, 1)&&GetBit(PINA, 2)) {state = down_P; break;}
					else if (GetBit(PINA, 0)&&GetBit(PINA, 1)&&!GetBit(PINA, 2)) {state = sound_on; break;}
					else state = sound_off;break;
		case up_P: if (GetBit(PINA, 0)) state = up_R;break;
		case down_P: if (GetBit(PINA, 1)) state = down_R;break;
		case sound_on: if (GetBit(PINA, 2)) state = sound_off;break;
		case up_R:
		case down_R: state = sound_off;break;
	}
	switch(state)
	{
		case sound_off: set_PWM(0);break;
		case up_R: if (scale < 7) {scale += 1;}break;
		case down_R: if (scale > 0) {scale -= 1;}break;
		case up_P:
		case down_P: break;
		case sound_on: set_PWM(note[scale]);break;
	}
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x08; PORTB = 0x00;
	PWM_on();
    while (1) 
    {
		Tick();
    }
}
