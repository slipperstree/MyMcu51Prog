#include <DS1302.h>
#include <stdlib.h>

#define uchar unsigned char
#define uint  unsigned int

sbit reset = P2^5;		//P2^5;
sbit sclk  = P2^7;		//P2^6;
sbit io    = P2^6;		//P2^7;

uchar disp_buf[20] ={0x00}; //������ʾ������
uchar temp [2]={0};			//�����������ʱ��Сʱ�����ӵ��м�ֵ
uint m=0,n=0,r=0;

uchar sec,min,hour,ri,yue,nian;					//�����롢�ֺ�Сʱ����

/********������дһ�ֽ����ݺ���********/
void write_byte(uchar inbyte)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		sclk=0;             //дʱ�͵�ƽ�ı�����

		if(inbyte&0x01)
			io=1;
		else
			io=0;

		sclk=1;            //�ߵ�ƽ������д��DS1302

		inbyte=inbyte>>1;
	}
}

/********�����Ƕ�һ�ֽ����ݺ���********/
uchar read_byte()    
{
	uchar i,temp=0; 
	io=1;
	for(i=0;i<7;i++)
	{
		sclk=0;
		if(io==1)
			temp=temp|0x80;
		else
			temp=temp&0x7f;

		sclk=1;             //����������
		temp=temp>>1;
	}
	return (temp);
}

/********дDS1302����, ��DS1302��ĳ����ַд������ ********/
void write_ds1302(uchar cmd,uchar indata)
{
	sclk=0;
	reset=1;
	write_byte(cmd);        
	write_byte(indata);
	sclk=0;
	reset=0;
}

/********��DS1302����,��DS1302ĳ��ַ�ĵ�����********/
uchar read_ds1302(uchar addr)
{
	uchar backdata;
	sclk=0;
	reset=1;
	write_byte(addr);      		//��д��ַ
	backdata=read_byte();  		//Ȼ�������
	sclk=0;
	reset=0;
	return (backdata);
}

// ����ͨ����ת��DS1302�����Ĵ�����Ҫ�ĸ�ʽ
// ʮλ�����ڸ�4λ�ϣ���λ�����ڵ�4λ��
uchar dec2DS1302Format(uchar dec){
	return (dec/10)*16 + (dec%10);
}

/********��ʼ��DS1302����********/
void init_ds1302(uchar pYear, uchar pMonth, uchar pDay, 
				 uchar pWeek,
				 uchar pHour, uchar pMin, uchar pSec)
{
	reset=0;
	sclk=0;

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

void get_time()
{
	write_ds1302(0x8e,0x00);		//��������,WP=0,����д����   
	write_ds1302(0x90,0xab);		//���������

	sec=read_ds1302(0x81);			//��ȡ��Ĵ�����BIT7=CH��BIT6:4=���ʮλ����BIT3:0=��ĸ�λ��
	min=read_ds1302(0x83);			//��ȡ�ּĴ�����BIT7=NA��BIT6:4=�ֵ�ʮλ����BIT3:0=�ֵĸ�λ��
	hour=read_ds1302(0x85);			//��ȡʱ�Ĵ�����BIT7=24Сʱ��(0)/12Сʱ��(1)
									//				BIT6=0
									//				12Сʱ�ƣ�BIT5=AM(0)/PM(1)��BIT4=ʱ��ʮλ��
									//				24Сʱ�ƣ�BIT5:4=ʱ��ʮλ��
									//				BIT3:0=ʱ�ĸ�λ��
	ri=read_ds1302(0x87);			//��ȡ�ռĴ�����BIT7:6=0��BIT5:4=�յ�ʮλ����BIT3:0=�յĸ�λ��
	yue=read_ds1302(0x89);			//��ȡ�¼Ĵ�����BIT7:5=0��BIT4=�µ�ʮλ����BIT3:0=�µĸ�λ��
	nian=read_ds1302(0x8D);			//��ȡ��Ĵ�����BIT7:4=���ʮλ����BIT3:0=��ĸ�λ��

	// ��������ÿ���Ĵ�����������ת�������յ���Ч����
	sec=(sec&0x7F)/16*10+(sec&0x7F)%16;
	min=(min&0x7F)/16*10+(min&0x7F)%16;

	if ((hour&0x80)==0x80)
	{
		// 12Сʱ��
		hour=(hour&0x1F)/16*10+(hour&0x1F)%16;
	} else {
		// 24Сʱ��
		hour=(hour&0x3F)/16*10+(hour&0x3F)%16;
	}

	ri=ri/16*10+ri%16;
	yue=yue/16*10+yue%16;
	nian=nian/16*10+nian%16;

	/*
	time_buf[0]=sec/16*10+sec%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[1]=min/16*10+min%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[2]=hour/16*10+hour%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[3]=ri/16*10+ri%16;			//����ȡ����16������ת��Ϊ10����
	time_buf[4]=yue/16*10+yue%16;		//����ȡ����16������ת��Ϊ10����
	time_buf[6]=nian/16*10+nian%16;		//����ȡ����16������ת��Ϊ10����
	*/
}

/********������ת������,������ʱ����ת��Ϊ�ʺ�LCD��ʾ������********/
uchar* getStirngYYYYMMDDHHMMSS()
{
	uchar idx = 0;

	disp_buf[idx++]=2+0x30;    		//��ǧλ����
	disp_buf[idx++]=0+0x30;			//���λ����
	disp_buf[idx++]=nian/10+0x30;   //��ʮλ����
	disp_buf[idx++]=nian%10+0x30;	//��ʱ��λ����
	disp_buf[idx++]='/';
	disp_buf[idx++]=yue/10+0x30;    //��ʮλ����
	disp_buf[idx++]=yue%10+0x30;	//�¸�λ����
	disp_buf[idx++]='/';
	disp_buf[idx++]=ri/10+0x30;    	//��ʮλ����
	disp_buf[idx++]=ri%10+0x30;		//�ո�λ����
	disp_buf[idx++]=' ';
	disp_buf[idx++]=hour/10+0x30;   //Сʱʮλ����
	disp_buf[idx++]=hour%10+0x30;	//Сʱ��λ����
	disp_buf[idx++]=':';
	disp_buf[idx++]=min/10+0x30;    //����ʮλ����
	disp_buf[idx++]=min%10+0x30;	//���Ӹ�λ����
	disp_buf[idx++]=':';
	disp_buf[idx++]=sec/10+0x30;    //��ʮλ����
	disp_buf[idx++]=sec%10+0x30;	//���λ����

	disp_buf[idx++]=0x00;

	return disp_buf;
}


uchar getYear(){
	return nian;
}

uchar getMonth(){
	return yue;
}

uchar getDay(){
	return ri;
}

uchar getHour(){
	return read_ds1302(0x85);
}

uchar getMin(){
	return min;
}

uchar getSec(){
	return sec;
}

