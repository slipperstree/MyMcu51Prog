#include <DS1302.h>
#include <stdlib.h>

#define uchar unsigned char
#define uint  unsigned int

sbit ds1302_reset = P3^4;
sbit ds1302_sclk  = P1^7;
sbit ds1302_io    = P1^6;

uchar ds1302_disp_buf[20] ={0x00}; //������ʾ������
uchar ds1302_temp [2]={0};			//�����������ʱ��Сʱ�����ӵ��м�ֵ

uchar ds1302_sec,ds1302_min,ds1302_hour,ds1302_ri,ds1302_yue,ds1302_nian;					//�����롢�ֺ�Сʱ����

/********������дһ�ֽ����ݺ���********/
void write_byte(uchar inbyte)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		ds1302_sclk=0;             //дʱ�͵�ƽ�ı�����

		if(inbyte&0x01)
			ds1302_io=1;
		else
			ds1302_io=0;

		ds1302_sclk=1;            //�ߵ�ƽ������д��DS1302

		inbyte=inbyte>>1;
	}
}

/********�����Ƕ�һ�ֽ����ݺ���********/
uchar read_byte()    
{
	uchar i,ds1302_temp=0; 
	ds1302_io=1;
	for(i=0;i<7;i++)
	{
		ds1302_sclk=0;
		if(ds1302_io==1)
			ds1302_temp=ds1302_temp|0x80;
		else
			ds1302_temp=ds1302_temp&0x7f;

		ds1302_sclk=1;             //����������
		ds1302_temp=ds1302_temp>>1;
	}

	ds1302_io=0;
	return (ds1302_temp);
}

/********дDS1302����, ��DS1302��ĳ����ַд������ ********/
void write_ds1302(uchar cmd,uchar indata)
{
	ds1302_sclk=0;
	ds1302_reset=1;
	write_byte(cmd);        
	write_byte(indata);
	ds1302_sclk=0;
	ds1302_reset=0;
}

/********��DS1302����,��DS1302ĳ��ַ�ĵ�����********/
uchar read_ds1302(uchar addr)
{
	uchar backdata;
	ds1302_sclk=0;
	ds1302_reset=1;
	write_byte(addr);      		//��д��ַ
	backdata=read_byte();  		//Ȼ�������
	ds1302_sclk=0;
	ds1302_reset=0;
	return (backdata);
}

// ����ͨ����ת��DS1302�����Ĵ�����Ҫ�ĸ�ʽ
// ʮλ�����ڸ�4λ�ϣ���λ�����ڵ�4λ��
uchar dec2DS1302Format(uchar dec){
	return (dec/10)*16 + (dec%10);
}

/********��ʼ��DS1302����********/
void ds1302_init(uchar pYear, uchar pMonth, uchar pDay, 
				 uchar pWeek,
				 uchar pHour, uchar pMin, uchar pSec)
{
	ds1302_reset=0;
	ds1302_sclk=0;

	write_ds1302(0x90,0xab); 	//д�����

	//����д���ʼ��ʱ�� ����

	//��Ĵ�����BIT7:4=���ʮλ����BIT3:0=��ĸ�λ��
	write_ds1302(0x8c, dec2DS1302Format(pYear)); 		// ��(20xx��xx����)����00-99��

	//�¼Ĵ�����BIT7:5=0��BIT4=�µ�ʮλ����BIT3:0=�µĸ�λ��
	write_ds1302(0x88, dec2DS1302Format(pMonth)); 	// ��

	//�ռĴ�����BIT7:6=0��BIT5:4=�յ�ʮλ����BIT3:0=�յĸ�λ��
	write_ds1302(0x86, dec2DS1302Format(pDay)); 		// ��

	//���ڼĴ�����BIT7:3=0��BIT2:0=���ڣ�1-7��
	write_ds1302(0x8a, dec2DS1302Format(pWeek));		// ����

	//ʱ�Ĵ�����BIT7=24Сʱ��(0)/12Сʱ��(1)
	//			BIT6=0
	//			12Сʱ�ƣ�BIT5=AM(0)/PM(1)��BIT4=ʱ��ʮλ��
	//			24Сʱ�ƣ�BIT5:4=ʱ��ʮλ��
	//			BIT3:0=ʱ�ĸ�λ��
	write_ds1302(0x84, dec2DS1302Format(pHour)); 		// ʱ

	//�ּĴ�����BIT7=NA��BIT6:4=�ֵ�ʮλ����BIT3:0=�ֵĸ�λ��
	write_ds1302(0x82, dec2DS1302Format(pMin)); 		// ��

	//��Ĵ�����BIT7=CH��BIT6:4=���ʮλ����BIT3:0=��ĸ�λ��
	write_ds1302(0x80, dec2DS1302Format(pSec)); 		// ��

	write_ds1302(0x8e,0x80); 	//д���������֣���ֹд
}

