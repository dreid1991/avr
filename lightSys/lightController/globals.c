#ifndef GLOBALS
#define GLOBALS

#define CLK 3
#define DAT 4

#define NUMCH 3

#define CH0 0
#define CH1 1 
#define CH2 2

#define BRIGHTMAX 255
#define SSHORTMAX 32767
#define SHORTMAX 65535

#if CLK == 3 
#define PCINTCLK PCINT3
#else
#define PCINTCLK PCINT4
#endif

#define UPDATERINIT(base) updater_package_init(&base, &base ## Init)

#define NUMUPS 4

#define NOUP_IDX 0
#define TEST_IDX 1
#define PULSE_IDX 2 
#define FLASH_IDX 3
struct updater_package {
	void (*update) ();
	void (*init) ();
};

//global biz
volatile unsigned char msg = 0;
volatile char msgIdx = 0;
volatile unsigned char prog = 0;
volatile unsigned char pinClockLast;
volatile unsigned char newProgram = 0;

volatile unsigned short brightness[NUMCH];
volatile unsigned char brightTime[NUMCH];
volatile signed short speeds[NUMCH];
volatile signed short TOC = BRIGHTMAX;
volatile signed char gettingBrighter[NUMCH];
volatile signed short brightDelay[NUMCH];
volatile unsigned char argc;
volatile unsigned char mask = 0;
const unsigned char CHNL[3] = {CH0, CH1, CH2};

//end of ze globalz
//
//
void setProgram();
struct updater_package updater;

struct updater_package updaters[NUMUPS];

struct updater_package updater_package_init(void (*updater) (), void (*updaterInit) ()) {
	struct updater_package u;
	u.update = updater;
	u.init = updaterInit;
	return u;
}



signed char hash(signed char x) {
	signed char y = 0;
	//avg over char's range is -4
	for (unsigned char i=0; i<8; i++) {
		char digit = (x >> i) & 1;
		y = digit + (y << 2) + (y << 4) - y;
	}
	return y;
}

signed char offsetTOC() {
	signed short x = TOC;
	return TOC - 128;
}


#endif
