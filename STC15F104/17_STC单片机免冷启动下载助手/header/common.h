#ifndef __COMMON_H_
#define __COMMON_H_

#include "STC15104E.h"
#include <intrins.h>

sbit ONOFF = P3^4;
sbit LED = P3^2;

#define uchar unsigned char
#define uint unsigned int
#define static_idata_uchar static idata unsigned char
#define static_idata_uint static idata unsigned int
#define static_idata_int static idata int

// 精确延时用
#define delay1NOP()	    _nop_();
#define delay2NOP()	    delay1NOP();_nop_();
#define delay3NOP()	    delay2NOP();_nop_();
#define	delay4NOP()	    delay3NOP();_nop_();
#define	delay5NOP()	    delay4NOP();_nop_();
#define	delay6NOP()	    delay5NOP();_nop_();
#define	delay7NOP()	    delay6NOP();_nop_();
#define	delay8NOP()	    delay7NOP();_nop_();
#define	delay9NOP()	    delay8NOP();_nop_();
#define	delay10NOP()	delay9NOP();_nop_();
#define	delay11NOP()	delay10NOP();_nop_();
#define	delay12NOP()	delay11NOP();_nop_();
#define	delay13NOP()	delay12NOP();_nop_();
#define	delay14NOP()	delay13NOP();_nop_();
#define	delay15NOP()	delay14NOP();_nop_();

void delay_ms(unsigned char);
void Delay100ms_STC15Fxx_110592M(uchar n);
unsigned char My_strlen(char* x);

#endif