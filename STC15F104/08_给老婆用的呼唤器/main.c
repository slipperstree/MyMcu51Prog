#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>
#define uchar unsigned char
#define uint unsigned int

sbit pLED        =P3^1;         //
sbit pBuz        =P3^5;         //
sbit pInp        =P3^0;         //
sbit pBtn        =P3^2;

void doBeep();
void doBtnPressed();
void delay(uint t);


void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<250;j++);
}
						 
int beepCnt=0;

#define TIME_LONG_BEEP 500
#define TIME_SHORT_BEEP 200
#define TIME_INTERVAL   100
#define TIME_INTERVAL_END  800
#define BEEP_CNT         2

uchar isBeeping=0;
uchar isBtnPressed=0;

void checkBtn(){
	if(pBtn == 0 && isBtnPressed == 0){
	 	delay(100); // 防抖
		if(pBtn == 0){
			// 确定按钮按下
			isBtnPressed = 1;
			doBtnPressed();
		}
	}
	
	if(isBtnPressed == 1 && pBtn == 1){
		isBtnPressed = 0;
	}
}

void doBtnPressed(){
	//beepCnt = 0;
	//pLED = 1;
	//isFlashInterval = 1;
	//nowLedFlashCnt = 0;
	//tt = 0;
	IAP_CONTR = 0x20; // reset
}

void doBeep(){
	uchar i=0;
	
	isBeeping = 1;
	beepCnt+=2; // 闪烁LED用，每次鸣响后计数加2，也就是多一次LED的闪烁
	
	for (i=0; i<BEEP_CNT; i++){
		pBuz=0;
		delay(TIME_SHORT_BEEP);
		pBuz=1;
		delay(TIME_INTERVAL);
		
		pBuz=0;
		delay(TIME_SHORT_BEEP);
		pBuz=1;
		delay(TIME_INTERVAL);
		
		pBuz=0;
		delay(TIME_SHORT_BEEP);
		pBuz=1;
		delay(TIME_INTERVAL);
		
		pBuz=0;
		delay(TIME_LONG_BEEP);
		pBuz=1;
		delay(TIME_INTERVAL_END);
	}
	
	isBeeping = 0;
}

main()
{
	uchar i=0;
	
	TMOD = 0x01;  //定时0，工作在方式1
    TH0  = (65536-1000)/256;
    TL0  = (65536-1000)/256;
    TR0  = 1;     //启动计数
    EA   = 1;     //开总中断
    ET0  = 1;     //开定时器0中断
	
	pInp = 1;
	pBuz = 1;
	pLED = 1;

    while(1){
		//pInp = 1;
		if(pInp == 0 && isBeeping == 0){
			delay(10); // 防抖
			if(pInp == 0){
				// 确定是低电平信号
				doBeep();
			}
		}
		
		checkBtn();
	}
}

uchar nowLedFlashCnt=0;
int tt;
uchar isFlashInterval;
void timer0() interrupt 1
{
	tt++;
	TH0  = (65536-1000)/256;
    TL0  = (65536-1000)/256;
	
 	if(isFlashInterval==1 && tt >= 1500){
		tt=0;
		isFlashInterval = 0;
	}
	
	if(isFlashInterval==0 && tt >= 300){
		tt = 0;
		if(nowLedFlashCnt < beepCnt){
			pLED ^= 1;
			nowLedFlashCnt++;
		} else if (nowLedFlashCnt == beepCnt){
			isFlashInterval=1;
			nowLedFlashCnt=0;
		}
	}
	
}