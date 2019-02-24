/*	Partner(s) Name & E-mail:
 *  Xuanpeng Zhao & xuanpengzhao97@gmail.com
 *  Shuang Wu & frost134@live.com
 *	Lab Section: 022 
 *	Assignment: Lab 11  Exercise 2
 *	Exercise Description: 
 *	I acknowledge all content contained herein, excluding
    template or example code, is my own original work.
 */
#include <avr/io.h>
#include "io.c"
#include <timer.h>

int main(void)
{
	
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	TimerSet(500);
	TimerOn();
	LCD_init();
	unsigned char *string = "CS120B is Legend... wait for it DARY!";
	unsigned char lenth = strlen(string);
	unsigned char i = 33;
	unsigned char j = 1;
    while (1) 
    {
		if (i > 1)
		{
			LCD_DisplayString(i, string);
			i--;
		}
		else if (i == 1)
		{
			if (j <= lenth)
			{
				LCD_DisplayString(1, string);
				string++;
				j++;
			}
			else
			{
				string -= lenth;
				j = 1;
				i = 33;
			}
			
		}
		while(!TimerFlag){};
		TimerFlag = 0;
    }
}

