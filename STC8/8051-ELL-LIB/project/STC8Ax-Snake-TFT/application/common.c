#include "common.h"

// #pragma optimize=none �Ǹ�IAR���ģ����ⱻ�������Ż���������ʱ��������
// ��ʹ��iar����ʱ������Ҫɾ�����߸ĳ�keil֧�ֵ��﷨
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
