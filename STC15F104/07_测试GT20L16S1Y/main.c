#include<STC15104E.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#include<stdlib.h>
#define uchar unsigned char
#define uint unsigned int

sbit pCE        =P3^2;         //
sbit pSCLK      =P3^3;         //
sbit pSI        =P3^4;         //
sbit pSO        =P3^5;         //

sbit pLED        =P3^0;         //

void  delay(uint t);

void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<2500;j++);
}

long addr=0x123456;
uchar idx = 0;

main()
{
	//P3M0 = 0xFF;
	//P3M1 = 0x00;
	
	pSI=0;
	pCE=1;
	
	addr = ('A'-0x20)*16+0x3b7c0;
	
	pCE=0; // start to send command
	
	for (idx = 0; idx<8; idx++){
		//pLED=1;
		pSCLK=0;
		pSI=0x03<<idx & 0x80;
		//pLED=0;
		pSCLK=1;
	}
	
	for (idx = 0; idx<24; idx++){
		//pLED=1;
		pSCLK=0;
		pSI=addr<<idx & 0x800000;
		//pLED=0;
		pSCLK=1;
	}
	
	//get!
	for (idx = 0; idx<8*16; idx++){
		//pLED=1;
		pSCLK=0;
		//pLED=0;
		pSCLK=1;
	}

	pSCLK=0;

    while(1){
		
		
	}
}