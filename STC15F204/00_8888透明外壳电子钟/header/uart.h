#ifndef __UART_H_
#define __UART_H_

void UART_INIT();

/*********************************************************
	���������������Ҫ��main������ѭ�����ã�
*********************************************************/
void UART_SoftUartIsr();

//���ڷ����ַ���
void UART_SendString(unsigned char *s);

#endif
