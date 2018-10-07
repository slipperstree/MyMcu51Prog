/***************************************************/
/* 洞洞板_湿温度计_MAX7219驱动  */
/*  */
/***************************************************/     
//#include<STC15104E.h>
#include<STC15F104W.h>
//#include <reg51.h>
#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#include <readDHT11.h>

#define uchar unsigned char
#define uint unsigned int

#define TRUE 1
#define FALSE 0

uchar i;
int n;
int a, b, c;
int myWendu, myShidu;
int numForShow;
int intensity; //亮度0-15

void init();
void display(int);

void setBitData(uchar);
void setByte(uchar);

void setDecodeMode(uchar);
void setIntensity(uchar);
void setScanLimit(uchar);
void setShutdownMode(uchar);
void setDisplayTestMode(uchar);
void showDigit(uchar, uchar, uchar);

void delay(int);
void load();

void flashMax7219(int);
void checkBtn();
int getNumForShow();

sbit DIO = P3^5;	// 串行数据输入
sbit LOAD = P3^4;	// 锁存信号——上升沿有效
sbit SCLK = P3^3;	// 串行数据时钟信号————上升沿有效

// 三极管供电开关（本功能可以不使用，io口悬空即可，如要做其他用途则要修改代码将相关代码删掉）
// 作用是如果有时候碰上干扰太大无法正常送数据，则每次只在点亮前送一次数据，送完以后点亮，一旦点亮就无法正确送数据了。
// 然后定时通过此io口通过PNP型三极管断开MAX7219的电再通电让其复位，复位默认不点亮，然后再送下一次的数据，如此循环往复
sbit SWITCH = P3^1;

// 调亮度按钮（不使用外部中断，因为104E系列不支持外部中断，104W是支持，但为了通用，使用轮询方式。
// 该io用2k以上电阻上拉到vcc，然后连接开关到GND，开关按下低电平。
// 本功能可以不使用，io口上拉电阻拉到高电平即可
sbit BTN = P3^0;
uchar isBtnPressing = 0;

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

// 直接设置指定地址
void write_MAX7219(uchar address,uchar date)
{
        setByte(address);
        setByte(date);
        load();
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

// 指定no(1-8)号数码管显示数字num(0-9)
// 第三个参数是显示不显示小数点（TRUE:显示/FALSE:不显示）
void showDigit(uchar no, uchar num, uchar showDotPoint) {

	// 设置指令寄存器地址：0xX1-0xX8
	// 格式：D15-D12:任意（我们这里设置0）
	//       D11-D8: 1-8的4位二进制编码：例：1（0,0,0,1）
	setByte(no);

	// 设置显示内容
	// 格式：D7:显示小数点（1点亮）
	//       D6-D4:任意（我们这里设置0）
	//       D3-D0:数字0-9的4位二进制编码：例：2（0,0,1,0）
	if (showDotPoint == TRUE) {
		// 如果显示小数点则需要将数字的最高位（D7）设置为1(最高位跟1相或)
		setByte(num | 0x80);
	} else {
		setByte(num);
	}

	load();
}

void init()
{
	setDecodeMode(DECODE_MODE_ALL_USE);				// 数码管7－0全部采用译码模式
	setIntensity(intensity);						// 亮度(0-15)
	setScanLimit(3);								// 扫描显示位数(0-7)
	setShutdownMode(SHUTDOWN_MODE_NORMAL);			// 正常运行模式
	setDisplayTestMode(DISPLAY_TEST_MODE_NORMAL);	// 正常运行模式
}

main()
{
	int test = 0;
	n=0;
	intensity=0; // 默认亮度(0-15)

	// 下面的设置是推挽输出
	// PXM1：PXM0
	// 0     0  准双向口（传统IO）
	// 0     1  推挽输出（强上拉 ，电流可达20mA,尽量少用）
	// 1     0  仅为输入（高阻）
	// 1     1  开漏，如传统8051的P0口
	P3M0 = 0xFF;
	P3M1 = 0x00;

//test------------------------------
//init();
// display(5552);init();
// display(5889);
//display(1555);
//setShutdownMode(SHUTDOWN_MODE_NORMAL);			// 正常运行模式
//delay(10);
//SWITCH = 1;
//test------------------------------

	numForShow = getNumForShow();
	while(1){
		flashMax7219(1000);
// init();
// display(test++);
// delay(100);
	}
}

void display(int show) {
	showDigit(4, show%10, FALSE);
	showDigit(3, show/10%10, FALSE);
	showDigit(2, show/100%10, TRUE);
	showDigit(1, show/1000%10, FALSE);
}

void delay(int tt){
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

void flashMax7219(int time)
{
	SWITCH = 0; // ON 三极管导通 开始给MAX7219供电
	display(numForShow);  init();
	delay(time);

	numForShow = getNumForShow();

	SWITCH = 1; // ON 三极管截止 MAX7219断电
	display(numForShow);  init(); // 这里让MAX7219随便做点什么，消耗掉剩余电量即可（剩余电量来源不明）
	delay(1);// 这个延时太短有可能数码管还没有完全关闭，导致后面写数据时电平不够失败，太长则会有明显的闪烁或停顿（数码管关闭时间太长）
}

int getNumForShow(){
	uchar ret = 0;

	ret = readDHT11();
	switch (ret) {
		case READ_DHT11_RET_SUCCESS:
			myWendu = getWendu();
			myShidu = getShidu();
			numForShow = myWendu*100 + myShidu;
			break;
		case READ_DHT11_RET_TIMEOUT:
			numForShow = 9999;
			break;
		case READ_DHT11_RET_CHECKESUM_WRONG:
			numForShow = 5555;
			break;
		default:
			numForShow = ret;
			break;
	}

	return numForShow;
}

// 这里进行按钮的轮询
void checkBtn(){
	int tmp = 0;
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
}