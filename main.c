/*
 * xzhao083_lab8_part4.c
 *
 * Created: 2019/2/6/周三 18:51:25
 * Author : Zinxp
 */ 

#include <avr/io.h>
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}


int main(void)
{
	ADC_init();
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	unsigned short adc = ADC;
	unsigned char tempB;
	tempB = 0x00;
	while (1)
	{
		adc = ADC;
		if (adc <= 125)
			tempB = 0x01;
		else if (adc <= 251)
			tempB = 0x03;
		else if (adc <= 376)
			tempB = 0x07;
		else if (adc <= 502)
			tempB = 0x0F;
		else if (adc <= 626)
			tempB = 0x1F;
		else if (adc <= 753)
			tempB = 0x3F;
		else if (adc <= 876)
			tempB = 0x7F;
		else
			tempB = 0xFF;
		PORTB = tempB;
	}
}

