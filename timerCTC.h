/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 07.07.2021
Version: 0.9
Timer/Counter1 for starting functions or periodic measurements.
Range of work frequency: 245/N Hz - 8/N MHz.
Range of work time: 4*N ms - 0,000125*N ms
Mode CTC (4), clk/256

Libraries involved: <avr/interrupt>

Call function or measurements place in Interrupt:
ISR(TIMER1_COMPA_vect){...}

NOTE: sei(); in main.c
*/
//#ifndef F_CPU
//#define F_CPU		16000000UL
//#endif
#ifndef TIMERCTC_H
#define TIMERCTC_H
//#define TIMEDIVF			(F_CPU/(256*FREQ))-1
//#define TIMEDIVS(TIME)	(int)(F_CPU*TIME/256)-1

//Hz in freq
void timerInitF(uint16_t freq){
	uint16_t timediv = (F_CPU/(256*freq))-1;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	OCR1AH = (timediv>>8);
	OCR1AL = timediv&0xFF;
	TCCR1B |= (1<<WGM12)|(1<<CS12);
	TIMSK1 |= (1<<OCIE1A);
}
//ms in time
void timerInitS(uint16_t time){
	uint16_t timediv = (int)(F_CPU*time/256000)-1;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	OCR1AH = (timediv>>8);
	OCR1AL = timediv&0xFF;
	TCCR1B |= (1<<WGM12)|(1<<CS12);
	TIMSK1 |= (1<<OCIE1A);
}
#endif
