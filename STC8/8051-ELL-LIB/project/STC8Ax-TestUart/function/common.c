#include "../header/common.h"

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