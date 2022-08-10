/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 15.07.2021
Version: 1.0
Compile library for connection via SPI to ADC MCP3008. ASC 10bit 8 channel.
*/
#ifndif MCP3008_H
#define MCP3008_H

#include "SPI.h"

//Take >8bit
uint16_t SPIRead(uint8_t ch){
	uint8_t byte,data_high,data_low;
	byte=0b00000110;
	if(ch>3)
		byte|=0b00000001;
	PORTB &= ~(1<<2)
	SPIWrite(byte);
	byte=ch<<6;
	data_high = SPIWrite(byte);
	data_high &= 0b00001111;
	data_low = SPIWrite(0xFF);
	PORTB |= (1<<2);
	return ((data_high<<8)|data_low);
}

#endif
