/***************************************************/
/* 贪食蛇_MAX7219驱动  */
/*  */
/***************************************************/     
#include <STC12x20xx.h>
#include <memoryGame.h>

#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#define uchar unsigned char
#define uint unsigned int

#define TRUE 1
#define FALSE 0

uchar i;
uint n;
uint a, b, c;
uint intensity; //亮度0-15

uint tt;
uint waitTime;
#define TIME_STOP 0 // 这是一个特殊时间，返回给主程序0表示让主程序暂停NEXT函数的调用，等待用户输入

void init();
void display();

void setBitData(uchar);
void setByte(uchar);

void setDecodeMode(uchar);
void setIntensity(uchar);
void setScanLimit(uchar);
void setShutdownMode(uchar);
void setDisplayTestMode(uchar);
void setRowData(uchar, uchar);

void delay(uint);
void load();

void checkBtn();

sbit DIO = P1^4;	// 串行数据输入
sbit LOAD = P1^3;	// 锁存信号——上升沿有效
sbit SCLK = P3^4;	// 串行数据时钟信号————上升沿有效

// 调亮度按钮（不使用外部中断，因为104E系列不支持外部中断，104W是支持，但为了通用，使用轮询方式。
// 该io用2k以上电阻上拉到vcc，然后连接开关到GND，开关按下低电平。
// 本功能可以不使用，io口上拉电阻拉到高电平即可
//sbit BTN = P1^3;
//uchar isBtnPressing = 0;

// 方向按钮IO口定义（根据实际需要自行修改）
sbit BTN_U = P3^7;
sbit BTN_D = P1^2;
sbit BTN_L = P1^1;
sbit BTN_R = P1^0;
uchar isBtnUPressing = 0;
uchar isBtnDPressing = 0;
uchar isBtnLPressing = 0;
uchar isBtnRPressing = 0;

// 持续按键时，定时器代码会每隔指定间隔重置这个flg，
// 通知主程序调用处理函数
#define IS_NOT_KEEP_PRESSING		0
#define IS_KEEP_PRESSING			1
#define TIME_KEEP_PRESSING_TRIGGER	1000
uint ttKeepPressingU;
uint ttKeepPressingD;
uint ttKeepPressingL;
uint ttKeepPressingR;

void setBitData(uchar bitData){
	SCLK = 0;
	DIO =  bitData;
	SCLK = 1;
}

// 传输一个8位数
void setByte(uchar byteData){
	for (i = 0; i < 8; i++)
	{
		// 传入的数字从高位到低位依次判断是否为1，若为1则设置高电平，否则设置低电平
		// 判断的方法是先向左移位，把要判断的位移动到最高位然后跟0x80（1000 0000）相与，
		// 如果结果仍然是0x80（1000 0000）就表示最高位是1，否则最高位就是0
		if (((byteData<<i) & 0x80) == 0x80) {
			setBitData(1);
		} else {
			setBitData(0);
		}
	}
}

// 译码模式设置
#define DECODE_MODE_ALL_NOT_USE		0x00	// 所有数码管均不使用译码功能
#define DECODE_MODE_DIG0_ONLY		0x01 	// 只对DIG0号数码管进行译码，其他数码管不使用译码功能
#define DECODE_MODE_DIG0123_ONLY 	0x0f 	// 对DIG0-3号数码管进行译码，其他数码管不使用译码功能
#define DECODE_MODE_ALL_USE 		0xff	// 数码管7－0全部采用译码模式
void setDecodeMode(uchar mode) {
	// 指令寄存器地址设置：0xX9
	// D15-D12:任意
	// D11,10,9,8: 1,0,0,1
	setByte(0x09);
	setByte(mode);
	load();
}

// 亮度设置
// mode = 8
void setIntensity(uchar mode) {
	// 指令寄存器地址设置：0xXA
	// D15-D12:任意
	// D11,10,9,8: 1,0,1,0
	setByte(0x0A);
	
	// 亮度从0到15共16个等级，指令的D3－D0就是数字0－15的二进制编码
	// D7-D4:任意
	setByte(mode);

	load();
}
	
// 扫描显示位数设置(0-7)
void setScanLimit(uchar mode) {
	// 指令寄存器地址设置：0xXB
	// D15-D12:任意
	// D11,10,9,8: 1,0,1,1
	setByte(0x0B);
	
	// 扫描位数可设置0－7共8种选择，指令的D2－D0就是数字0－7的二进制编码
	// D7-D3:任意
	// D2-D0:0-7的3位二进制编码
	setByte(mode);

	load();
}

// 关断模式设置
#define SHUTDOWN_MODE_SHUTDOWN 	0x00 //关断模式
#define SHUTDOWN_MODE_NORMAL	0x01 //正常运行模式
void setShutdownMode(uchar mode) {

	// 指令寄存器地址设置：0xXC
	// D15-D12:任意
	// D11,10,9,8: 1,1,0,0
	setByte(0x0C);
	
	// 关断模式可设置0－1共2种选择，设置D0即可
	// D7-D1:任意
	// D0:1: 正常运行模式 0: 关断模式
	setByte(mode);

	load();
}

// 测试模式设置
#define DISPLAY_TEST_MODE_NORMAL 	0x00 //正常运行模式
#define DISPLAY_TEST_MODE_TEST		0x01 //测试模式(全亮模式)
void setDisplayTestMode(uchar mode) {

	// 指令寄存器地址设置：0xXF
	// D15-D12:任意
	// D11,10,9,8: 1,1,1,1
	setByte(0x0f);
	
	// 测试模式可设置0－1共2种选择，设置D0即可
	// D7-D1:任意
	// D0:0: 正常运行模式 1: 测试模式(全亮模式)
	setByte(mode);

	load();
}

