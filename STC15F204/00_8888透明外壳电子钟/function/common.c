#include "../header/common.h"

void delay_ms(unsigned int n)
{
	unsigned int x,y;
	for(x=n;x>0;x--)
	{
		for(y=110;y>0;y--);	
	}
}
