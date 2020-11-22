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

void delay_ms(unsigned char);
void Delay100ms_STC15Fxx_110592M(uchar n);
unsigned char My_strlen(char* x);

#endif