/***************************************************/

/* 测试单片机 STC15F100系列单片机 流水灯测试程序 */ 

/***************************************************/     
#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>

#include "DS1302.h"
#include "display.h"

#define uchar unsigned char
#define uint unsigned int

sbit p30 = P3^0;         //
sbit p31 = P3^1;         //

int tt = 0;
						 
/*************定时器0初始化程序***************/
void timer0_init()
{
	TMOD |= 0X21;	  //定时器1工作方式1
	ET0  = 1;		  //开定时器0中断

	TH0=0xfe;    //定时10ms中断一次
	TL0=0x0c;	 //500us

	EA   = 1;	 	  //开总中断
	TR0  = 1;		  //允许定时器0定时
}

main()
{
	timer0_init();
	DS1302_init();

	//测试用设置时间
	// WriteTime(0x80,( 0 /10)<<4|( 0 %10));	 //秒
	// WriteTime(0x82,( 5 /10)<<4|( 5 %10));  //分
    // WriteTime(0x84,( 15 /10)<<4|( 15 %10));  //时
	// WriteTime(0x86,( 14 /10)<<4|( 14 %10));  //日
	// WriteTime(0x88,( 9 /10)<<4|( 9 %10));  //月
	// WriteTime(0x8c,( 19 /10)<<4|( 19 %10));  //年

	while(1){

		delay_ms(300);

		GetTime();          //获取当前时间
		updateDisplay();	//更新显示用的数据（该处仅仅更新内存里的数据，真正刷新画面是再定时器函数里做的）
	}
}

uchar nowPos = 1;
void Timer0() interrupt 1  //调用定时器0
{
	TH0=0xfe;    //定时10ms中断一次
	TL0=0x0c;	 //500us

	nowPos++;
	if (nowPos==5)
	{
		nowPos=1;
	}

	showPosition(nowPos);
}

// ************ DS1302 ************************************************************
