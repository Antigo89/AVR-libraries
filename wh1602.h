/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 05.07.2021
Version: 1.0
Libraries for LCD WH1602 (controller HD44780)
Connection display D7(14)->PD7 D6(13)->PD6 D5(12)->PD5 D4(11)->PD4 D3(10)->NA
D2(9)->NA D1(8)->NA D0(7)->NA
E(6)->PD1 R/W(5)->GND RS(4)->PD0 Vee->(resistor 10K) GND->GND Vcc->+5V
INT0(PD2) and INT1(PD3) - free.
*/
#ifndef WH1602_H
#define WH1602_H
#include <avr/delay.h>

#define RS_ENABLE		PORTD |= (1<<PORTD0)
#define RS_DISABLE		PORTD &= ~(1<<PORTD0)
#define E_ENABLE		PORTD |= (1<<PORTD1)
#define E_DISABLE		PORTD &= ~(1<<PORTD1)
uint8_t init50[3] = {0x30, 0x30, 0x28};
uint8_t init20[4] = {0x08, 0x01, 0x06, 0x0D};


void lcdWriteHalfByte(uint8_t data){
	PORTD |= data;
	E_ENABLE;
	_delay_ms(20);
	E_DISABLE;
	PORTD &= ~(data);
}

void lcdWriteByte(uint8_t data){
	lcdWriteHalfByte(data&0xF0);
	lcdWriteHalfByte(data<<4);
}
void lcdPrint(char *str){
	do {
        lcdWriteByte(*str);
    }while(*++str);	
}

void lcdInit(){
	uint8_t i = 0;
	DDRD = 0b11110011;
	RS_DISABLE;
	_delay_ms(200);
	while(i<3){
		lcdWriteHalfByte(init50[i]);
		_delay_ms(50);
		i++;
	}
	i = 0;
	while(i<5){
		lcdWriteByte(init20[i]);
		_delay_ms(20);
		i++;
	}
	RS_ENABLE;
}

void lcdWriteCmd(uint8_t cmd) {
    RS_DISABLE;
    lcdWriteByte(cmd);
    RS_ENABLE;
}

void lcdSetCur(uint8_t line,uint8_t pos) {
    pos |= 0b10000000;
    if (line == 1) {
        pos += 0x40;
    }
    lcdWriteCmd(pos);
}

#endif

