#include<STC12x20xx.h>
#include<asciiCode.h>
//#include<absacc.h>
//#include<intrins.h>
//#include<string.h>
//#include<stdlib.h>

#define uchar unsigned char
#define uint unsigned int

sbit LED01        =P3^7;
sbit LED02        =P3^4;
sbit LED03        =P3^5;
sbit LED04        =P1^7;
sbit LED05        =P1^6;
sbit LED06        =P1^5;
sbit LED07        =P1^4;
sbit LED08        =P1^3;
sbit LED09        =P1^2;
sbit LED10        =P1^1;
sbit LED11        =P1^0;

void delay(uint);
void init();
void displayLED(uint);
void displayOFF();

uint getChar(uchar, uchar);

uint now;
uint tt;
uint dataShow;

void delay(uint time){
	uint i,j,k;
	for(i=0;i<time;i++)
		for(j=0;j<50;j++)
			for(k=0;k<1;k++);
}

void init(){
	tt = 0;

	TMOD = 0x00;

    TH0 = (65536 - 1) / 256;
    TL0 = (65536 - 1) % 256;

    IT0=1;	// 外部中断0设置为下降沿触发(0为低电平触发)
    EX0=1;	// 允许外部中断0

    IT1=1;	// 外部中断1设置为下降沿触发(0为低电平触发)
    EX1=1;	// 允许外部中断1

    EA=1;
    ET0=1;	// 定时器0有效
    TR0=1;	// 定时器0开始运行
}

main(){

	init();

    while(1){
    	//displayLED(dataShow);
	}
}

void displayOFF(){
	LED01 = 1;
	LED02 = 1;
	LED03 = 1;
	LED04 = 1;
	LED05 = 1;
	LED06 = 1;
	LED07 = 1;
	LED08 = 1;
	LED09 = 1;
	LED10 = 1;
	LED11 = 1;
}

void displayLED(uint dat){
	int i = 0;

	displayOFF();
	LED01 = (dat<<i++) & 0x0400; // 0x0400 : 10000000000
	//displayOFF();
	LED02 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED03 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED04 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED05 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED06 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED07 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED08 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED09 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED10 = (dat<<i++) & 0x0400;
	//displayOFF();
	LED11 = (dat<<i++) & 0x0400;
}

void timer0() interrupt 1
{

}

uint getChar(uchar ch, uchar offset) {
	return ~nAsciiDot[((ch - 0x20) * 5) + offset - 1];
}

void showChar(uint ch) {
	uint colidx = 0;
	for (colidx=0; colidx<= 5; colidx++)
    {
    	displayLED(getChar(ch, colidx));
		delay(10);
    }
	displayLED(0xfff);
	delay(20);
}

// 外部中断INT0
void exint0() interrupt 0
{
	EX0 = 0; //避免向右挥动时多次触发中断0，关闭中断，直到从右往左挥动时触发了右侧的中断1为止
	
	displayLED(0xfff);
	delay(30);

	showChar('C');
	showChar('H');
	showChar('E');
	showChar('N');


	EX0 = 1;
}

// 外部中断INT1
void exint1() interrupt 2
{
	//TR0 = 0; //停用定时器
	//dataShow = 0xfff; //从右往左挥动时，关闭输出
	//EX0 = 1; //中断0有效
}