#define PCINT3 3
#define PCINT4 4

#define TESTING

unsigned char DDRB = 0;
unsigned char CLKPR = 0;
unsigned char CLKPCE = 1;
unsigned char TIMSK = 0;
unsigned char TOIE0 = 0;

unsigned char PORTB = 0;
unsigned char PINB = 0;

unsigned char _BV(unsigned char x) {
	return 1 << x;
}
void sei() {

}
