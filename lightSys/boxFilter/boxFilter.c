
#define F_CPU 1048576/256
#define NUMRD 60 
#define HALF NUMRD/2
#define IN 0
#define OUT 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>


volatile signed short timerOverflowCount = 0;

volatile unsigned char vals[NUMRD];


int main() {
	TIMSK |= _BV(TOIE0);  
	CLKPR = _BV(CLKPCE); //not changing thing.  not sure why not
	CLKPR = _BV(CLKPS1);
	TCCR0B = _BV(CS02);
	DDRB = _BV(OUT);
	GIMSK = _BV(PCIE);
	PCMSK = _BV(PCINT0);
	sei();		
	volatile unsigned char curIdx = 0;
	volatile unsigned char curVal = PINB & _BV(IN);
	volatile unsigned char i=0;
	for (i=0; i<NUMRD; i++) {
		vals[i] = curVal;
	}
	while (1) {
		vals[curIdx] = PINB & _BV(IN) ? 1 : 0;
		if (curIdx == NUMRD-1) {
			curIdx = 0;
		} else {
			curIdx++;
		}
		volatile unsigned char sum = 0;
		for (i=0; i<NUMRD; i++) {
			sum += vals[i];
		}
		if (sum >= HALF) {
			PORTB |= _BV(OUT);
		} else {
			PORTB &= ~_BV(OUT);
		}

	}
}

ISR (TIMER0_OVF_vect) {
	timerOverflowCount++;	
}

