/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 10.07.2021
Version: 0.9
Library for Transmit USART. Without using interrupts.
*/
#ifndef USART_H
#define USART_H
#include "main.h"

#define MAXstring	255
//#define bauddiv 		F_CPU/(16*baudrate)


//Initialization USART 8bit 2stop-bita asynch
void UARTInit(uint32_t baud) {
	uint16_t bauddiv = F_CPU/(16*baud);
	UBRR0H = (bauddiv>>8);
	UBRR0L = bauddiv&0xFF;
	UCSR0B = (1<<TXEN0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
}

//send 1byte to USART
void UARTTransmit(uint8_t data ){
  while(!(UCSR0A & (1<<UDRE0))); 
  UDR0 = data; 
}

//send String
void UARTSend(char *data){
	uint8_t index = MAXstring;
	uint8_t saveUCSR0B = 0x00;
	if(UCSR0B&(1<<UDRIE0)){
		saveUCSR0B = UCSR0B;
		UCSR0B &=~(1<<UDRIE0);
	}
	while((*data != '\n')&&(*data != 0)&&(index>0)){
		UARTTransmit(*data);
		*++data;
		index--;
	}
	UARTTransmit('\n');
	if(saveUCSR0B != 0x00) UCSR0B = saveUCSR0B;
}

#endif

