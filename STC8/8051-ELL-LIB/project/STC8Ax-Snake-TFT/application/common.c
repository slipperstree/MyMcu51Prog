#include "common.h"

// #pragma optimize=none 是给IAR看的，避免被编译器优化掉导致延时被跳过。
// 不使用iar环境时可能需要删掉或者改成keil支持的语法
//#pragma optimize=none
void My_delay_ms(unsigned int n)
{
	unsigned int x,y;
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
