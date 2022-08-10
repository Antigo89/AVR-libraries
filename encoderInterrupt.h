/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 23.07.2021
Version: 0.9
Compile library for processing encoder. Connect DW to any PIND, 
and CLK to PD3(INT1) or PD2 (INT0). 
Polling occurs by interrupt INT1 or INT0.

Example:
ISR(INT1_vect){
	encClick(&count)
}
Default connected CLK -> PD3(INT1), DW -> PD2(INT0)
*/
#ifndef ENCODERI_H
#define ENCODERT_H
#define STEP	1
uint8_t volatile dwPin;
 //Initialization
 void encPinInit(uint8_t dwpin){
	 dwPin = dwpin;
	 DDRD &= ~((1<<DDD3)|(1<<dwPin));
	 PORTD |= (1<<PORTD3)|(1<<dwPin);
	 EICRA |= (1<<ISC11);
	 EIMSK |= (1<<INT1);
 }
//Default Initialization
 void encInit(){
	 encPinInit(2);
 }
 
 void encClick(uint8_t *count){
	 cli();
	 if(PIND & (1<<dwPin)){
		 if(*count < 255) *count = *count + 1;
	 }else{
		 if(*count > 0) *count = *count - 1;
	 }
	 sei();
 }
 #endif