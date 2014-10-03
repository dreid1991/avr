#define PCINT3 3
#define PCINT4 4
#include "globals.c"
#define TESTING
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
using namespace std;
unsigned char DDRB = 0;
unsigned char CLKPR = 0;
unsigned char CLKPCE = 1;
unsigned char TIMSK = 0;
unsigned char TOIE0 = 0;

unsigned char PORTB = 0;
unsigned char CS00 = 0;
unsigned char PINB = 0;
unsigned char GIMSK = 0;
unsigned char PCMSK = 0;
unsigned char PCIE = 0;
unsigned char TCCR0B = 0;
unsigned char _BV(unsigned char x) {
	return 1 << x;
}


//externs
extern volatile unsigned char msg;
extern void clkInt();

void sei() {

}

void wdt_disable() {

}


void _NOP() {

}

void helper() {
	msg = 2;
	setProgram();
	TOC = BRIGHTMAX - 1;
	updater.update();
	//cout << brightness[0] << ", " <<brightness[1] << ", " <<brightness[2] << endl;

	clkInt();

	//cout << (int) brightTime[0] << ", " << (int) brightTime[1] << ", " << (int) brightTime[2] << endl;
	for (unsigned int i=0; i<20000; i++) {
		if (rand() < RAND_MAX/2) {

			updater.update();
			cout << brightness[0] << endl;
		}
		clkInt();
	}
	/*
	cout << "ning" << endl;
	for (unsigned int i=0; i<255; i++) {

		clkInt();
	}
	*/
}

void _delay_ms(int x) {

}
