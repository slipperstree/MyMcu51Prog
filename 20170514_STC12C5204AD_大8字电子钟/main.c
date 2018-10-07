#include<STC12C52xx.h>
#include<DS1302.h>
//#include <reg51.h>
#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#define uchar unsigned char
#define uint unsigned int

#define TRUE 1
#define FALSE 0

uchar i;
int a, b, c;
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

sbit DIO = P1^0;	// 串行数据输入
sbit LOAD = P1^2;	// 锁存信号——上升沿有效
sbit SCLK = P1^1;	// 串行数据时钟信号————上升沿有效

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
	uchar YY, MM, DD, HH, MI, SS;

	uchar isShowDot = TRUE;

	uchar num1 = 0;
	uchar num2 = 1;
	uchar num3 = 2;
	uchar num4 = 3;
	intensity=3; // 默认亮度(0-15)

	// 下面的设置是推挽输出
	// PXM1：PXM0
	// 0     0  准双向口（传统IO）
	// 0     1  推挽输出（强上拉 ，电流可达20mA,尽量少用）
	// 1     0  仅为输入（高阻）
	// 1     1  开漏，如传统8051的P0口
	//P3M0 = 0xFF;
	//P3M1 = 0x00;

	init();

	// 年(20xx的xx部分)）（00-99）, 月, 日, 星期（1-7）,时, 分, 秒
	//ds1302_init(17, 12, 31, 7, 23, 59, 58);   // For Test
	//ds1302_init(17, 5, 14, 7, 13, 42, 30);

	while(1){
		setDecodeMode(DECODE_MODE_ALL_USE);

		ds1302_get_time();
		YY = ds1302_getYear();	
		MM = ds1302_getMonth();
		DD = ds1302_getDay();
		HH = ds1302_getHour();	//23(0010,0011) 35(0011,0101) -> 00(0000,0000) NG
		MI = ds1302_getMin();
		SS = ds1302_getSec();

		if (isShowDot == TRUE)
		{
			isShowDot = FALSE;
		} else {
			isShowDot = TRUE;
		}

		showDigit(1, HH/10, FALSE);
		showDigit(2, HH%10, isShowDot);
		showDigit(3, MI/10, FALSE);
		showDigit(4, MI%10, FALSE);

		//showDigit(1, 1, FALSE);
		//showDigit(2, 2, TRUE);
		//showDigit(3, 3, FALSE);
		//showDigit(4, 4, FALSE);

		delay(50);
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
				//checkBtn();
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