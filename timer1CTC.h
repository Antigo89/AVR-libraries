/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 25.09.2023
Version: 1.0
Timers/Counters for starting functions or periodic measurements.
Range of work frequency: 245/N Hz - 8/N MHz.
Range of work time: 4*N ms - 0,000125*N ms
Added function auto calibration coefficient divinity (N).
Mode CTC (4)

Call function or measurements place in Interrupt:
ISR(TIMER1_COMPA_vect){...}

NOTE:	Libraries involved: <avr/interrupt>
		sei(); in main.c
*/

#ifndef TIMER1CTC_H
#define TIMER1CTC_H

//Timer/Counter 1 (16x)
//Hz in freq
void timer1Init_freq(uint16_t freq){
	uint16_t div;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	if((F_CPU/(1024*freq))-1 > 1){
		div = 1024;
		TCCR1B = 0x05;
	}else if((F_CPU/(256*freq))-1 > 1){
		div = 256;
		TCCR1B = 0x04;
	}else if((F_CPU/(64*freq))-1 > 1){
		div = 64;
		TCCR1B = 0x03;
	}else if((F_CPU/(8*freq))-1 > 1){
		div = 8;
		TCCR1B = 0x02;
	}else{
		div = 1;
		TCCR1B = 0x01;
	}
	uint16_t timediv = (int)(F_CPU/(div*freq))-1;
	OCR1AH = (timediv>>8);
	OCR1AL = timediv&0xFF;
	TCCR1B |= (1<<WGM12);
	TIMSK1 |= (1<<OCIE1A);
}
//ms in time
void timer1Init_ms(uint16_t time){
	uint16_t div;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	if(((F_CPU/500)*(time/1024))-1 > 1){
		div = 1024;
		TCCR1B = 0x05;
	}else if(((F_CPU/500)*(time/256))-1 > 1){
		div = 256;
		TCCR1B = 0x04;
	}else if(((F_CPU/500)*(time/64))-1 > 1){
		div = 64;
		TCCR1B = 0x03;
	}else if(((F_CPU/500)*(time/8))-1 > 1){
		div = 8;
		TCCR1B = 0x02;
	}else{
		div = 1;
		TCCR1B = 0x01;
	}
	uint16_t timediv = (int)((F_CPU/500)*(time/div))-1;
	OCR1AH = (timediv>>8);
	OCR1AL = timediv&0xFF;
	TCCR1B |= (1<<WGM12);
	TIMSK1 |= (1<<OCIE1A);
}
/*
//us in time
void timer1Init_us(uint16_t time){
	uint16_t div;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	if((F_CPU/(1000000)*(time/1024))-1 > 1){
		div = 1024;
		TCCR1B = 0x05;
		}else if((F_CPU/(1000000)*(time/256))-1 > 1){
		div = 256;
		TCCR1B = 0x04;
		}else if((F_CPU/(1000000)*(time/64))-1 > 1){
		div = 64;
		TCCR1B = 0x03;
		}else if((F_CPU/(1000000)*(time/8))-1 > 1){
		div = 8;
		TCCR1B = 0x02;
		}else{
		div = 1;
		TCCR1B = 0x01;
	}
	uint16_t timediv = (int)((F_CPU/1000000)*(time/div))-1;
	OCR1AH = (timediv>>8);
	OCR1AL = timediv&0xFF;
	TCCR1B |= (1<<WGM12);
	TIMSK1 |= (1<<OCIE1A);
}
*/
#endif
