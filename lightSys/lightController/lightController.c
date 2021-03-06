
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

struct updater_package updater;

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

}

void pulse() {
	const signed char bitesize = 30;
	for (unsigned char i=0; i<NUMCH; i++) {
		float rnd = myRand();
		speed[i] += 20*(rnd - .5);	
		if (rnd < 0.01) {
			speed[i] *= 1.1;
		} else { 
			speed[i] = speed[i] * 0.99;
		}
		if (rnd < 0.5 && rnd > 0.4993) { //every 10000 or so, add bias of up to 10000
			buffA[i] = myRand() * 60000 - 30000;
		}
		if (rnd < 0.6 && rnd > 0.599) {
			speed[i] = -speed[i];
		}
			
		
		if (absFloat(speed[i]) > SCHARMAX) {
			speed[i] *= 0.5;
		}
		signed char dBright = myRound(speed[i]);
		if (buffA[i] != 0 && dBright <= SCHARMAX-bitesize && dBright >= -SCHARMAX+bitesize) {
			if (buffA[i] < 0) {
				dBright-=10;
				buffA[i]+=10;
			} else {
				dBright+=10;
				buffA[i]-=10;
			}
			if (absFloat(buffA[i]) < 10) {
				buffA[i] = 0;
			}
		}
		signed char bndRes = boundShort(&brightness[i], &dBright);
		if (bndRes != 0) {
			speed[i] = -.5*speed[i];
		}
	}

}


void sweep() {
	for (unsigned char i=0; i<NUMCH; i++) {
		buffA[i] += speed[i];
		signed char dBright = buffA[i];
		buffA[i] -= dBright;
		signed char bndRes = boundShort(&brightness[i], &dBright);
		if (bndRes != 0) {
			speed[i] = -speed[i];
		}
	}
}

void pulseSweep() {
	for (unsigned char i=0; i<NUMCH; i++) {
		buffA[i] += speed[i];
		signed char dBright = buffA[i];
		buffA[i] -= dBright;
		signed char bndRes = boundShort(&brightness[i], &dBright);
		if (bndRes != 0) {
			speed[i] = -speed[i];
		}
	}
}

//end updater implementations

//init implementations
void noUpInit() {

}

void testInit() {
	brightness[0] = SHORTMAX;
	brightness[1] = SHORTMAX;
	brightness[2] = SHORTMAX;
}
void pulseInit() {
	brightness[0] = 0;//myHash(offsetTOC());
}


void sweepInit() {
	for (unsigned int i=0; i<NUMCH; i++) {
		speed[i] = 2 * myRand() + 0.1;
	}
}

void pulseSweepInit() {
	for (unsigned int i=0; i<NUMCH; i++) {
		speed[i] = 2 * myRand() + 0.1;
	}
}

//end init implementations

int main(void)
{
	updaters[NOUP_IDX] = UPDATERINIT(noUp);
	updaters[TEST_IDX] = UPDATERINIT(test);
	updaters[PULSE_IDX] = UPDATERINIT(pulse);
	updaters[SWEEP_IDX] = UPDATERINIT(sweep);
	updaters[PULSESWEEP_IDX] = UPDATERINIT(pulseSweep);

	updater = updaters[PULSE_IDX];
	updater.update();
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
	//msg = 2;	
	//setProgram();
	msg = 0;
	sei();
#ifndef TESTING
    while(1)
    {
		if (newProgram) {
			TOC = BRIGHTMAX - 1; //so set brightness is called first time through
			for (unsigned char i=0; i<BUFFSIZE; i++) {
				buffA[i] = 0;
				speed[i] = 0;
				brightness[i] = 0;
			}
			updater.init();
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
		updater = updaters[SWEEP_IDX];
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
