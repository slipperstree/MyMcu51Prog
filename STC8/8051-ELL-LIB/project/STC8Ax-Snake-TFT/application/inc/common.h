#ifndef __COMMON_H_
#define __COMMON_H_

#include "main.h"

#define uchar unsigned char
#define uint unsigned int
#define static_idata_uchar static idata unsigned char
#define static_idata_uint static idata unsigned int
#define static_idata_int static idata int

void My_delay_ms(unsigned int n);
unsigned char My_strlen(char* x);

#endif