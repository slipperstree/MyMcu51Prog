#include "../header/common.h"

void delay_ms(uchar n)
{
	uchar x,y;
	for(x=n;x>0;x--)
	{
		for(y=110;y>0;y--);	
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