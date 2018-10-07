#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>
#define uchar unsigned char
#define uint unsigned int

sbit pR        =P3^2;         //
sbit pG        =P3^3;         //
sbit pY        =P3^4;         //

void  delay(uint t);

void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<2500;j++);
}

int x=10;						 
int a=0;
main()
{
	P3M0 = 0xFF;
	P3M1 = 0x00;

    while(1){
		if(a==0){
			pR = 1;
			pG = 0;
			pY = 0;
			a = 1;
			delay(700*x);
		}else if(a==1){
			pR = 0;
			pG = 0;
			pY = 1;
			a = 2;
			delay(300*x);
		}else if(a==2){
			pR = 0;
			pG = 1;
			pY = 0;
			a = 3;
			delay(700*x);
		}else if(a==3){
			pR = 0;
			pG = 0;
			pY = 1;
			a = 0;
			delay(300*x);
		}
	}
}