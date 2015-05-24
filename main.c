#include <pic.h>
__CONFIG (INTIO & UNPROTECT & BOREN & MCLRDIS & PWRTEN & WDTDIS);

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000 
#endif

#include "rc5.h"
#include "types.h"

void hwInit(void)
{
	CMCON =  0x07;						// ���������� ������������ �� GP0 ,GP1
	GPIO =   0x0;						// ������ ����
	TRISIO = 0x20;						// 5 ����, ��������� �� �����         
	GPPU = 1;							// ���������� ������������� ���������

	rc5Init();

	return;
}	

void showRC5Buf(uint16_t buf)
{
	int8_t i;
	
	GPIO1 = 0;
	for (i = 13; i >= 0; i--) {
		if (buf & (1<<i)) {
			GPIO1 = 1;
		} else {
			GPIO1 = 0;
		}	
		__delay_us(200);
	}
	GPIO1 = 0;
}	

int main (void){

	hwInit();

	GPIO0 = 0;

	uint16_t rc5Buf;

	while(1) {
		__delay_ms(200);
		rc5Buf = getRC5RawBuf();
		showRC5Buf(rc5Buf);
	}	
}	