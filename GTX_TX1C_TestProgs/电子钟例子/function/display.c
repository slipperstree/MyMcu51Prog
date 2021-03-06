#include "../header/display.h"
#include "../header/ds1302.h"
#include "../header/common.h"

#define DISP_MODE_HHMM		1
#define DISP_MODE_MMDD		2
#define DISP_MODE_YYYY		3
#define DISP_MODE_MMSS		4

static uchar dispMode = DISP_MODE_HHMM;
static uchar dispModeBefore = DISP_MODE_HHMM;

// 数码管引脚连接 位
sbit A1 = P0^0;
sbit A2 = P0^1;
sbit A3 = P0^2;
sbit A4 = P0^3;
sbit A5 = P0^4;
sbit A6 = P0^5;
sbit CSAD = P0^7;

// 位选锁存
sbit WELA = P2^7;
// 段选锁存
sbit DULA = P2^6;

//数字显示code
static uchar code digit[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

//数码管动态刷新当前位
static uchar nowPos = 0;

//数码管各位上的显示内容
static uchar dispDat[6];

//标志位
static uchar bdata flagByte;
sbit flagIsFlash = flagByte^0;				//闪烁状态标志
sbit flagIsShowingForAWhile = flagByte^1;	//暂时显示状态标志
static int showForAWhileInterval = 0;			//暂时显示时间计数

//画面每刷新一次计数器加一，可用于动画，闪烁等动态效果
static uchar frameCounter = 0;

void showPosition(uchar pos, uchar dat){

	//打开指定位
	P0 = 0xff;
	WELA = 1;
	A1 = 1;
	A2 = 1;
	A3 = 1;
	A4 = 1;
	A5 = 1;
	A6 = 1;
	switch(pos)
	{
		case 0:
			A1 = 0;
			break;
		case 1:
			A2 = 0;
			break;
		case 2:
			A3 = 0;
			break;
		case 3:
			A4 = 0;
			break;
		case 4:
			A5 = 0;
			break;
		case 5:
			A6 = 0;
			break;
		default:
			break;
	}
	WELA = 0;//锁存

	//打开数码管的段锁存器（高电平）
	P0 = 0xff;
	DULA = 1;
	//设置数码管的数字
	P0 = dat;
	//关闭数码管的段锁存器（高电平）
	DULA = 0;
}

// 刷新显示用的内容，需要在main循环中调用
void DISPLAY_updateDisplay() {

	frameCounter++;

	// 切换闪烁状态(显示 / 不显示)
	if (frameCounter % 8 == 0)
	{
		flagIsFlash = ~flagIsFlash;
	}

	// 如果当前正在暂时显示状态下，则判断有没有到时间，如果到时间了就切换回原来的显示模式
	if (flagIsShowingForAWhile)
	{
		showForAWhileInterval--;
		if (showForAWhileInterval <= 0)
		{
			// 时间到，恢复之前的状态
			flagIsShowingForAWhile = 0;
			dispMode = dispModeBefore;
		}
	}

	switch (dispMode)
	{
		case DISP_MODE_HHMM:
			// 显示HH:MM:SS
			dispDat[0] = digit[DS1302_GetHour() % 100 / 10];
			dispDat[1] = digit[DS1302_GetHour() % 10];		
			dispDat[2] = digit[DS1302_GetMinute() % 100 / 10];
			dispDat[3] = digit[DS1302_GetMinute() % 10] | (flagIsFlash ? 0x80 : 0x00);		// 显示dp;
			dispDat[4] = digit[DS1302_GetSecond() % 100 / 10];
			dispDat[5] = digit[DS1302_GetSecond() % 10];
			break;

		case DISP_MODE_MMDD:
			// 显示MM.DD
			dispDat[0] = digit[DS1302_GetMonth() % 100 / 10];
			dispDat[1] = digit[DS1302_GetMonth() % 10] | 0x80;		// 显示dp;;		
			dispDat[2] = digit[DS1302_GetDay() % 100 / 10];
			dispDat[3] = digit[DS1302_GetDay() % 10];
			dispDat[4] = digit[DS1302_GetHour() % 100 / 10];
			dispDat[5] = digit[DS1302_GetHour() % 10];
			break;
		
		case DISP_MODE_YYYY:
			// 显示YYYY
			dispDat[0] = digit[2];
			dispDat[1] = digit[0];
			dispDat[2] = digit[DS1302_GetYear() % 100 / 10];
			dispDat[3] = digit[DS1302_GetYear() % 10];
			dispDat[4] = digit[DS1302_GetMonth() % 100 / 10];
			dispDat[5] = digit[DS1302_GetMonth() % 10];
			break;

		case DISP_MODE_MMSS:
			// 显示MM:SS
			dispDat[0] = digit[DS1302_GetMinute() % 100 / 10];
			dispDat[1] = digit[DS1302_GetMinute() % 10];
			dispDat[2] = digit[DS1302_GetSecond() % 100 / 10];
			dispDat[3] = digit[DS1302_GetSecond() % 10];
			dispDat[4] = 0x00;
			dispDat[5] = 0xff;
			break;

		default:
			break;
	}
}

// 刷新显示，需要在main循环中调用
void DISPLAY_refreshDisplay() {
	nowPos++;
	if (nowPos>=6)
	{
		nowPos=0;
	}
	showPosition(nowPos, dispDat[nowPos]);
}

// 显示一小会指定mode，然后切换回原来的显示状态
void showModeForAWhile(uchar mode, int interval) {

	// 当前为非暂时显示状态才允许切换
	if (flagIsShowingForAWhile == 0)
	{
		//记住之前的状态以便到时间后恢复
		dispModeBefore = dispMode;

		//切换状态
		dispMode = mode;

		//倒计时计数开始
		showForAWhileInterval = interval;
		flagIsShowingForAWhile = 1;
	}
}

void DISPLAY_ShowHHMM(){
	dispMode = DISP_MODE_HHMM;
}

void DISPLAY_ShowMMDD(){
	dispMode = DISP_MODE_MMDD;
}

// 显示一小会MMDD，然后切换回原来的显示状态
void DISPLAY_ShowMMDD_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_MMDD, interval);
}

void DISPLAY_ShowYYYY(){
	dispMode = DISP_MODE_YYYY;
}

// 显示一小会YYYY，然后切换回原来的显示状态
void DISPLAY_ShowYYYY_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_YYYY, interval);
}

void DISPLAY_ShowMMSS(){
	dispMode = DISP_MODE_MMSS;
}

// 显示一小会MMSS，然后切换回原来的显示状态
void DISPLAY_ShowMMSS_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_MMSS, interval);
}
