/***************************************************/
/* 电子钟程序
/* 实时时钟采用DS1302芯片
/* 可以用串口进行校时（以便今后增加ESP8266模块进行网络校时）
/***************************************************/
//#include <string.h>

#include "header/STC15104E.h"
#include "header/display.h"
#include "header/key.h"
#include "header/common.h"
#include "header/ds1302.h"
#include "header/uart.h"
#include "header/sensorAdc.h"

int tt_getTime = 0;
int tt_refresh = 0;

// 开机上电时，6脚延时60秒后才输出低电平。
// 用于8266因停电而不能授时的问题。
// 因为来电时，电子钟的8266要连接网，但网猫大约要5分钟后才能连上网。
// 故8266是连不上网的，一旦连不上，8266就会放弃连接。故时钟就处于脱网状态了
int tt_P13_ms = 0;
int tt_P13_sec = 0;

main()
{
	//上电时，6脚P13设置高电平
	P13 = 1;

	DS1302_init();
	UART_init();
	KEY_init();
	ADC_init();

	// 开启呼吸效果
	//DISPLAY_SetBreathMode(DISPLAY_BREATH_MODE_ON, DISPLAY_SPEED_LV_5);

	while(1){
		//这个延时太长会导致软串口丢数据
		//delay_ms(1);

		KEY_keyscan();

		//软串口服务程序
		UART_SoftUartIsr();

		//大约1分钟后，6脚P13设置低电平(2000这个数字可调整，调小可减少时间，调高可增加时间)
		if (tt_P13_sec < 60) tt_P13_ms++;
		if (tt_P13_ms >= 2000)
		{
			tt_P13_ms=0;
			tt_P13_sec++;
			if (tt_P13_sec>=60)
			{
				P13 = 0;
			}
			
		}
		

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
		if (tt_refresh >= 5)
		{
			tt_refresh = 0;

			//时间也取了，用来显示的内容也更新完了，这里开始刷新画面
			//具体刷新的逻辑由display.h的下列接口函数来完成，用户可自行适配不同的显示模块，比如1602，LCD等
			DISPLAY_refreshDisplay();
		}
	}
}
