/*
 * receiver.c
 *
 * Created: 6/28/2014 4:54:34 PM
 *  Author: Daniel
 */ 
#define MSGLEN 8
#define F_CPU 8000000

#ifndef TESTING
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>
#include <avr/io.h>
#endif

#ifdef TESTING
#define PININTFN void pinInt();
#define CLKINTFN void clkInt();
#else
#define PININTFN ISR (PCINT0_vect)
#define CLKINTFN ISR (TIMER0_OVF_vect)
#endif

#define CLK 3
#define DAT 4

#define NUMCH 3

#define CH0 0
#define CH1 1 
#define CH2 2

#define BRIGHTMAX 255
#define SSHORTMAX 32767

#if CLK == 3 
#define PCINTCLK PCINT3
#else
#define PCINTCLK PCINT4
#endif

//global biz
volatile unsigned char msg = 0;
volatile char msgIdx = 0;
volatile unsigned char prog = 0;
volatile unsigned char pinClockLast;
volatile unsigned short brightness[NUMCH];
volatile unsigned char brightTime[NUMCH];
volatile signed short speeds[NUMCH];
volatile signed short TOC = 0;
volatile signed char gettingBrighter[NUMCH];
volatile signed short brightDelay[NUMCH];
volatile unsigned char argc;
volatile unsigned char mask = 0;
const unsigned char CNHL[3] = {CH0, CH1, CH2};
//end of ze globalz

//updaters


void noUp();

void (*updater) () = &noUp;
//end updaters

//updater implementations
void noUp() {
	_NOP();
}


//end updater implementations


int main(void)
{
	DDRB = _BV(CH0) | _BV(CH1) | _BV(CH2);
	CLKPR = _BV(CLKPCE);
	CLKPR = 0; //8 whole mhz
	wdt_disable(); //disables watchdog	
	GIMSK |= _BV(PCIE); //enabling pin change interrupts
	PCMSK |= _BV(PCINTCLK); //setting pin 1 to trigger interrupts
	pinClockLast = 0;
	
	TIMSK |= _BV(TOIE0); 
	TCCR0B |= 3;//setting the clock as some multiple of the global clock 

	sei();
    while(1)
    {
		updater();
    }
}


void readData() {
	unsigned char newBit = PINB & _BV(DAT);
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
	if (msg == 0) {
		updater = &noUp;
	}
}



void pinInt() {
	unsigned char pinClock = PINB & _BV(DAT);
	if (pinClock != pinClockLast && pinClock != 0) {
		readData();
		if (msgIdx == 8) {
			setProgram();
			clearMsg();
		}
	}
	pinClockLast = pinClock;
}

PININTFN {
	pinInt();
}


void setInitBrightnesses() {
	for (volatile unsigned char i=0; i<NUMCH; i++) {
		brightTime[i] = brightness[i] >> 8;
		if ((brightTime[i] != 0) && !(mask & _BV(CNHL[i]))) {
			PORTB |= _BV(CNHL[i]);
		} else {
			PORTB &= ~_BV(CNHL[i]);
		}
	}
}

CLKINTFN { // so 255 is on the whole time
	TOC++; 
	if (TOC == BRIGHTMAX) {
		TOC = 0;
		setInitBrightnesses();
	} else {
		for (unsigned char i=0; i<NUMCH; i++) {
			if (TOC == brightTime[i]) {
				PORTB &= ~_BV(CNHL[i]);
			}
		}
	}
		

}
