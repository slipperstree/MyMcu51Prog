#include "../header/common.h"

void delay_ms(uchar n)
{
	uchar x,y;
	for(x=n;x>0;x--)
	{
		for(y=110;y>0;y--);	
	}
}

// 适用于STC15Fxx / STC15Lxx / STC15Wxx
// 不包括STC15F104E / STC15L104E / STC15F204EA / STC15L204EA
void Delay100ms_STC15Fxx_110592M(uchar n)		//@11.0592MHz
{
	unsigned char i, j, k, l;
	for(l=n;l>0;l--) {
		_nop_();_nop_();i = 5;j = 52;k = 195;do{do{while (--k);} while (--j);} while (--i);
	}
}

uchar My_strlen(char* x)
{
	uchar count = 0;
	while(*x != 0x00)
	{
		count++;
		x++;
	}
	return count;
}