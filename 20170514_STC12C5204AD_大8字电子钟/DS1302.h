#include<STC12C52xx.h>

#ifndef __TXS_DS1302_H_
#define __TXS_DS1302_H_

#define uint unsigned int 
#define uchar unsigned char 

/********以下是函数声明********/
void  write_byte(uchar inbyte);		//写一字节数据函数声明
uchar read_byte();    				//读一字节数据函数声明
void  write_ds1302(uchar cmd,uchar indata);	//写DS1302函数声明
uchar read_ds1302(uchar addr);			//读DS1302函数声明
void  set_ds1302(uchar addr,uchar *p,uchar n);	//设置DS1302初始时间函数声明
void  get_ds1302(uchar addr,uchar *p,uchar n);	//读当前时间函数声明

//DS1302初始化函数声明
void ds1302_init(uchar pYear, uchar pMonth, uchar pDay, 
				 uchar pWeek,
				 uchar pHour, uchar pMin, uchar pSec);

void ds1302_get_time();
uchar ds1302_getYear();
uchar ds1302_getMonth();
uchar ds1302_getDay();
uchar ds1302_getHour();
uchar ds1302_getMin();
uchar ds1302_getSec();
uchar* ds1302_getStirngYYYYMMDDHHMMSS();

#endif