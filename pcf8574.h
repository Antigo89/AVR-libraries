/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 11.08.2021
Version: 1.0
Compile library for connected PCF8574 (Port Expanders) to I2C.

*/
#ifndef PCF8574_H
#define PCF8574_H
#include "I2C.h"
//ADDR 0x70 - 0x7E for write. Read ADDR +1. Setting Jumpers A0-A2.
//standard addr 0x40 Disable.
#define PCF8574	0x70

void pcfSend(uint8_t data){
	if(I2CStart(PCF8574)){
		I2CSend(0);
		I2CSend(data);
		I2CStop();
	}
}
 void pcfSendAdr(uint8_t addr, uint8_t data){
	 if(I2CStart(addr)){
		 I2CSend(0);
		 I2CSend(data);
		 I2CStop();
	 }
 }

#endif