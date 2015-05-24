#include <pic.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000 
#endif

#include "rc5.h"
#include "types.h"

volatile uint8_t rc5Cnt;						/* RC5 bit counter */
volatile rc5State state;						/* Decoding process status */
volatile uint16_t rc5RawBuf = RC5_BUF_EMPTY;	/* Last decoded RC5 command */
volatile uint16_t rc5Cmd;						/* RC5 command */

static const uint8_t trans[4] = {0x01, 0x91, 0x9b, 0xfb};

const unsigned int arr_seg[]={
// 0bABCDEFGH <– расположение сегментов по битам
   0b00000011, // 0
   0b10011111, // 1
   0b00100101, // 2
   0b00001101, // 3
   0b10011001, // 4
   0b01001001, // 5
   0b01000001, // 6
   0b00011111, // 7
   0b00000001, // 8
   0b00001001, // 9
   0b00010001, // A
   0b11000001, // B
   0b01100011, // C
   0b10000101, // D
   0b01100001, // E
   0b01110001, // F
}; // конец массива

static volatile uint8_t dig[4];								// Цифры таймера

static void rc5Reset()
{
	rc5Cnt = 14;
	rc5Cmd = 0;
	state = STATE_BEGIN;

	return;
}

void showHex(uint16_t num)
{
	uint8_t i;

	for (i = 0; i < 4; i++)
		dig[i] = arr_seg[(num >> (i << 2)) & 0x0F];
//		dig[i] = arr_seg[(num >> (i * 4)) & 0x0F];

	return;
}	

void rc5Init(void)
{
	PEIE = 1;					// прерывание от перефирии
	GPIE = 1;
	IOCB5 = 1;					// Включаем прерывание по изменению на входе 5
	GIE = 1; 

	// Настраиваем предделитель 16бит таймера

	rc5Reset();

	//****инициализация таймера1***//******************************************************
 	TMR1ON = 0;                                 // остановка таймера
 	TMR1CS = 0;                                 // внутренний источник тактирования
 	T1CKPS1 = 0; 
 	T1CKPS0 = 0;                                // делитель на 1 (1MHz)
 	TMR1ON = 1;                                 // запуск таймера

 	TMR1H = 0x00;
 	TMR1L = 0x00;

	// инициализация таймера 
 	
 	T0CS = 0;
 	PS2 = 0;
 	PS1 = 0;
 	PS0 = 1;
 	
 	T0IE = 1;

	return;
}

void SPI (uint16_t x)							// отправка данных
{
	static uint8_t i;
   	for (i = 0; i < 16; i++) {					// отправка 16бит(2байта)
		if (x & 0x8000)
			GPIO0 = 1;
		else
			GPIO0 = 0;
		x <<= 1;

  		GPIO2 = 1;								// защёлкиваем посланные данные в выходных регистрах(строб)
   		GPIO2 = 0;
   	}          
   	GPIO1 = 1;									// устанавливаем защёлку на входных регистрах
   	GPIO1 = 0;									// снимаем защёлку на входных регистрах

	return;                         
}

void interrupt isr (void)
{
	static uint8_t p;
	
	if (T0IF) {
		p++;
		if (p >= 4)
			p = 0;
		
   	   	SPI((0x0100 << p) | dig[p]);
  
   	   	T0IF = 0;
   	   	return;
			
	} else if (GPIF) {	
		uint16_t delay;
		delay = (TMR1H << 8);
		delay |= TMR1L;
	
		/* event: 0 / 2 - short space/pulse, 4 / 6 - long space/pulse */
		uint8_t event;
		if (GPIO5)
			event = 2;
		else
			event = 0;
	
		if (delay > RC5_LONG_MIN && delay < RC5_LONG_MAX) {
			event += 4;
		} else if (delay < RC5_SHORT_MIN || delay > RC5_SHORT_MAX) {
			rc5Reset();
		}
	
		if (state == STATE_BEGIN) {
			rc5Cnt--;					
			rc5Cmd |= (1 << rc5Cnt);	
			state = STATE_MID1;			
			
			TMR1H = 0x00;
	 		TMR1L = 0x00;					
	
			GPIF = 0;
			return;
		}
	
		rc5State newstate = (trans[state] >> event) & 0x03;
		if (newstate == state || state > STATE_START0) {
			rc5Reset();
			GPIO0 = 0;
			GPIF = 0;
			return;
		}
		state = newstate;
	
		if (state == STATE_MID0)
			rc5Cnt--;
		else if (state == STATE_MID1) {
			rc5Cnt--;
			rc5Cmd |= (1 << rc5Cnt);
		}
	
		if (rc5Cnt == 0 && (state == STATE_START1 || state == STATE_MID0)) {
			rc5RawBuf = rc5Cmd;
			rc5Reset();
		}
	
		TMR1H = 0x00;
	 	TMR1L = 0x00;
	
		GPIF = 0;
		return;
	}	
} 


uint16_t getRC5RawBuf(void)
{
	uint16_t ret = rc5RawBuf;
	rc5RawBuf = RC5_BUF_EMPTY;
	return ret;
}
