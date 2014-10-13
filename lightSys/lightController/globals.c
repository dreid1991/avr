#ifndef GLOBALS
#define GLOBALS

#define CLK 3
#define DAT 4

#define NUMCH 3

#define CH0 0
#define CH1 1 
#define CH2 2

#define SCHARMAX 127
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
#define SWEEP_IDX 3
#define PULSESWEEP_IDX 4 

#define BUFFSIZE 3
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
volatile float speed[NUMCH];
volatile signed short TOC = BRIGHTMAX;
volatile signed char gettingBrighter[NUMCH];
volatile signed short brightDelay[NUMCH];
volatile unsigned char argc;
volatile unsigned char mask = 0;
const unsigned char CHNL[3] = {CH0, CH1, CH2};

volatile float buffA[3];
volatile float buffB[3];
unsigned long rndseed = 1000;

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



float myRand() {
	rndseed = (16807*rndseed) % 2147483647; 
	return (float) rndseed / (float) 2147483711;
}

signed char myHash(signed char x) {
	signed char y = 0;
	//avg over char's range is -4
	for (unsigned char i=0; i<7; i++) {  //HEY - MAKE IT 8 AGAIN?
		char digit = (x >> i) & 1;
		y = digit + (y << 2) + (y << 4) - y;
	}
	return y;
}

signed char offsetTOC() {
	signed short x = TOC;
	return TOC - 127;
}

signed short abs(signed short x) {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

signed char absChar(signed char x) {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

float absFloat(float x) {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
	
}

signed char myRound(float x) {
	x += 0.5;
	signed short y = x;
	return y;
}

signed char boundSignedShort(volatile signed short *x, signed char *dx) {
	signed long trial = *x;
	trial += *dx;
	if (trial > SSHORTMAX) {
		*x = SSHORTMAX;
		return 1;
	} else if (trial < -SSHORTMAX) {
		*x = -SSHORTMAX;
		return -1;
	}
	*x += *dx;
	return 0;
}
signed char boundShort(volatile unsigned short *x, signed char *dx) {
	signed long trial = *x;
	trial += *dx;
	if (trial > SHORTMAX) {
		*x = SHORTMAX;
		return 1;
	} else if (trial < 0) {
		*x = 0;
		return -1;
	}
	*x = trial;
	return 0;
}



#endif
