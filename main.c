#include <pic.h>
__CONFIG (INTIO & UNPROTECT & BOREN & MCLRDIS & PWRTEN & WDTDIS);

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000 
#endif

#include "rc5.h"
#include "types.h"

void hwInit(void)
{
	CMCON =  0x07;						// отключение компараторов на GP0 ,GP1
	GPIO =   0x0;						// чистим порт
	TRISIO = 0x20;						// 5 вход, остальное на выход         
	GPPU = 1;							// отключение подт€гивающих резистров

	rc5Init();

	return;
}	

int main (void){

	hwInit();
	
	uint16_t rc5Buf, rc5Last;

	while(1) {
		rc5Buf = getRC5RawBuf();
		if (rc5Buf != RC5_BUF_EMPTY)
			rc5Last = rc5Buf;
		
		showHex(rc5Last);
	}	
}	