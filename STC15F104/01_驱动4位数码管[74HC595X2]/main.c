/***************************************************/
/* 测试STC15F104 74HC595 x 2 驱动4位数码管[74HC595X2] */
/*  */
/***************************************************/     
#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>

#define uchar unsigned char
#define uint unsigned int

uint i;
uint tt;
uint now;
uint isBtnPushing;

void init(void);
void setBitData(uchar);
void showDigit(uchar, uchar, uchar);


sbit DIO = P0^5;	// 串行数据输入
sbit RCLK = P0^6;	// 锁存信号——上升沿有效
sbit SCLK = P0^7;	// 串行数据时钟信号————上升沿有效

sbit btn = P3^4;	// 按钮默认用上拉电阻接到VCC拉低，按钮按下后下拉电阻接到GND拉低

void setBitData(uchar bitData){
	DIO =  bitData;
	SCLK = 0;
	SCLK = 1;
}

// 指定no(1-4)号数码管显示数字num(0-9)，第三个参数是显示不显示小数点（1/0）
void showDigit(uchar no, uchar num, uchar showDotPoint){
	if (num == 0) {
		setBitData(showDotPoint);
		setBitData(1)           ;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
	}
	else if (num == 1) {
		setBitData(showDotPoint) ;
		setBitData(1)            ;
		setBitData(1)            ;
		setBitData(1)            ;
		setBitData(1)            ;
		setBitData(0)            ;
		setBitData(0)            ;
		setBitData(1)            ;
	}
	else if (num == 2) {
		setBitData(showDotPoint) ;
		setBitData(0)                ;
		setBitData(1)                ;
		setBitData(0)                ;
		setBitData(0)                ;
		setBitData(1)                ;
		setBitData(0)                ;
		setBitData(0)                ;
	}
	else if (num == 3) {
		setBitData(showDotPoint)     ;
		setBitData(0)                ;
		setBitData(1)                ;
		setBitData(1)                ;
		setBitData(0)                ;
		setBitData(0)                ;
		setBitData(0)                ;
		setBitData(0)                ;
	}
	else if (num == 4) {
		setBitData(showDotPoint);
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(1)           ;
		setBitData(1)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(1)           ;
	}
	else if (num == 5) {
		setBitData(showDotPoint);
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(1)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(1)           ;
		setBitData(0)           ;
	}
	else if (num == 6) {
		setBitData(showDotPoint);
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(1)           ;
		setBitData(0)           ;
	}
	else if (num == 7) {
		setBitData(showDotPoint);
		setBitData(1)           ;
		setBitData(1)           ;
		setBitData(1)           ;
		setBitData(1)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
	}
	else if (num == 8) {
		setBitData(showDotPoint);
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
		setBitData(0)           ;
	}
	else if (num == 9) {
		setBitData(showDotPoint) ;
		setBitData(0)            ;
		setBitData(0)            ;
		setBitData(1)            ;
		setBitData(0)            ;
		setBitData(0)            ;
		setBitData(0)            ;
		setBitData(0)            ;
	}

	if (no == 1) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
		setBitData(0);
		setBitData(0);
		setBitData(0);
	}
	else if (no == 2) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
		setBitData(0);
		setBitData(0);
	}
	else if (no == 3) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
		setBitData(0);
	}
	else if (no == 4) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
	}
	
	RCLK = 1;
	RCLK = 0;
}

void init(void)
{
    TMOD = 0x00; 

    TH0 = (65536 - 1000) / 256;
    TL0 = (65536 - 1000) % 256;
    
    EA=1;
    ET0=1;
    TR0=1;

    RCLK = 0;
	SCLK = 0;
    
    now = 0;
    isBtnPushing = 0;
}

main()
{
	init();
	while(1){
		//btn = 0; // 取状态之前先设置一下状态
		if (isBtnPushing == 0 && btn == 0)
		{
			// 防抖动，延时一小会
			for (i = 0; i < 10; ++i){;}
				if (isBtnPushing == 0 && btn == 0) {
					// 判定按钮被按下，暂停/恢复计时器
					isBtnPushing = 1; // 按钮为按下的状态++++
					if (TR0 == 1)
					{
						TR0 = 0;
					} else{
						TR0 = 1;
					}
				}
		}

		if (isBtnPushing == 1 && btn == 1){
			// 判定按钮被放开
			isBtnPushing = 0;
		}

		showDigit(1, now / 1000, 1);
		showDigit(2, now % 1000 / 100, 1);
		showDigit(3, now % 100 / 10, 1);
		showDigit(4, now % 10, 1);
	}
}

void timer0() interrupt 1
{
    tt++;
    if (tt >= 50) {
    	tt = 0;
    	now++;
    	if (now >= 10000)
    	{
    		now = 0;
    	}
    }
}