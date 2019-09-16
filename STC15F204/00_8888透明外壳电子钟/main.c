/***************************************************/
/* 电子钟程序
/* 实时时钟采用DS1302芯片
/* 可以用串口进行校时（以便今后增加ESP8266模块进行网络校时）
/***************************************************/     
#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#include "header/STC15104E.h"
#include "header/display.h"
#include "header/key.h"
#include "header/common.h"
#include "header/ds1302.h"
#include "header/uart.h"

int tt_getTime = 0;
int tt_refresh = 0;

/*************定时器0初始化程序***************/
// void timer0_init()
// {
// 	TMOD |= 0X21;	  //定时器1工作方式1
// 	ET0  = 1;		  //开定时器0中断

// 	TH0=0xfe;    //定时10ms中断一次
// 	TL0=0x0c;	 //500us

// 	EA   = 1;	 	  //开总中断
// 	TR0  = 1;		  //允许定时器0定时
// }

main()
{
	uchar nowPos = 1;

	//timer0_init();
	DS1302_init();
	UART_init();

	// 测试用设置时间(2019/09/14 15:00:00)
	// WriteTime_Sec(0);	//秒
	// WriteTime_Min(0);  	//分
    // WriteTime_Hour(15);  //时
	// WriteTime_Day(14);  	//日
	// WriteTime_Month(9);  //月
	// WriteTime_Month(19);  //年

	while(1){
		//这个延时太长会导致软串口丢数据
		delay_ms(1);

		KEY_keyscan();

		//软串口服务程序
		UART_SoftUartIsr();

		//获取当前时间
		tt_getTime++;
		if (tt_getTime >= 300)
		{
			tt_getTime = 0;

			DS1302_GetTimeFromDS1302();

			//根据上面的GetTime取到的数据(定义再DS302.h里面的全局变量shi fen miao nian yue ri等)更新显示用的数据
			//该处仅仅更新内存里的数据(定义在display.h里的 d1 d2 d3 d4)，真正刷新画面是在后面的showPosition函数里做的）
			DISPLAY_updateDisplay();
		}

		// 刷新画面 ----------------
		tt_refresh++;
		if (tt_refresh >= 10)
		{
			tt_refresh = 0;

			//时间也取了，用来显示的内容也更新完了，这里开始刷新画面
			//具体刷新的逻辑由display.h的下列接口函数来完成，用户可自行适配不同的显示模块，比如1602，LCD等
			DISPLAY_refreshDisplay();
		}
	}
}

//void Timer0() interrupt 1  //调用定时器0
// void Timer0()
// {
// 	TH0=0xfe;    //定时10ms中断一次
// 	TL0=0x0c;	 //500us
// }
