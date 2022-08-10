/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 07.07.2021
Version: 1.0
Compile library I2C/TWI interface for ATMega328p. Time-out between 
reading and writing more 1ms.
*/
#ifndef I2C_H
#define I2C_H
//bus baudrate I2C, Hz
#define F_SCL 		100000
//number of connection attempts
#define CONNATEM	5
#define I2C_READ	0x01
#define I2C_WRITE	0x00

//standard address

#define AT24C32		0xA0

//Initialization I2C/TWI
void I2CInit(){
	TWSR = 0; //non used reg TWSR for speed
	//calculation division ratio
	TWBR = ((F_CPU/F_SCL)-16)/2;
	if(TWBR<10) TWBR = 10;
	TWSR &= (~((1<<TWPS1)|(1<<TWPS0)));
}

//Start condition
uint8_t I2CStart(uint8_t addr){
	uint8_t current = 0;
	do{
		if(current == CONNATEM) return 0;
        TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
        while(!(TWCR & (1<<TWINT)));
        if((TWSR & 0xF8) != 0x08) return 0;
        TWDR = addr;
        TWCR=(1<<TWINT)|(1<<TWEN);
        while(!(TWCR & (1<<TWINT)));
        current++;
    }while((TWSR & 0xF8) != 0x18);
    return 1;
}
//Two start condition (read condition)
uint8_t I2CStartTwo(uint8_t addr){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    if((TWSR & 0xF8) != 0x10) return 0;
    TWDR = addr+0x01;
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    if((TWSR & 0xF8) != 0x40) return 0;
    return 1;
}
//I2C Send procedure
uint8_t I2CSend(uint8_t data){
	TWDR = data;
	TWCR |= (1<<TWINT)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
	if((TWSR&0xF8) != 0x28) return 0;
	return 1;
}
//Read last byte
uint8_t I2CReadLast(){
	TWCR |= (1<<TWINT)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
	//if((TWSR&0xF8) != 0x58) return 0;
	return TWDR;
}

//Stop condition
void I2CStop(){
	TWCR |= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(TWCR&(1<<TWSTO));
}

//Read 1 byte (addr 1 byte)
uint8_t I2CRead8(uint8_t addr, uint8_t segment){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	if(!I2CStartTwo(addr)) return 0;
	return I2CReadLast();
}

//Read N byte (addr 1 byte)
uint8_t I2CRead8M(uint8_t addr, uint8_t segment, uint8_t len, uint8_t *req){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	if(!I2CStartTwo(addr)) return 0;
	do{
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    	while(!(TWCR & (1<<TWINT)));
    	if((TWSR & 0xF8) != 0x50) return 0;
    	*(req++) = TWDR;
    	//*++req;
    	len--;
	}while(len>0);
	*req = I2CReadLast();
	I2CStop();
	return 1;
}

//Write byte (addr 1 byte)
uint8_t I2CWrite8(uint8_t addr, uint8_t segment, uint8_t data){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	if(!I2CSend(data)) return 0;
	I2CStop();
	return 1;
}

//Write N byte (addr 1 byte)
uint8_t I2CWrite8M(uint8_t addr, uint8_t segment, uint8_t len, uint8_t *data){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	while(len>0){
		if(!I2CSend(*data++)) return 0;
    	//*++data;
    	len--;
	}
	I2CStop();
	return 1;
}
/*
//Read 1 byte (addr 2 byte)
uint8_t I2CRead16(uint8_t addr, uint16_t segment){
	//uint8_t data;
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment>>8)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	if(!I2CStartTwo(addr)) return 0;
	return I2CReadLast();
}
*/
//Read N byte (addr 2 byte)
uint8_t I2CRead16M(uint8_t addr, uint16_t segment, uint8_t len, uint8_t *req){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment>>8)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	if(!I2CStartTwo(addr)) return 0;
	do{
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    	while(!(TWCR & (1<<TWINT)));
    	if((TWSR & 0xF8) != 0x50) return 0;
    	*(req++) = TWDR;
    	//*++req;
    	--len;
	}while(len>0);
	*req = I2CReadLast();
	I2CStop();
	return 1;
}
/*
//Write byte (addr 2 byte)
uint8_t I2CWrite16(uint8_t addr, uint16_t segment, uint8_t data){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment>>8)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	if(!I2CSend(data)) return 0;
	I2CStop();
	return 1;
}
*/
//Write N byte (addr 2 byte)
uint8_t I2CWrite16M(uint8_t addr, uint16_t segment, uint8_t len, uint8_t *data){
	if(!I2CStart(addr)) return 0;
	if(!I2CSend(segment>>8)) return 0;
	if(!I2CSend(segment&0xFF)) return 0;
	while(len>0){
		if(!I2CSend(*data++)) return 0;
    	//*++data;
    	len--;
	}
	I2CStop();
	return 1;
}
#endif
