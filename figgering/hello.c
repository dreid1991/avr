#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>

int main() {
	DDRB = _BV(0);
	while (1) {
		_delay_ms(200);
		PORTB = _BV(0);
		_delay_ms(200);
		PORTB = 0;
	}
}
//hello
