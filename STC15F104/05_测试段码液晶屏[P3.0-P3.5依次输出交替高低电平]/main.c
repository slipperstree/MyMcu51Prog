/*********************************************************************************************/

/* ���Ե�Ƭ�� STC15F100ϵ�е�Ƭ�� ���Զ���Һ����[P3.0-P3.5�����������ߵ͵�ƽ] */ 

/*********************************************************************************************/     
#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>
#define uchar unsigned char
#define uint unsigned int

sbit p30        =P3^0;         //
sbit p31        =P3^1;         //
sbit p32        =P3^2;         //
sbit p33        =P3^3;         //
sbit p34        =P3^4;         //
sbit p35        =P3^5;         //

void  delay(uint t);

void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<250;j++);
}
						 
int a=0;
main()
{
    while(1){
		if(a==0){
			p30 = 0;
			p31 = 1;
			p32 = 0;
			p33 = 1;
			p34 = 0;
			p35 = 1;
			a = 1;
		}else if(a==1){
		 	p30 = 1;
			p31 = 0;
			p32 = 1;
			p33 = 0;
			p34 = 1;
			p35 = 0;
			a = 0;
		}
		delay(10);
	}
}