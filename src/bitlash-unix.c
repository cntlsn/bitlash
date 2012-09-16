/*
	bitlash-unix.c: A minimal implementation of certain core Arduino functions	
	
	The author can be reached at: bill@bitlash.net

	Copyright (C) 2008-2012 Bill Roy

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:
	
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/
#include "bitlash.h"

/*
issues

doesn't receive console input

auto detect gcc for build, set unix_build flag

sizeof(numvar) is 8!

serialAvailable
serialRead
serialWrite
*/

#if _POSIX_TIMERS	// not on the Mac, unfortunately
struct timespec startup_time, current_time, elapsed_time;

// from http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
struct timespec time_diff(timespec start, timespec end) {
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

unsigned long millis(void) {
	if ((startup_time.tv_sec == 0) && (startup_time.tv_nsec == 0)) {
		clock_gettime(CLOCK_REALTIME, &startup_time);
		return 0;
	}
	clock_gettime(CLOCK_REALTIME, &current_time);	
	elapsed_time = time_diff(startup_time, current_time);
	return (elapsed_time.tv_sec * 1000UL) + (elapsed_time.tv_nsec / 1000000UL);
}
#else
#include <sys/time.h>

unsigned long startup_millis, current_millis, elapsed_millis;
struct timeval startup_time, current_time;

// after http://laclefyoshi.blogspot.com/2011/05/getting-nanoseconds-in-c-on-freebsd.html
unsigned long millis(void) {
	if (startup_millis == 0) {
		gettimeofday(&startup_time, NULL);
		startup_millis = (startup_time.tv_sec * 1000) + (startup_time.tv_usec /1000);
		return 0UL;
	}
	gettimeofday(&current_time, NULL);
	current_millis = (current_time.tv_sec * 1000) + (current_time.tv_usec / 1000);
	elapsed_millis = current_millis - startup_millis;
	return elapsed_millis;
}

#endif


#include "conio.h"

int serialAvailable(void) { return 1; }
int serialRead(void) { return getch(); }
void spb (char c) { 
	putchar(c);
	//printf("%c", c);
	fflush(stdout);
}
void sp(const char *str) { while (*str) spb(*str++); }
void speol(void) { spb(13); spb(10); }

numvar setBaud(numvar pin, unumvar baud) { return 0; }

// stubs for the hardware IO functions
//
unsigned long pins;
void pinMode(byte pin, byte mode) { ; }
int digitalRead(byte pin) { return ((pins & (1<<pin)) != 0); }
void digitalWrite(byte pin, byte value) {
	if (value) pins |= 1<<pin;
	else pins &= ~(1<<pin);
}
int analogRead(byte pin) { return 0; }
void analogWrite(byte pin, int value) { ; }
int pulseIn(int pin, int mode, int duration) { return 0; }

// stubs for the time functions
//
void delay(unsigned long ms) {
	unsigned long start = millis();
	while (millis() - start < ms) { ; }
}
void delayMicroseconds(unsigned int us) {;}

// fake eeprom
byte fake_eeprom[E2END];
void init_fake_eeprom(void) {
int i=0;
	while (i < E2END) eewrite(i++, 0xff);
}
byte eeread(int addr) { return fake_eeprom[addr]; }
void eewrite(int addr, byte value) { fake_eeprom[addr] = value; }


int main () {
	init_fake_eeprom();
	initBitlash(0);
	for (;;) runBitlash();
}