void Timer0Init(void)		//1毫秒@5.5296MHz
{
	tt = 0;

	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xEA;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

	EA=1;
    ET0=1;
}

void init()
{
	setDecodeMode(DECODE_MODE_ALL_NOT_USE);			// 数码管7－0全部不采用译码模式
	setIntensity(intensity);						// 亮度(0-15)
	setScanLimit(7);								// 扫描显示位数(0-7)
	setShutdownMode(SHUTDOWN_MODE_NORMAL);			// 正常运行模式
	setDisplayTestMode(DISPLAY_TEST_MODE_NORMAL);	// 正常运行模式
}

main()
{
	intensity=0; // 默认亮度(0-15)

	waitTime=200;

	init();

	mg_restart();

	Timer0Init();

	while(1){
		checkBtn();
		display();
	}
}

void display() {
	uchar row;
	for (row= 0; row < 8; ++row)
	{
		setRowData(row+1, mg_getRowDataForShow(row));
	}
}

// 指定no(1-8)行显示指定内容
void setRowData(uchar row, uchar rowData) {

	// 设置指令寄存器地址：0xX1-0xX8
	// 格式：D15-D12:任意（我们这里设置0）
	//       D11-D8: 1-8的4位二进制编码：例：1（0,0,0,1）
	setByte(row);

	// 设置显示内容
	setByte(rowData);

	load();
}

void delay(uint tt){
	for (a = 0; a < tt; ++a)
	{
		for (b = 0; b < 100; ++b)
		{

			for (c = 0; c < 100; ++c)
			{
				// 进行亮度调整按钮的处理
				checkBtn();
			}
		}
	}
}

void load(){
	//_nop_();_nop_();_nop_();_nop_();
	LOAD = 1;
	//_nop_();_nop_();_nop_();_nop_();
	LOAD = 0;
	//_nop_();_nop_();_nop_();_nop_();
}

// 这里进行按钮的轮询
void checkBtn(){
	int tmp = 0;
	/*
	if (BTN == 0 && isBtnPressing == 0)
	{
		// 消除抖动
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN == 0) {
			isBtnPressing = 1;
			// 确认按钮被按下，调整亮度加一，到达最高亮度15再按下则回到最低亮度0
			intensity+=1;
			if (intensity > 7)
			{
				intensity = 0;
			}
			// 设置亮度
			setIntensity(intensity);
		}
	}

	if (BTN == 1 && isBtnPressing == 1)
	{
		isBtnPressing = 0;
	}
	*/

	// U
	if (BTN_U == 0 && isBtnUPressing == 0)
	{
		// 消除抖动
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_U == 0) {
			isBtnUPressing = 1;
			// 确认按钮被按下
			waitTime = mg_moveUp();
		}
	}

	if (isBtnUPressing == 1 && BTN_U == 1)
	{
		// 松开按钮
		isBtnUPressing = 0;

		// 清空按住不放的计数
		ttKeepPressingU = 0;

		//mg_UpRelease();
	}

	// D
	if (BTN_D == 0 && isBtnDPressing == 0)
	{
		// 消除抖动
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_D == 0) {
			isBtnDPressing = 1;
			// 确认按钮被按下
			waitTime = mg_moveDown();
		}
	}

	if (BTN_D == 1 && isBtnDPressing == 1)
	{
		isBtnDPressing = 0;

		// 清空按住不放的计数
		ttKeepPressingD = 0;
	}

	if (BTN_L == 0 && isBtnLPressing == 0)
	{
		// 消除抖动
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_L == 0) {
			isBtnLPressing = 1;
			// 确认按钮被按下
			waitTime = mg_moveLeft();
		}
	}

	if (BTN_L == 1 && isBtnLPressing == 1)
	{
		isBtnLPressing = 0;

		// 清空按住不放的计数
		ttKeepPressingL = 0;
	}

	// R
	if (BTN_R == 0 && isBtnRPressing == 0)
	{
		// 消除抖动
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_R == 0) {
			isBtnRPressing = 1;
			// 确认按钮被按下
			waitTime = mg_moveRight();
		}
	}

	if (BTN_R == 1 && isBtnRPressing == 1)
	{
		isBtnRPressing = 0;

		// 清空按住不放的计数
		ttKeepPressingR = 0;
	}
}

void timer0() interrupt 1
{
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xEA;		//设置定时初值

	// 如果前一次调用next函数的返回值为0，表示子程序要求主控停止继续调用Next函数
	// 那么tt变量不自加，也就是暂停next的处理。
	// 等待用户输入, 直到某次用户输入恢复waitTime变量
	if (waitTime != TIME_STOP)
	{
		tt++;

		if (tt >= waitTime)
		{
			tt=0;
			waitTime = mg_moveNext();
		}
	} else {
		tt=0;
	}

	
	
	// 监视按钮长按事件
	if (BTN_U == 0 && isBtnUPressing == 1)
	{
		ttKeepPressingU++;
		if (ttKeepPressingU >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingU = 0;
			waitTime = mg_KeepPressingU();
		}
	}

	if (BTN_D == 0 && isBtnDPressing == 1)
	{
		ttKeepPressingD++;
		if (ttKeepPressingD >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingD = 0;
			waitTime = mg_KeepPressingD();
		}
	}

	if (BTN_L == 0 && isBtnLPressing == 1)
	{
		ttKeepPressingL++;
		if (ttKeepPressingL >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingL = 0;
			waitTime = mg_KeepPressingL();
		}
	}

	if (BTN_R == 0 && isBtnRPressing == 1)
	{
		ttKeepPressingR++;
		if (ttKeepPressingR >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingR = 0;
			waitTime = mg_KeepPressingR();
		}
	}
}