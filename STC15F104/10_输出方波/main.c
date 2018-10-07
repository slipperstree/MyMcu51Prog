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

void  delay(uint t);

void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<250;j++);
}
						 
int a=0;

void Timer0Init(void)		//1毫秒@24.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xBE;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}

main()
{
	Timer0Init();
	
    while(1){
		 
	}
}

void timer0() interrupt 1
{
	P33 = !P33;
}

