#include "header/common.h"
#include "header/uart.h"

main()
{
	// P32,P34 ǿ������� (P3M1.x=0 P3M0.x=1)
	P3M0 |= 0x24; // 00101000
	P3M1 &= 0xEB; // 11010111

	// �ϵ�Ĭ�� ָʾ�������󼶵�Դ���磨ONOFF PNP�����ܿ��� �͵�ƽ���磩
	LED = 1; ONOFF = 0;

	UART_init();

	while(1) {
		//���ڷ������
		UART_SoftUartIsr();
	}
}

