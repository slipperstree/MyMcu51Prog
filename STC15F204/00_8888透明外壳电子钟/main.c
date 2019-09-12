/***************************************************/

/* 测试单片机 STC15F100系列单片机 流水灯测试程序 */ 

/***************************************************/     
#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>
#define uchar unsigned char
#define uint unsigned int

sbit p30 = P3^0;         //
sbit p31 = P3^1;         //

sbit A1 = P3^2;         //
sbit A2 = P3^3;         //
sbit A3 = P3^4;         //
sbit A4 = P3^5;         //

sbit segA = P2^0;
sbit segB = P2^1;
sbit segC = P2^2;
sbit segD = P2^3;
sbit segE = P2^4;
sbit segF = P2^5;
sbit segG = P2^6;
sbit segDP = P2^7;

//第124位用数字显示code
uchar code digit124[17]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71}; // 0-F
//第3位数码管在硬件上是倒过来接的，数字显示code不一样，要改
uchar code digit3[17]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71}; // 0-F

uchar code digit3[];

void  delay(uint t);

void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<250;j++);
}
						 
int a=0;
main()
{
	uint i=0;

	// 共阳，位选高电平点亮（用的是三极管驱动，IO输出0，位选呈高电平）
    A1 = 0;
	A2 = 0;
	A3 = 0;
	A4 = 0;

	// 共阳，段码低电平点亮
	// segA = 0;
	// segB = 0;
	// segC = 0;
	// segD = 0;
	// segE = 0;
	// segF = 0;
	// segG = 1;
	// segDP = 1;
	
	for (i = 0; i < 10; i++)
	{
		P2 = digit124[i];
		delay(1400);
	}
	
}