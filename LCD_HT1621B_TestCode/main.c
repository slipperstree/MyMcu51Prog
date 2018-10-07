#include <at89x52.h>
#include "HT1621B.h"
#include "HT1621B.c"
#include <intrins.h>	   //为了使用NOP

#define NOP _nop_()

#define uchar unsigned char

void DelayMs(uint n);

void setSingleNumber(uchar n, uchar num);

void main(void)
{
	// 初始化为10是为了默认各位上不显示，显示子程序里如果指定0-9以外的数字就清除显示
	uchar n1=0;
	uchar n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12=0;
	
	Init_1621();       //初始化HT1621
	HT1621_all_off();  //清空LCD显示
	HT1621_all_on();   //点亮所有Seg
	DelayMs(2000);
	HT1621_all_off();

	//以下点亮指定Seg，以显示所需要的数字、字符等
	Write_1621(16,0x80,4);  //显示"TEST"，注意：数据是低位在前！！！
	DelayMs(1000);
	
	while(1) {
		DelayMs(100);
		
		n1++;
		if(n1==10) {
			n2++;
			n1=0;
		}
		if(n2==10) {
			n3++;
			n2=0;
		}
		if(n3==10) {
			n4++;
			n3=0;
		}
		if(n4==10) {
			n5++;
			n4=0;
		}
		if(n5==10) {
			n6++;
			n5=0;
		}
		if(n6==10) {
			n7++;
			n6=0;
		}
		if(n7==10) {
			n8++;
			n7=0;
		}
		if(n8==10) {
			n9++;
			n8=0;
		}
		if(n9==10) {
			n10++;
			n9=0;
		}
		if(n10==10) {
			n11++;
			n10=0;
		}
		if(n11==10) {
			n12++;
			n11=0;
		}
		if(n12==10) {
			break;
		}

		setSingleNumber(1, n12);
		setSingleNumber(2, n11);
		setSingleNumber(3, n10);
		setSingleNumber(4, n9);
		setSingleNumber(5, n8);
		setSingleNumber(6, n7);
		setSingleNumber(7, n6);
		setSingleNumber(8, n5);
		setSingleNumber(9, n4);
		setSingleNumber(10, n3);
		setSingleNumber(11, n2);
		setSingleNumber(12, n1);
	}
	
	while(1) {}
}

void DelayMs(uint n)
{
    uchar a,b;
	uint i;
	for(i=n;i>0;i--)
	    for(b=80;b>0;b--)
	        for(a=10;a>0;a--)NOP;
}

void setSingleNumber(uchar n, uchar num) {
	uchar addr1 = 0;
	uchar addr2 = 0;
	uchar data1=0x00;
	uchar data2=0x00;
	
	switch (n) {
		case 1:
			addr1=	22;
			addr2=23;
			break;
		case 2:
			addr1=	0;
			addr2=21;
			break;
		case 3:
			addr1=	1;
			addr2=20;
			break;
		case 4:
			addr1=	2;
			addr2=19;
			break;
		case 5:
			addr1=	3;
			addr2=18;
			break;
		case 6:
			addr1=	4;
			addr2=17;
			break;
		case 7:
			addr1=	5;
			addr2=16;
			break;
		case 8:
			addr1=	6;
			addr2=15;
			break;
		case 9:
			addr1=	7;
			addr2=14;
			break;
		case 10:
			addr1=	8;
			addr2=13;
			break;
		case 11:
			addr1=	9;
			addr2=12;
			break;
		case 12:
			addr1=	10;
			addr2=11;
			break;
	}
	
	switch (num) {
		case 1:
			data1=	0x60;
			data2=0x00;
			break;
		case 2:
			data1=	0xd0;
			data2=0x30;
			break;
		case 3:
			data1=	0xf0;
			data2=0x20;
			break;
		case 4:
			data1=	0x60;
			data2=0x60;
			break;
		case 5:
			data1=	0xb0;
			data2=0x60;
			break;
		case 6:
			data1=	0xb0;
			data2=0x70;
			break;
		case 7:
			data1=	0xe0;
			data2=0x00;
			break;
		case 8:
			data1=	0xf0;
			data2=0x70;
			break;
		case 9:
			data1=	0xf0;
			data2=0x60;
			break;
		case 0:
			data1=0xf0;
			data2=0x50;
			break;
		default:
			data1=0x00;
			data2=0x00;
			break;
	}
	
	Write_1621(addr1, data1, 4);
	Write_1621(addr2, data2, 4);
}






