/***************************************************/
/* �����ӳ���
/* ʵʱʱ�Ӳ���DS1302оƬ
/* �����ô��ڽ���Уʱ���Ա�������ESP8266ģ���������Уʱ��
/***************************************************/     
#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#include "header/STC15104E.h"
#include "header/display.h"
#include "header/key.h"
#include "header/common.h"
#include "header/ds1302.h"
#include "header/uart.h"

int tt_getTime = 0;
int tt_refresh = 0;

/*************��ʱ��0��ʼ������***************/
// void timer0_init()
// {
// 	TMOD |= 0X21;	  //��ʱ��1������ʽ1
// 	ET0  = 1;		  //����ʱ��0�ж�

// 	TH0=0xfe;    //��ʱ10ms�ж�һ��
// 	TL0=0x0c;	 //500us

// 	EA   = 1;	 	  //�����ж�
// 	TR0  = 1;		  //����ʱ��0��ʱ
// }

main()
{
	uchar nowPos = 1;

	//timer0_init();
	DS1302_init();
	UART_init();

	// ����������ʱ��(2019/09/14 15:00:00)
	// WriteTime_Sec(0);	//��
	// WriteTime_Min(0);  	//��
    // WriteTime_Hour(15);  //ʱ
	// WriteTime_Day(14);  	//��
	// WriteTime_Month(9);  //��
	// WriteTime_Month(19);  //��

	while(1){
		//�����ʱ̫���ᵼ�����ڶ�����
		delay_ms(1);

		KEY_keyscan();

		//���ڷ������
		UART_SoftUartIsr();

		//��ȡ��ǰʱ��
		tt_getTime++;
		if (tt_getTime >= 300)
		{
			tt_getTime = 0;

			DS1302_GetTimeFromDS1302();

			//���������GetTimeȡ��������(������DS302.h�����ȫ�ֱ���shi fen miao nian yue ri��)������ʾ�õ�����
			//�ô����������ڴ��������(������display.h��� d1 d2 d3 d4)������ˢ�»������ں����showPosition���������ģ�
			DISPLAY_updateDisplay();
		}

		// ˢ�»��� ----------------
		tt_refresh++;
		if (tt_refresh >= 10)
		{
			tt_refresh = 0;

			//ʱ��Ҳȡ�ˣ�������ʾ������Ҳ�������ˣ����￪ʼˢ�»���
			//����ˢ�µ��߼���display.h�����нӿں�������ɣ��û����������䲻ͬ����ʾģ�飬����1602��LCD��
			DISPLAY_refreshDisplay();
		}
	}
}

//void Timer0() interrupt 1  //���ö�ʱ��0
// void Timer0()
// {
// 	TH0=0xfe;    //��ʱ10ms�ж�һ��
// 	TL0=0x0c;	 //500us
// }
