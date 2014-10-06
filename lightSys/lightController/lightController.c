/*
 * receiver.c
 *
 * Created: 6/28/2014 4:54:34 PM
 *  Author: Daniel
 */ 
#define MSGLEN 8
#define F_CPU 8000000
//#include "helpers.cpp"

#ifndef TESTING
#include "globals.c"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>
#include <avr/io.h>
#endif

#ifdef TESTING
#define PININTFN void pinIntWrap()
#define CLKINTFN void clkInt()
#else
#define PININTFN ISR (PCINT0_vect)
#define CLKINTFN ISR (TIMER0_OVF_vect)
#endif



//global biz
/*
volatile unsigned char msg = 0;
volatile char msgIdx = 0;
volatile unsigned char prog = 0;
volatile unsigned char pinClockLast;
volatile unsigned short brightness[NUMCH];
volatile unsigned char brightTime[NUMCH];
volatile signed short speed[NUMCH];
volatile signed short TOC = 0;
volatile signed char gettingBrighter[NUMCH];
volatile signed short brightDelay[NUMCH];
volatile unsigned char argc;
volatile unsigned char mask = 0;
const unsigned char CHNL[3] = {CH0, CH1, CH2};
*/
//end of ze globalz



//updaters

void noUp();
void test();
void pulse();
//end updaters

//updaterInits
void noUpInit();
void testInit();
void pulseInit();
//end updaterInits

//updater implementations
void noUp() {
	_NOP();
}

void test() {
	if (brightness[0] == SHORTMAX) {
		speed[0] = -1;
	} else if (brightness[0] == 0) {
		speed[0] = 1;
	}
	brightness[0] += speed[0];
		

}

void pulse() {
	float rnd = myRand();
	speed[0] += rnd - .5;	
	if (rnd < 0.01) {
		speed[0] *= 1.1;
	}

	if (absFloat(speed[0]) > SCHARMAX) {
		speed[0] *= 0.5;
	}
	signed char dBright = myRound(speed[0]);
	unsigned short foo = brightness[0];	
	signed char bndRes = boundShort(&foo, &dBright);
	brightness[0] = foo;
	if (bndRes != 0) {
		speed[0] = -speed[0];
	}
	/*	
	unsigned short distFromMax = SHORTMAX - brightness[0];
	distFromMax >> 12;
	*/
	//brightness[0] += distFromMax;
	
	

/*
	if (brightness[0] < 40000) {
		rnd += 35;
	} else if (brightness[0] < 50000) {
		rnd += 15;
	}
	speed[0] += rnd * ((absChar(rnd) & 1) + 1) * ((((absChar(rnd) & 8) >> 3) + 1) << 5);

	volatile signed long trialBright = brightness[0];
	trialBright += speed[0] >> 7;
	*/
}


void flash() {
	if (brightness[0] == 0) {
		brightness[0] = 0xffff;
	} else {
		brightness[0] = 0x0;
	}
	_delay_ms(1000);

}
//end updater implementations

//init implementations
void noUpInit() {

}

void testInit() {
//	brightness[0] = 0x8fff;
	brightness[0] = 0x0000;
	speed[0] = 1;
	brightness[1] = 0x4fff;
	brightness[2] = 0x0ff0;
	mask |= _BV(CH1);
}

void pulseInit() {
	brightness[0] = myHash(offsetTOC());
	speed[0] = 0;
}


void flashInit() {
	brightness[0] = 0x0000;
	brightness[1] = 0x0000;
	brightness[2] = 0x0000;

}
//end init implementations

int main(void)
{
	updaters[NOUP_IDX] = UPDATERINIT(noUp);
	updaters[TEST_IDX] = UPDATERINIT(test);
	updaters[PULSE_IDX] = UPDATERINIT(pulse);
	updaters[FLASH_IDX] = UPDATERINIT(flash);
	updater = updaters[NOUP_IDX];

	DDRB = _BV(CH0) | _BV(CH1) | _BV(CH2);
	CLKPR = _BV(CLKPCE);
	CLKPR = 0; //8 whole mhz
	wdt_disable(); //disables watchdog	
	//UNCOMMENT THESE
//	GIMSK |= _BV(PCIE); //enabling pin change interrupts
//	PCMSK |= _BV(PCINTCLK); //setting pin 1 to trigger interrupts
	pinClockLast = 0;
	
	TIMSK |= _BV(TOIE0); 

	TCCR0B = _BV(CS00);//setting the clock as some multiple of the global clock 
	msg = 2;	
	setProgram();
	msg = 0;
	sei();
#ifndef TESTING
    while(1)
    {
		if (newProgram) {
			updater.init();
			TOC = BRIGHTMAX - 1; //so set brightness is called first time through
			newProgram = 0;
		}
		updater.update();
    }
#else
	helper();
#endif
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
		updater = updaters[NOUP_IDX];
	} else if (msg == 1) {
		updater = updaters[TEST_IDX];
	} else if (msg == 2) {
		updater = updaters[PULSE_IDX];
	} else if (msg == 3) {
		updater = updaters[FLASH_IDX];
	}
	newProgram = 1;

}

void pinInt() {
	unsigned char pinClock = PINB & _BV(DAT);
	if (pinClock != pinClockLast && pinClock != 0) {
		readData();
		if (msgIdx == 8) {
			setProgram();
			TOC = BRIGHTMAX - 1;
			clearMsg();
		}
	}
	pinClockLast = pinClock;
}

PININTFN {
	pinInt();
}


void setCycleBrightnesses() {
	//cout <<"mask is " <<  (int) mask << endl;
	for (volatile unsigned char i=0; i<NUMCH; i++) {
		brightTime[i] = brightness[i] >> 8;
		if ((brightTime[i] != 0) && !(mask & _BV(CHNL[i]))) {
			PORTB |= _BV(CHNL[i]);
		} else {
			PORTB &= ~_BV(CHNL[i]);
		}
	}
}

CLKINTFN { // so 255 is on the whole time
	TOC++; 
	if (TOC == BRIGHTMAX) {
		TOC = 0;
		setCycleBrightnesses();
	//	PORTB |= _BV(CH0);
	}
//	if (TOC == 1) {
//		PORTB &= ~_BV(CH0);
//	}
	for (unsigned char i=0; i<NUMCH; i++) {
		if (TOC == brightTime[i]) {
			PORTB &= ~_BV(CHNL[i]);
		}
//		cout << (bool) (PORTB & _BV(CHNL[i])) << ", ";
	}
//	cout << endl;

}
