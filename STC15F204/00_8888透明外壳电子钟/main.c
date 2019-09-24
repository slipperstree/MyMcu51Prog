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

main()
{
	DS1302_init();
	UART_init();
	KEY_init();
	ADC_init();

	// ��������Ч��
	// DISPLAY_SetBreathMode(DISPLAY_BREATH_MODE_ON, DISPLAY_SPEED_LV_5);

	while(1){
		//�����ʱ̫���ᵼ�����ڶ�����
		//delay_ms(1);

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
		if (tt_refresh >= 5)
		{
			tt_refresh = 0;

			//ʱ��Ҳȡ�ˣ�������ʾ������Ҳ�������ˣ����￪ʼˢ�»���
			//����ˢ�µ��߼���display.h�����нӿں�������ɣ��û����������䲻ͬ����ʾģ�飬����1602��LCD��
			DISPLAY_refreshDisplay();
		}
	}
}
