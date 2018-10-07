/***************************************************/
/* 摇摇棒 74HC595  */
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
uint ttmode;
uint tt;
uint mode;
uint now;
uint isBtnPushing;

void init(void);
void setBitData(uchar);
void showDigit(uchar, uchar, uchar);


sbit DIO = P3^0;	// 串行数据输入
sbit RCLK = P3^1;	// 锁存信号——上升沿有效
sbit SCLK = P3^2;	// 串行数据时钟信号————上升沿有效

sbit btn = P3^4;	// 按钮默认用上拉电阻接到VCC拉低，按钮按下后下拉电阻接到GND拉低

void setBitData(uchar bitData){
	DIO =  bitData;
	SCLK = 0;
	SCLK = 1;
}


void showTriangle(uint n){
	setBitData(0)           ;
	setBitData(0)           ;
	setBitData(0)       	;
	
	if(n == 0)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	} 
	else if (n == 1)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	}
	else if (n == 2)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 3)
	{
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 4)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 5)
	{
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 6)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 7)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	}
	else if (n == 8)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}

	
	RCLK = 1;
	RCLK = 0;
}

void showRect(uint n){
	setBitData(0)           ;
	setBitData(0)           ;
	setBitData(0)       	;
	
	if(n == 0)
	{
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	} 
	else if (n == 1)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 2)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 3)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 4)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 5)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 6)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 7)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 8)
	{
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	}

	
	RCLK = 1;
	RCLK = 0;
}

void showCross(uint n){
	setBitData(0)           ;
	setBitData(0)           ;
	setBitData(0)       	;
	
	if(n == 0)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	} 
	else if (n == 1)
	{
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	}
	else if (n == 2)
	{
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(0)       	;
	}
	else if (n == 3)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
	}
	else if (n == 4)
	{
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
	}
	else if (n == 5)
	{
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(0)       	;
	}
	else if (n == 6)
	{
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	}
	else if (n == 7)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
	}
	else if (n == 8)
	{
		setBitData(1)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(0)       	;
		setBitData(1)       	;
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
    mode = 0;
    isBtnPushing = 0;
}

main()
{
	init();
	while(1){
		
	}
}

void timer0() interrupt 1
{
    tt++;
    ttmode++;
    if (ttmode >= 2000)
    {
    	ttmode = 0;
    	mode++;
    	if (mode >= 3)
    	{
    		mode = 0;
    	}

    	now = 0;
    	tt=0;
    }

    if (tt >= 5) {
    	tt = 0;
    	
		if (mode == 0) showTriangle(now);
		if (mode == 1) showRect(now);
		if (mode == 2) showCross(now);

		now++;
    	if (now >= 9)
    	{
    		now = 0;
    	}
    }
}