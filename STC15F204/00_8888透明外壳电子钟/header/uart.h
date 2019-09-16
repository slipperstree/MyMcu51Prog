#ifndef __UART_H_
#define __UART_H_

void UART_INIT();

/*********************************************************
	串行软服务函数（需要在main函数里循环调用）
*********************************************************/
void UART_SoftUartIsr();

//串口发送字符串
void UART_SendString(unsigned char *s);

#endif
