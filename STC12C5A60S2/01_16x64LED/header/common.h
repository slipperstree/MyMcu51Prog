#ifndef __COMMON_H_
#define __COMMON_H_

#include <STC89C52.h>

#define NOP() _nop_()

#define uchar unsigned char
#define uint unsigned int
#define static_idata_uchar static idata unsigned char
#define static_idata_uint static idata unsigned int
#define static_idata_int static idata int

void delay_ms(unsigned char);
unsigned char My_strlen(char* x);

#endif