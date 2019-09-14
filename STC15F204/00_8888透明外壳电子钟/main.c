/***************************************************/

/* ���Ե�Ƭ�� STC15F100ϵ�е�Ƭ�� ��ˮ�Ʋ��Գ��� */ 

/***************************************************/     
#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>

#include "DS1302.h"
#include "display.h"

#define uchar unsigned char
#define uint unsigned int

sbit p30 = P3^0;         //
sbit p31 = P3^1;         //

int tt = 0;
						 
/*************��ʱ��0��ʼ������***************/
void timer0_init()
{
	TMOD |= 0X21;	  //��ʱ��1������ʽ1
	ET0  = 1;		  //����ʱ��0�ж�

	TH0=0xfe;    //��ʱ10ms�ж�һ��
	TL0=0x0c;	 //500us

	EA   = 1;	 	  //�����ж�
	TR0  = 1;		  //����ʱ��0��ʱ
}

main()
{
	timer0_init();
	DS1302_init();

	//����������ʱ��
	// WriteTime(0x80,( 0 /10)<<4|( 0 %10));	 //��
	// WriteTime(0x82,( 5 /10)<<4|( 5 %10));  //��
    // WriteTime(0x84,( 15 /10)<<4|( 15 %10));  //ʱ
	// WriteTime(0x86,( 14 /10)<<4|( 14 %10));  //��
	// WriteTime(0x88,( 9 /10)<<4|( 9 %10));  //��
	// WriteTime(0x8c,( 19 /10)<<4|( 19 %10));  //��

	while(1){

		delay_ms(300);

		GetTime();          //��ȡ��ǰʱ��
		updateDisplay();	//������ʾ�õ����ݣ��ô����������ڴ�������ݣ�����ˢ�»������ٶ�ʱ�����������ģ�
	}
}

uchar nowPos = 1;
void Timer0() interrupt 1  //���ö�ʱ��0
{
	TH0=0xfe;    //��ʱ10ms�ж�һ��
	TL0=0x0c;	 //500us

	nowPos++;
	if (nowPos==5)
	{
		nowPos=1;
	}

	showPosition(nowPos);
}

// ************ DS1302 ************************************************************
