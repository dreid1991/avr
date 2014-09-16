/*
 * receiver.c
 *
 * Created: 6/28/2014 4:54:34 PM
 *  Author: Daniel
 */ 
#define MSGLEN 8
#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>
volatile unsigned char msg = 0;
volatile char msgIdx = 0;
volatile unsigned char pinClockLast;
#include <avr/io.h>
/*
0->data
1->clock
*/
int main(void)
{
	DDRB = _BV(2);
	CLKPR = CLKPCE;
	CLKPR = CLKPS0;
	//wdt_disable(); //disables watchdog	
	GIMSK |= 1<<PCIE; //enabling pin change interrupts
	PCMSK |= 1<<PCINT1; //setting pin 1 to trigger interrupts
	pinClockLast = 0;
	sei();
    while(1)
    {

    }
}

void spewMsg(unsigned char msg) {
	//wrapping this function in sei, cli, seems to turn the watchdog timer back on.  There is no god.
	PORTB |= _BV(2);
	_delay_ms(100);
	PORTB &= ~_BV(2);
	_delay_ms(100);
	PORTB |= _BV(2);
	_delay_ms(100);
	PORTB &= ~_BV(2);
	_delay_ms(100);
	PORTB |= _BV(2);
	_delay_ms(100);
	PORTB &= ~_BV(2);
	_delay_ms(1000);
	for (signed char i=7; i>=0; i--) {
		PORTB |= _BV(2);
		if (msg & (1 << (unsigned char ) i)) {
			_delay_ms(1300);
			
		} else {
			_delay_ms(500);
		}
		PORTB &= ~_BV(2);
		_delay_ms(500);
		
	}
}


void readData() {
	unsigned char newBit = PINB & _BV(0);
	if (newBit) {
		msg |= 1 << msgIdx;
	}
	msgIdx ++;
}

void clearMsg() {
	msg = 0;
	msgIdx = 0;
}

void setProgram() {
	spewMsg(msg);
}



void interrupt() {
	unsigned char pinClock = PINB & _BV(1);
	if (pinClock != pinClockLast && pinClock != 0) {
		readData();
		if (msgIdx == 8) {
			setProgram();
			clearMsg();
		}
	}
	pinClockLast = pinClock;
}


ISR (PCINT0_vect) {
	//PORTB |= _BV(2);
	interrupt();
}