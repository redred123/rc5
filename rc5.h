#ifndef RC5_H
#define RC5_H

#include "types.h"

#define RC5_SHORT_MIN	444		/* 444 microseconds */
#define RC5_SHORT_MAX	1333	/* 1333 microseconds */
#define RC5_LONG_MIN	1334	/* 1334 microseconds */
#define RC5_LONG_MAX	2222	/* 2222 microseconds */

#define RC5_STBT_MASK	0x3000 // 0b11 0000 0000 0000
#define RC5_TOGB_MASK	0x0800 // 0b00 1000 0000 0000
#define RC5_ADDR_MASK	0x07C0 // 0b00 0111 1100 0000
#define RC5_COMM_MASK	0x003F // 0b00 0000 0011 1111

#define RC5_BUF_EMPTY	0

typedef enum {
	STATE_START1, // 0
	STATE_MID1,   // 1
	STATE_MID0,   // 2
	STATE_START0, // 3
	STATE_ERROR,  // 4
	STATE_BEGIN,  // 5
	STATE_END     // 6
} rc5State;

void rc5Init(void);

uint16_t getRC5RawBuf(void);

#endif /* RC5_H */
