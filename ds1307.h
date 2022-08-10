/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 12.07.2021
Version: 1.1
Compile library for RTC DS1307 or DS3231 (I2C interface). Only data and time.
Receive array Integer or array Character(String).

Return String time in the format: HH:MM:SS
Return String date in the format: DD.MM.YY
Return Integer date and time in the format: {S, Minu, H, DW, D, Month, Year}
Takes Integer date and time in the format: {S, Minu, H, DW, D, Month, Year}
	
V1.1: Added the ability to write date / time from a string.
*/
#ifndef DS1307_H
#define DS1307_H
#include "I2C.h"
#define DS1307 		0xD0

#define SECONDS_REG	0x00
#define MINUTES_REG	0x01
#define HOURS_REG  	0x02
#define DAYOFWK_REG	0x03
#define DAYS_REG	0x04
#define MONTHS_REG	0x05
#define YEARS_REG	0x06

uint8_t mask[7] = {0x7F, 0x7F, 0x3F, 0x07, 0x3F, 0x1F, 0x7F};
char str[9];
uint8_t timeBD[7] = {0, 0, 0, 0, 0, 0, 0};
//Int to DS1307
uint8_t RTCConvertBinDec(uint8_t c){
	uint8_t ch = ((c/10)<<4|(c%10));
	return ch;
}
//DS1307 to Int
uint8_t RTCConvertDec(uint8_t c){
	uint8_t ch = ((c>>4)*10 + (c&0x0f));
	return ch;
}
//Initialization
void timeInit(){
	I2CInit();
}
//Write time
void timeWrite(uint8_t *time){
	//uint8_t timeBD[7] = {0, 0, 0, 0, 0, 0, 0}; 
	uint8_t index = 0;
	while(index < 7){
		timeBD[index] = RTCConvertBinDec(time[index]&mask[index]);
		index++;
	}
    timeBD[SECONDS_REG] &=~(1<<7);
    timeBD[SECONDS_REG] |= (1<<0);
    timeBD[HOURS_REG] &=~(1<<7);
	I2CWrite8M(DS1307, 0, 7, timeBD);
}

//Read time
uint8_t * timeRead(){
	uint8_t index = 0;
	I2CRead8M(DS1307, 0, 6, timeBD);
	while(index < 7){
		timeBD[index] = RTCConvertDec(timeBD[index]&mask[index]);
		index++;
	}
	return &timeBD[0];
}

//Read time to String (char time[9])
char * timeReadString(){
	//uint8_t read[7];
	char *time = &str[0];
	I2CRead8M(DS1307, 0, 7, timeBD);
	int8_t index = 2;
	while(index >= 0){
		*(time++) = ((timeBD[index] & mask[index])>>4) + '0';
		//*++time;
		*(time++) = (timeBD[index] & 0x0F) + '0';
		//*++time;
		*(time++) = ':';
		//*++time;
		index--;
	}
	//*--time;
	*(--time) = '\n';
	return &str[0];
}
//Read date to String (char date[9])
char * dateReadString(){
	//uint8_t read[7];
	char *date = &str[0];
	int8_t index = 4;
	I2CRead8M(DS1307, 0, 7, timeBD);
	while(index < 7){
		*(date++) = ((timeBD[index] & mask[index])>>4) + '0';
		//*++date;
		*(date++) = (timeBD[index] & 0x0F) + '0';
		//*++date;
		*(date++) = '.';
		//*++date;
		index++;
	}
	//*--date;
	*(--date) = '\n';
	return &str[0];
}
//Write time out String (9 byte)
void timeWriteString(char *time){
	timeBD[HOURS_REG] = ((*(time++) - '0') * 10);
	timeBD[HOURS_REG] = timeBD[HOURS_REG] + (*(time++) - '0');
	timeBD[HOURS_REG] = RTCConvertBinDec(timeBD[HOURS_REG]&mask[HOURS_REG]);
	if((*time++) == ':');
	timeBD[MINUTES_REG] = (*(time++) - '0') * 10;
	timeBD[MINUTES_REG] += (*(time++) - '0');
	timeBD[MINUTES_REG] = RTCConvertBinDec(timeBD[MINUTES_REG]&mask[MINUTES_REG]);
	if((*time++) == ':');
	timeBD[SECONDS_REG] = (*(time++) - '0') * 10;
	timeBD[SECONDS_REG] += (*(time++) - '0');
	timeBD[SECONDS_REG] = RTCConvertBinDec(timeBD[SECONDS_REG]&mask[SECONDS_REG]);
	timeBD[SECONDS_REG] &=~(1<<7);
	timeBD[SECONDS_REG] |= (1<<0);
	timeBD[HOURS_REG] &=~(1<<7);
	I2CWrite8M(DS1307, 0, 3, timeBD);
}
//Write date out String (9 byte)
void dateWriteString(char *date){
	timeBD[DAYS_REG] = ((*(date++) - '0') * 10);
	timeBD[DAYS_REG] = timeBD[DAYS_REG] + (*(date++) - '0');
	timeBD[DAYS_REG] = RTCConvertBinDec(timeBD[DAYS_REG]&mask[DAYS_REG]);
	if((*date++) == '.');
	timeBD[MONTHS_REG] = (*(date++) - '0') * 10;
	timeBD[MONTHS_REG] += (*(date++) - '0');
	timeBD[MONTHS_REG] = RTCConvertBinDec(timeBD[MONTHS_REG]&mask[MONTHS_REG]);
	if((*date++) == '.');
	timeBD[YEARS_REG] = (*(date++) - '0') * 10;
	timeBD[YEARS_REG] += (*(date++) - '0');
	timeBD[YEARS_REG] = RTCConvertBinDec(timeBD[YEARS_REG]&mask[YEARS_REG]);
	I2CWrite8M(DS1307, DAYS_REG, 3, &timeBD[DAYS_REG]);
}

#endif
