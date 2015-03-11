#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>
#include <avr/io.h>


int main() {

	DDRB = _BV(0);
	while (1) {
		if (PINB & _BV(1)) {
			PORTB = _BV(0);
		} else {
			PORTB = 0;
		}
	}
}

