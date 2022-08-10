/*
Author: Antigo
Mail: Antigo1989@gmail.com
Date: 29.07.2021
Version: 1.0
Compile library(low level) 1-Wire interface for ATMega328p. Compile Lessen.
Original article: https://aterlux.ru/article/1wire
*/
#ifndef ONEW_H
#define ONEW_H
//Connected 1-Wire
#define PORTOW	PORTB
#define DDROW	DDRB
#define PINOW	PINB
#define OW		1
//Power resistor
#define DDROW_STRONG	DDRB
#define PORTOW_STRONG	PORTB
#define OW_STRONG		2

//Set Strong resistor
void owStrongEn() {
	PORTOW_STRONG |= (1<<OW_STRONG);
	DDROW_STRONG |= (1<<OW_STRONG);
}

//Reset Strong resistor
void owStrongDis() {
	DDROW_STRONG &= ~(1<<OW_STRONG);
	PORTOW_STRONG &= (1<<OW_STRONG);
}


//Commands
//Skip reading procedure, for Search All or Single-Drop mode
#define SKIP_ROM	0xCC
//Read ROM 64 bit family code and number device
#define READ_ROM	0x33
//Selected one device for family code and number device
#define MATCH_ROM	0x55
//Search all devices (ROM)
#define SEARCH_ROM	0xF0

enum {
	NORMAL,
	NONE_DEVICE,
	ERROR_CRC_FAMILY_CODE,
	ERROR_CRC_SCRATCHPAD,
	ERROR_TYPE_DEVICE
} status;

typedef struct {
	uint8_t date[8];
} deviceCode_t;

// Set LOW to 1-Wire
inline void owLow() {
	DDROW |= (1<<OW);
}

// Set HIGH to 1-Wire
inline void owHigh() {
	DDROW &= ~(1<<OW);
}

// Read level to 1-Wire
inline uint8_t owLevel() {
	return PINOW & (1<<OW);
}

uint8_t owReset()
{
	owLow();
	_delay_us(640); //480..960 mks
	owHigh();
	_delay_us(2);
	// Later 60 мс request signal
	for (uint8_t c = 80; c; c--) {
		if (!owLevel()) {
			while (!owLevel());
			return 1;
		}
		_delay_us(1);
	}
	return 0;
}


//Send one bit
void owSendBit(uint8_t bit) {
	owLow();
	if (bit) {
		_delay_us(5); // Low level 1 to 15 mks
		owHigh();
		_delay_us(90); // >60mks
		} else {
		_delay_us(90); // 60-120 mks
		owHigh();
		_delay_us(5);
	}
}

//Send byte LSB forward
void owSend(uint8_t b) {
	for (uint8_t p = 8; p; p--) {
		owSendBit(b & 1);
		b >>= 1;
	}
}

//Read one bit
uint8_t owReadBit() {
	owLow();
	_delay_us(2); // >1mks
	owHigh();
	_delay_us(8); // <15mks
	uint8_t r = owLevel();
	_delay_us(80); // Next >60mks
	return r;
}

//Read byte LSB forward
uint8_t owRead() {
	uint8_t ret = 0;
	for (uint8_t p = 8; p; p--) {
		ret >>= 1;
		if (owReadBit()) ret |= 0x80;
	}
	return ret;
}


//CRC one byte
uint8_t owCRCup(uint8_t crc, uint8_t b) {
	//  return pgm_read_byte(&onewire_crc_table[crc ^ b]);
	for (uint8_t i = 8; i; i--) {
		crc = ((crc ^ b) & 1) ? (crc >> 1) ^ 0b10001100 : (crc >> 1);
		b >>= 1;
	}
	return crc;
}

//Init bus, broadcast skip ROM
uint8_t owSkip() {
	if (!owReset())return 0;
	owSend(SKIP_ROM); //broadcast 0xCC
	return 1;
}


//Init + Read ROM (family code/number device 64 bit) Single-drop mode
uint8_t owReadROM(uint8_t * buf) {
	if (!owReset()) return 0;
	owSend(READ_ROM); //0x33
	for (uint8_t p = 0; p < 8; p++) {
		*(buf++) = owRead();
	}
	return 1;
}

//Init and Selected device for family code and number device 64 bit.
uint8_t owMatch(uint8_t * data) {
	if (!owReset()) return 0;
	owSend(MATCH_ROM); //0x55
	for (uint8_t p = 0; p < 8; p++) {
		owSend(*(data++));
	}
	return 1;
}


uint8_t owEnum[8]; // search addr
uint8_t owEnumLast; // last bit 0

// Init search all
void owEnumInit() {
	for (uint8_t p = 0; p < 8; p++) {
		owEnum[p] = 0;
	}
	owEnumLast = 65;
}

//Next found addresses
uint8_t * owEnumNext() {
	if (!owEnumLast) return 0;
	if (!owReset()) return 0;
	uint8_t bp = 8;
	uint8_t * pprev = &owEnum[0];
	uint8_t prev = *pprev;
	uint8_t next = 0;
	
	uint8_t p = 1;
	owSend(SEARCH_ROM); //0xF0
	uint8_t newfork = 0;
	while(1) {
		uint8_t not0 = owReadBit();
		uint8_t not1 = owReadBit();
		if (!not0) {
			if (!not1) {
				if (p < owEnumLast) {
					if (prev & 1) {
						next |= 0x80;
					} else {
						newfork = p;
					}
				} else if (p == owEnumLast) {
					next |= 0x80;
				} else {
					newfork = p;
				}
			}
		} else {
			if (!not1) {
				next |= 0x80;
			} else {
				return 0;
			}
		}
		owSendBit(next & 0x80);
		bp--;
		if (!bp) {
			*pprev = next;
			if (p >= 64) break;
			next = 0;
			pprev++;
			prev = *pprev;
			bp = 8;
		} else {
			if (p >= 64) break;
			prev >>= 1;
			next >>= 1;
		}
		p++;
	}
	owEnumLast = newfork;
	return &owEnum[0];
}

//Init + ReadRom for Last search addr
uint8_t owMatchLast() {
	return owMatch(&owEnum[0]);
}

//Search All devices. Return *pointer array family code und number.
uint8_t searchAll(uint8_t *arrayDevices){
	if(owSkip()){
		owEnumInit();
		while(1){
			uint8_t *p = owEnumNext();
			if(!p) break;
			uint8_t d = *(p++);
			uint8_t crc = 0;
			for (uint8_t i = 0; i < 8; i++) {
				crc = owCRCup(crc, d);
				*(arrayDevices++) = d;
				d = *(p++);
				
			}
			if (crc) return 0;
		}
	}else{
		return 0;
	}
	return 1;
}

//Alternative CRC
/*
const uint8_t PROGMEM onewire_crc_table[] = {
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

inline uint8_t owCRCup(uint8_t crc, uint8_t b) {
	return pgm_read_byte(&onewire_crc_table[crc ^ b]);
}
*/
#endif