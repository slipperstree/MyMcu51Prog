#include<STC12C52xx.h>

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