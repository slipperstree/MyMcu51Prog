#ifndef __UART_H_
#define __UART_H_

void UART_init();

/*********************************************************
	���������������Ҫ��main������ѭ�����ã�
*********************************************************/
void UART_SoftUartIsr();

//���ڷ���һ���ֽ�
void UART_SendByte(unsigned char dat);

//���ڷ����ַ���
void UART_SendString(unsigned char *s);

#endif
