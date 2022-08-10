	/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 17.07.2021
Version: 0.9
Compile library SPI interface for ATMega328p. Master device. Speed 1MHz.
*/
#ifndef SPI_H
#define SPI_H

#define SPI_PORT 	PORTB
#define SCK_POS 	5
#define MISO_POS 	4
#define MOSI_POS	3
#define SS_POS		2
#define SPI_DDR 	DDRB

//additional Chip Select (CS)
//#define CS_PORT		PORTB
//#define CS_DDR		DDRB
//#define CS_POS		2
//#define CS_HIGH()	CS_PORT|=(1<<CS_POS)
//#define CS_LOW()	CS_PORT&=(~(1<<CS_POS))

//Initialization SPI Master
void SPIMasterInit(){
	//MOSI SCK SS - outport
	DDRB |=((1<<3)|(1<<5)|(1<<2));
	//SS - '1'
	PORTB |=(1<<2);
	//Enable SPI, Master, sck = f/16 (1MHz)
	SPCR|=(1<<SPE)|(1<<MSTR)|(1<<SPR0);
	#ifdef CS_POS
	CS_DDR|=(1<<CS_POS);
	CS_HIGH();
	#endif
}
//Initialization SPI Slave
void SPISlaveInit(){
	//MISO - outport
	DDRB |=(1<<4);
	//Enable SPI
	SPCR|=(1<<SPE);
}
//End connected SPI
void SPIClose(){
	SPCR &= ~(1<<SPE);
	DDRB &= (~((1<<3)|(1<<5)));
}
//Send 8bit to standart SS
uint8_t SPIWrite(uint8_t data){
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return (SPDR);
}
//Selected SS(CS) to '0' when reading


uint8_t SPIReadSlave(){
   while(!(SPSR & (1 << SPIF)));
   return SPDR;
}
#endif
