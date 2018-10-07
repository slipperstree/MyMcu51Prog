#include "reg51.h"

#ifndef __TXS_DS1302_H_
#define __TXS_DS1302_H_

#define uint unsigned int 
#define uchar unsigned char 

/********�����Ǻ�������********/
void  write_byte(uchar inbyte);		//дһ�ֽ����ݺ�������
uchar read_byte();    				//��һ�ֽ����ݺ�������
void  write_ds1302(uchar cmd,uchar indata);	//дDS1302��������
uchar read_ds1302(uchar addr);			//��DS1302��������
void  set_ds1302(uchar addr,uchar *p,uchar n);	//����DS1302��ʼʱ�亯������
void  get_ds1302(uchar addr,uchar *p,uchar n);	//����ǰʱ�亯������

//DS1302��ʼ����������
void init_ds1302(uchar pYear, uchar pMonth, uchar pDay, 
				 uchar pWeek,
				 uchar pHour, uchar pMin, uchar pSec);

void get_time();
uchar getYear();
uchar getMonth();
uchar getDay();
uchar getHour();
uchar getMin();
uchar getSec();
uchar* getStirngYYYYMMDDHHMMSS();

#endif