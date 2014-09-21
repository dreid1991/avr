#define PCINT3 3
#define PCINT4 4

#define TESTING
#include <iostream>
using namespace std;
unsigned char DDRB = 0;
unsigned char CLKPR = 0;
unsigned char CLKPCE = 1;
unsigned char TIMSK = 0;
unsigned char TOIE0 = 0;

unsigned char PORTB = 0;
unsigned char PINB = 0;
unsigned char GIMSK = 0;
unsigned char PCMSK = 0;
unsigned char PCIE = 0;
unsigned char TCCR0B = 0;
unsigned char _BV(unsigned char x) {
	return 1 << x;
}
void sei() {

}

void wdt_disable() {

}


void _NOP() {

}

void helper() {
	
}
