/*
 * GccApplication3.c
 *
 * Created: 12/29/2013 10:18:47 PM
 *  Author: Daniel
 */ 
// http://brownsofa.org/blog/2011/01/04/the-compleat-attiny13-led-flasher-part-2-using-timer-interrupts/	
#define F_CPU 1000000
#define MSGLEN 8
#define TWITCHTHRESH 5
#define DASHTHRESH 25
#define SENDTIMESCALE 1
#define CLK 1
#define DAT 0

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>
/*
0 -> data
1 -> clock
3 -> sense
4 -> LED

*/
//2 -> R
//1 -> G
//0 -> B
//3 -> morse
//4 -> shift
volatile char pin3Last = 0;
volatile char recording = 0;
volatile char isDown = 0;
volatile signed short timeBeginCur = 0; //THIS
volatile signed short timerOverflowCount = 0; //AND THIS MUST BE SIGNED.  IF NOT, IT MESSES UP timerOverflowCount - timeBeginCur > ###.  I do not understand

volatile unsigned char msgIdx = 0;

volatile signed short downTimes[MSGLEN + 1];
volatile signed short upTimes[MSGLEN];


void delay(short a) {
	for (short i=0; i<a; i++) {
		_NOP();
	}
}



void resetTimes() {
	for (unsigned char i=0; i<MSGLEN; i++) {
		downTimes[i] = 0;
		upTimes[i] = 0;
	}
	downTimes[MSGLEN] = 0;
	msgIdx = 0;
}




void recordCharacter(signed short elapsed) {
	
	if (!isDown) {
		downTimes[msgIdx] = elapsed;

	} else {
		upTimes[msgIdx] = elapsed;	
	}
}

void spewMsg(char leMsg) {
//wrapping this function in sei, cli, seems to turn the watchdog timer back on.  go figure.

	PORTB |= _BV(4);
	_delay_ms(100);
	PORTB &= ~_BV(4);
	_delay_ms(100);
	PORTB |= _BV(4);
	_delay_ms(100);
	PORTB &= ~_BV(4);
	_delay_ms(100);
	PORTB |= _BV(4);
	_delay_ms(100);
	PORTB &= ~_BV(4);
	_delay_ms(400);

	for (signed char i=7; i>=0; i--) {
		PORTB |= _BV(4);
		if (leMsg & (1 << i)) {
			_delay_ms(1300); 
			
			} else {
			_delay_ms(500);
		}
		PORTB &= ~_BV(4);
		_delay_ms(500);
		
	}


}



void sendMsg(unsigned char msg) {
	for (unsigned char i=0; i<MSGLEN; i++) {
		unsigned char bit = msg & (1 << i);
		if (bit) {
			PORTB |= _BV(DAT);
		} else {
			PORTB &= ~_BV(DAT);
		}
		_delay_us(50);
		PORTB |= _BV(CLK);
		_delay_ms(SENDTIMESCALE * 2);
		PORTB &= ~(_BV(CLK) | _BV(DAT));
		_delay_ms(SENDTIMESCALE);
	}
}

void registerEndMsg() {
	//msg condensed by the time we get here
	recording = 0;
	
	volatile unsigned char msg = 0;
	for (signed char i=0; i<MSGLEN; i++) {
		if (downTimes[i] > DASHTHRESH) {
			msg |= 1 << (MSGLEN - 1 - i);
			
		}
		
	}
	volatile unsigned char totalDashs = 0;
	volatile unsigned char totalDots = 0;
	for (signed char i=0; i<MSGLEN; i++) {
		if (downTimes[i] > DASHTHRESH) {
			totalDashs++;	
		} else {
			totalDots++;
		}
		
	}

	recording = 0;

	//spewMsg(totalDashs);
	//spewMsg(totalDots);
	spewMsg(msg);
	_delay_ms(2000);
	sendMsg(msg);
	
}

void condenseMsg() {
	for (signed char i=msgIdx; i>=0; i--) {
		if (upTimes[i] < TWITCHTHRESH) { //must allow it to check if uptime == 0 b/c a really fast signal flip will register as zero uptime.  WAS SOURCE OF SNEAKY PROBLEM
			downTimes[i] += downTimes[i+1];
			
			for (signed char j=i+1; j<MSGLEN; j++) {
				downTimes[j] = downTimes[j+1];
				upTimes[j-1] = upTimes[j];	
				
			}
			upTimes[MSGLEN-1] = 0;
			downTimes[MSGLEN] = 0;
			msgIdx--;
		}
		
	}	
	
}


char registerPinChange() {
	if (!recording && isDown) {
		recording = 1;
		
	} else if (recording) {

		recordCharacter(timerOverflowCount);
		if (isDown) {
			msgIdx ++; //a down/up pair is complete
		}
		if (!isDown && msgIdx == MSGLEN) {
			condenseMsg();
		}
		if (msgIdx == MSGLEN) {
			//force finish and stop recording
			
		}
	}

	if (isDown) {
		PORTB |= _BV(4);
		
	} else {
		PORTB &= ~_BV(4);

	}
	
	return isDown;
}







int main(void)
{
	resetTimes();
	wdt_disable(); //disables watchdog

	DDRB = _BV(0) | _BV(1) | _BV(4);

	TIMSK |= 1<<TOIE0; //unmasks timer 0


	TCCR0B |= 3;//setting the clock as some multiple of the global clock thingy. required



	GIMSK |= 1<<PCIE; //enabling pin change interrupts
	PCMSK |= 1<<PCINT3; //setting pin 3 to trigger interrupts
	//volatile char fired = 0;
	pin3Last = PINB & _BV(3);
	
	sei(); //enables global interrupts
	while(1)
	{
		/*
		if (!fired) {
			for (unsigned char i=0; i<3; i++) {
				PORTB |= _BV(2);
				_delay_ms(1000);
				PORTB &= ~_BV(2);
				_delay_ms(1000);
			}
			for (unsigned char i=0; i<3; i++) {
				PORTB |= _BV(2);
				_delay_ms(600);
				PORTB &= ~_BV(2);
				_delay_ms(40);
			}
			for (unsigned char i=0; i<1; i++) {
				PORTB |= _BV(2);
				_delay_ms(1000);
				PORTB &= ~_BV(2);
				_delay_ms(1000);
			}
	
			fired = 1;	
		}
		*/
		
	}
}





ISR (TIMER0_OVF_vect) {
	timerOverflowCount++; 
	//can probably make timerOverflowCount a short
	if (recording && !isDown && timerOverflowCount > 125) { //let sit for short time, then register last. ALSO tick = 16 ms as 1 MHz
		condenseMsg();
		registerEndMsg();
		//need to reset junk

	}
		//mebe make it sleep if it's just hanging around and no program is running
		

}
void pinInt() {
	cli(); //cli, sei does not affect behavior of morse interpreter, at least when nothing else is going on
	
	char pin3Val = PINB & _BV(3);
	if (pin3Val != pin3Last) {
		isDown = pin3Val;
		registerPinChange();
		timerOverflowCount = 0;
		pin3Last = pin3Val;

	}
	sei();	
	
}

ISR (PCINT0_vect) {
	pinInt();
	while(pin3Last != (PINB & _BV(3))) {
		pinInt();	
		
	}
}
