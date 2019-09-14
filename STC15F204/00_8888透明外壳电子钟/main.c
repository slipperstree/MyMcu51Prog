/***************************************************/
/* �����ӳ���
/* ʵʱʱ�Ӳ���DS1302оƬ
/* �����ô��ڽ���Уʱ���Ա�������ESP8266ģ���������Уʱ��
/***************************************************/     
#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#include "STC15104E.h"
#include "DS1302.h"
#include "display.h"
#include "uart.h"

#define uchar unsigned char
#define uint unsigned int

int tt_getTime = 0;
int tt_refresh = 0;

void delay(unsigned int n)
{
	unsigned int x,y;
	for(x=n;x>0;x--)
	{
		for(y=110;y>0;y--);	
	}
}

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
	uchar nowPos = 1;

	//timer0_init();
	DS1302_init();
	uart_init();

	// ����������ʱ��(2019/09/14 15:00:00)
	// WriteTime_Sec(0);	//��
	// WriteTime_Min(0);  	//��
    // WriteTime_Hour(15);  //ʱ
	// WriteTime_Day(14);  	//��
	// WriteTime_Month(9);  //��
	// WriteTime_Month(19);  //��

	while(1){
		//�����ʱ̫���ᵼ�����ڶ�����
		delay(1);

		//���ڷ������
		Soft_Uart_Isr();

		//��ȡ��ǰʱ��
		tt_getTime++;
		if (tt_getTime >= 300)
		{
			tt_getTime = 0;

			GetTime();

			//���������GetTimeȡ��������(������DS302.h�����ȫ�ֱ���shi fen miao nian yue ri��)������ʾ�õ�����
			//�ô����������ڴ��������(������display.h��� d1 d2 d3 d4)������ˢ�»������ں����showPosition���������ģ�
			updateDisplay();
		}

		// ˢ�»��� ----------------
		tt_refresh++;
		if (tt_refresh >= 10)
		{
			tt_refresh = 0;
			
			nowPos++;
			if (nowPos==5)
			{
				nowPos=1;
			}
			showPosition(nowPos);
		}
	}
}

//void Timer0() interrupt 1  //���ö�ʱ��0
void Timer0()
{
	TH0=0xfe;    //��ʱ10ms�ж�һ��
	TL0=0x0c;	 //500us

	// nowPos++;
	// if (nowPos==5)
	// {
	// 	nowPos=1;
	// }

	// showPosition(nowPos);
}
