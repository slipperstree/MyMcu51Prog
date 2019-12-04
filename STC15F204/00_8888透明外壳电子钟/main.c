/***************************************************/
/* �����ӳ���
/* ʵʱʱ�Ӳ���DS1302оƬ
/* �����ô��ڽ���Уʱ���Ա�������ESP8266ģ���������Уʱ��
/***************************************************/
//#include <string.h>

#include "header/STC15104E.h"
#include "header/display.h"
#include "header/key.h"
#include "header/common.h"
#include "header/ds1302.h"
#include "header/uart.h"
#include "header/sensorAdc.h"

int tt_getTime = 0;
int tt_refresh = 0;

// �����ϵ�ʱ��6����ʱ60��������͵�ƽ��
// ����8266��ͣ���������ʱ�����⡣
// ��Ϊ����ʱ�������ӵ�8266Ҫ������������è��ԼҪ5���Ӻ������������
// ��8266�����������ģ�һ�������ϣ�8266�ͻ�������ӡ���ʱ�Ӿʹ�������״̬��
int tt_P13_ms = 0;
int tt_P13_sec = 0;

main()
{
	//�ϵ�ʱ��6��P13���øߵ�ƽ
	P13 = 1;

	DS1302_init();
	UART_init();
	KEY_init();
	ADC_init();

	// ��������Ч��
	//DISPLAY_SetBreathMode(DISPLAY_BREATH_MODE_ON, DISPLAY_SPEED_LV_5);

	while(1){
		//�����ʱ̫���ᵼ�����ڶ�����
		//delay_ms(1);

		KEY_keyscan();

		//���ڷ������
		UART_SoftUartIsr();

		//��Լ1���Ӻ�6��P13���õ͵�ƽ(2000������ֿɵ�������С�ɼ���ʱ�䣬���߿�����ʱ��)
		if (tt_P13_sec < 60) tt_P13_ms++;
		if (tt_P13_ms >= 2000)
		{
			tt_P13_ms=0;
			tt_P13_sec++;
			if (tt_P13_sec>=60)
			{
				P13 = 0;
			}
			
		}
		

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
		if (tt_refresh >= 5)
		{
			tt_refresh = 0;

			//ʱ��Ҳȡ�ˣ�������ʾ������Ҳ�������ˣ����￪ʼˢ�»���
			//����ˢ�µ��߼���display.h�����нӿں�������ɣ��û����������䲻ͬ����ʾģ�飬����1602��LCD��
			DISPLAY_refreshDisplay();
		}
	}
}
