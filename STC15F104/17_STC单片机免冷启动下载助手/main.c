#include "header/common.h"
#include "header/uart.h"

main()
{
	// P32,P34 强推挽输出 (P3M1.x=0 P3M0.x=1)
	P3M0 |= 0x24; // 00101000
	P3M1 &= 0xEB; // 11010111

	// 上电默认 指示灯亮，后级电源给电（ONOFF PNP三极管开关 低电平给电）
	LED = 1; ONOFF = 0;

	UART_init();

	while(1) {
		//软串口服务程序
		UART_SoftUartIsr();
	}
}

