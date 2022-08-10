/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 07.08.2021
Version: 1.0
Compile library for DS18B20, DS18S20, DS1822 ATMega328p. Compile Lessen.
Setting Port and Pin connected to library "OneWire.h"
Original article: https://aterlux.ru/article/1wire
*/
#ifndef DS18_H
#define DS18_H
#include "OneWire.h"

#define MEASURING	0x44
#define READ_T		0xBE
char termo[7];

//Initialization
void owInit(){
	owHigh();
}
//Read scratchpad
int16_t readScratchpad(uint8_t ds18s20){
	owSend(READ_T); //0xBE
	uint8_t scratchpad [8];
	uint8_t crc = 0x00;
	for(uint8_t i = 0; i<8; i++){
		uint8_t b = owRead();
		scratchpad[i] = b;
		crc = owCRCup(crc, b);
	}
	if(owRead() != crc) {
		status = ERROR_CRC_SCRATCHPAD;
	}else{
		int16_t t = (scratchpad[1] << 8) | scratchpad[0];
		//ds18s20 family code
		if(ds18s20){
			t <<= 3;
			if (scratchpad[7] == 0x10) {
				t &= 0xFFF0;
				t += 12 - scratchpad[6];
			}
		}
		return t;
	}
	return 0;
}
//Read ALL Devices DS18B20, DS18S20, DS1822, return array temperatures (int16_t)
uint8_t readAll(int16_t *arrayTerm){
	if(owSkip()){
		owSend(MEASURING); //0x44
		_delay_ms(900);
		owEnumInit();
		while(1){
			uint8_t *p = owEnumNext();
			if(!p) break;
			uint8_t d = *(p++);
			uint8_t crc = 0;
			uint8_t family_code = d;
			for (uint8_t i = 0; i < 8; i++) {
				crc = owCRCup(crc, d);
				d = *(p++);
			}
			if (crc) {
				status = ERROR_CRC_FAMILY_CODE;
			} else{
				// 0x10 - DS18S20, 0x28 - DS18B20, 0x22 - DS1822
				if ((family_code == 0x28) || (family_code == 0x22) || (family_code == 0x10)) {
						if(family_code == 0x10){
							*(arrayTerm++) = readScratchpad(1);
						}else{
							*(arrayTerm++) = readScratchpad(0);
						}
						status = NORMAL;
					
				}else{
					status = ERROR_TYPE_DEVICE;
				}
			}
		}
	}else{
		status = NONE_DEVICE;
	}
	return status;
}

//Convert int16_t to array char (7byte insert '\n')
char * convertTerm (int16_t num){
	char *term = termo;
	uint16_t divin[] = {1000, 100, 10, 1, 0};
	uint16_t *div = divin;
	char count = '0';
	if (num < 0) {
		num = -num;
		*(term++) = 0x2D; //char '-'
	}
	//Discard the fractional part
	uint16_t snum =  num >> 4; 
	uint8_t flag = 0;
	do{
		count = '0';
		while(snum >= *div){
			snum -= *div;
			count++;
		}
		if((count > '0')||(flag != 0)){
			flag = 1;
			*(term++) = count; //char '0'-'9'
			//*++term;
		}
		//*++div;
	}while(*(++div) > 0);
	*(term++) = 0x2E; //char '.'
	snum = (((uint8_t)(num&0x0F)) * 10) >> 4;
	count = '0';
	while(snum >= 1){
		snum -= 1;
		count++;
	}
	*(term++) = count; //char '0'-'9'
	*term = '\n';
	return termo;
}

//Read Device to family_code and number
int16_t readDS18(uint8_t *device){
	owSend(MEASURING); //0x44
	_delay_ms(900);
	if(!owMatch(device)) return 0;
	if(*device == 0x10){
		return readScratchpad(1);
	}else{
		return readScratchpad(0);
	}
	return 1;
}

//Read one device to bus
int16_t readOneDS18(){
		owSkip();
		owSend(MEASURING); //0x44
		_delay_ms(900);
		owSkip();
		return readScratchpad(0);
}

#endif