void ds1302_get_time()
{
	write_ds1302(0x8e,0x00);		//��������,WP=0,����д����   
	write_ds1302(0x90,0xab);		//���������

	ds1302_sec=read_ds1302(0x81);			//��ȡ��Ĵ�����BIT7=CH��BIT6:4=���ʮλ����BIT3:0=��ĸ�λ��
	ds1302_min=read_ds1302(0x83);			//��ȡ�ּĴ�����BIT7=NA��BIT6:4=�ֵ�ʮλ����BIT3:0=�ֵĸ�λ��
	ds1302_hour=read_ds1302(0x85);			//��ȡʱ�Ĵ�����BIT7=24Сʱ��(0)/12Сʱ��(1)
									//				BIT6=0
									//				12Сʱ�ƣ�BIT5=AM(0)/PM(1)��BIT4=ʱ��ʮλ��
									//				24Сʱ�ƣ�BIT5:4=ʱ��ʮλ��
									//				BIT3:0=ʱ�ĸ�λ��
	ds1302_ri=read_ds1302(0x87);			//��ȡ�ռĴ�����BIT7:6=0��BIT5:4=�յ�ʮλ����BIT3:0=�յĸ�λ��
	ds1302_yue=read_ds1302(0x89);			//��ȡ�¼Ĵ�����BIT7:5=0��BIT4=�µ�ʮλ����BIT3:0=�µĸ�λ��
	ds1302_nian=read_ds1302(0x8D);			//��ȡ��Ĵ�����BIT7:4=���ʮλ����BIT3:0=��ĸ�λ��

	// ��������ÿ���Ĵ�����������ת�������յ���Ч����
	ds1302_sec=(ds1302_sec&0x7F)/16*10+(ds1302_sec&0x7F)%16;
	ds1302_min=(ds1302_min&0x7F)/16*10+(ds1302_min&0x7F)%16;

	if ((ds1302_hour&0x80)==0x80)
	{
		// 12Сʱ��
		ds1302_hour=(ds1302_hour&0x1F)/16*10+(ds1302_hour&0x1F)%16;
	} else {
		// 24Сʱ��
		ds1302_hour=(ds1302_hour&0x3F)/16*10+(ds1302_hour&0x3F)%16;
	}

	ds1302_ri=ds1302_ri/16*10+ds1302_ri%16;
	ds1302_yue=ds1302_yue/16*10+ds1302_yue%16;
	ds1302_nian=ds1302_nian/16*10+ds1302_nian%16;

	/*
	time_buf[0]=ds1302_sec/16*10+ds1302_sec%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[1]=ds1302_min/16*10+ds1302_min%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[2]=ds1302_hour/16*10+ds1302_hour%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[3]=ds1302_ri/16*10+ds1302_ri%16;			//����ȡ����16������ת��Ϊ10����
	time_buf[4]=ds1302_yue/16*10+ds1302_yue%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[6]=ds1302_nian/16*10+ds1302_nian%16;		//����ȡ����16������ת��Ϊ10����
	*/
}

/********������ת������,������ʱ����ת��Ϊ�ʺ�LCD��ʾ������********/
uchar* ds1302_getStirngYYYYMMDDHHMMSS()
{
	uchar idx = 0;

	ds1302_disp_buf[idx++]=2+0x30;    		//��ǧλ����
	ds1302_disp_buf[idx++]=0+0x30;			//���λ����
	ds1302_disp_buf[idx++]=ds1302_nian/10+0x30;   //��ʮλ����
	ds1302_disp_buf[idx++]=ds1302_nian%10+0x30;	//��ʱ��λ����
	ds1302_disp_buf[idx++]='/';
	ds1302_disp_buf[idx++]=ds1302_yue/10+0x30;    //��ʮλ����
	ds1302_disp_buf[idx++]=ds1302_yue%10+0x30;	//�¸�λ����
	ds1302_disp_buf[idx++]='/';
	ds1302_disp_buf[idx++]=ds1302_ri/10+0x30;    	//��ʮλ����
	ds1302_disp_buf[idx++]=ds1302_ri%10+0x30;		//�ո�λ����
	ds1302_disp_buf[idx++]=' ';
	ds1302_disp_buf[idx++]=ds1302_hour/10+0x30;   //Сʱʮλ����
	ds1302_disp_buf[idx++]=ds1302_hour%10+0x30;	//Сʱ��λ����
	ds1302_disp_buf[idx++]=':';
	ds1302_disp_buf[idx++]=ds1302_min/10+0x30;    //����ʮλ����
	ds1302_disp_buf[idx++]=ds1302_min%10+0x30;	//���Ӹ�λ����
	ds1302_disp_buf[idx++]=':';
	ds1302_disp_buf[idx++]=ds1302_sec/10+0x30;    //��ʮλ����
	ds1302_disp_buf[idx++]=ds1302_sec%10+0x30;	//���λ����

	ds1302_disp_buf[idx++]=0x00;

	return ds1302_disp_buf;
}


uchar ds1302_getYear(){
	return ds1302_nian;
}

uchar ds1302_getMonth(){
	return ds1302_yue;
}

uchar ds1302_getDay(){
	return ds1302_ri;
}

uchar ds1302_getHour(){
	return ds1302_hour;
}

uchar ds1302_getMin(){
	return ds1302_min;
}

uchar ds1302_getSec(){
	return ds1302_sec;
}

