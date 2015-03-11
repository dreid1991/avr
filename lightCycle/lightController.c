

#define MSGLEN 8
#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>
#include <avr/io.h>

#define SRCLK 0
#define SER 1
#define RCLK 2

#define REGSIZE 8
#define NUMOUTS 5
int main(void)
{

	CLKPR = _BV(CLKPCE);
	CLKPR = 0; //8 whole mhz
	_delay_ms(100);
	DDRB = _BV(SRCLK) | _BV(SER) | _BV(RCLK);
	/*
	unsigned char msg = 255;
	for (char i=REGSIZE-1; i>=0; i--) {
		if (msg & _BV(i)) {
			PORTB |= _BV(SER);
		} else {
			PORTB &= ~_BV(SER);
		}
		_delay_ms(10);
		PORTB |= _BV(SRCLK);
		_delay_ms(10);
		PORTB &= ~_BV(SRCLK);
		_delay_ms(50);
	
	}
	PORTB |= _BV(SER);
	for (int i=0; i<8; i++) {
		PORTB |= _BV(SRCLK);
		_delay_ms(10);
		PORTB &= ~_BV(SRCLK);
		_delay_ms(10);
	}
	PORTB = 0;
	PORTB |= _BV(RCLK);
	_delay_ms(10);
	PORTB = 0;
	*/
	char curOn = 0;
	while(1) {
		unsigned char msg = _BV(curOn);
		for (char i=REGSIZE-1; i>=0; i--) {
			if (msg & _BV(i)) {
				PORTB |= _BV(SER);
			} else {
				PORTB &= ~_BV(SER);
			}
			_delay_ms(10);
			PORTB |= _BV(SRCLK);
			_delay_ms(10);
			PORTB &= ~_BV(SRCLK);
			//_delay_ms(50);

		}
		PORTB = 0;
		PORTB |= _BV(RCLK);
		_delay_ms(10);
		PORTB = 0;
		curOn++;
		if (curOn == NUMOUTS) {
			curOn = 0;
		}
	};	
}


