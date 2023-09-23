#include "main.h"

int main(){
	
	DDRB |= (1<<DDB5); //PB5(D13) - out - LED
	timer1Init_ms(500);
	sei();

	while(1){

	}
	return 0;
}

ISR(TIMER1_COMPA_vect){
	if(PORTB&(1<<PORTB5)){
		PORTB &=~(1<<PORTB5);
	}else{
		PORTB |= (1<<PORTB5);
	}
}
