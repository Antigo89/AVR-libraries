/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 25.09.2023
Version: 0.8 alfa
Timers/Counters for starting functions or periodic measurements.
From 61Hz and above.
Added function auto calibration coefficient divinity (N).
Mode CTC (4)

Call function or measurements place in Interrupt:
ISR(TIMER0_COMPA_vect){...}

NOTE:	Libraries involved: <avr/interrupt>
		sei(); in main.c
*/

#ifndef TIMER0CTC_H
#define TIMER0CTC_H

//Timer/Counter 0 (8x)
//Hz in freq
void timer0Init_freq(uint16_t freq){
	uint16_t div;
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	if((F_CPU/(1024*freq))-1 > 1){
		div = 1024;
		TCCR0B = 0x05;
	}else if((F_CPU/(256*freq))-1 > 1){
		div = 256;
		TCCR0B = 0x04;
	}else if((F_CPU/(64*freq))-1 > 1){
		div = 64;
		TCCR0B = 0x03;
	}else if((F_CPU/(8*freq))-1 > 1){
		div = 8;
		TCCR0B = 0x02;
	}else{
		div = 1;
		TCCR0B = 0x01;
	}
	uint8_t timediv = (F_CPU/(div*freq))-1;
	OCR0A = timediv&0xFF;
	TCCR0A |= (1<<WGM01);
	TIMSK0 |= (1<<OCIE0A);
}

#endif
