/*************************************************************
���TLC5940оƬ���16·PWM����LED����
����������Ҫ��Խ��Խ�ã����鲻����24M
MAX_GRAY_SCAL�����PWM���ȣ����Ϊ4096����TCL5940������
MAX_BRIGHT��   �����������ȣ����ΪMAX_GRAY_SCAL

Ӳ�����ӣ�
|-----------------------------------------|
|TLC5940		51��Ƭ��/����             |
|-----------------------------------------|
|VPRG			GND                       |
|SIN			P3.1                      |
|SCLK			P3.2                      |
|XLAT			P3.3                      |
|BLANK			P3.4                      |
|GSCLK			P3.5                      |
|IREF			ͨ��һ��2-3K�ĵ���ӵ�    |
|OUT0-15		��LED������LED������VCC�� |
|-----------------------------------------|
**************************************************************/

#include<STC15104E.h>

#define uchar unsigned char
#define uint unsigned int

#define MAX_GRAY_SCAL 500
#define MAX_BRIGHT 200

#define USER_MAX_BRIGHT 10

sbit VPRG	=P3^0;
sbit SIN	=P3^1;
sbit SCLK	=P3^2;
sbit XLAT	=P3^3;
sbit BLANK	=P3^4;
sbit GSCLK	=P3^5;

#define OFF_TO_ON 0
#define ON_TO_OFF 1
#define START_TO_END 0
#define END_TO_START 1

/*
DISP_MODE01 ����˵��

 ���鹲17�����֡�
  ��һ������ָ��һ������ѭ���������֡������9��ʾ����������9��֡
  �����16�����ֱַ��ʾ16ֻ��Ӧ���ڶ����ĵ�n֮֡�󣨶�����ֻ�ڵ�n֡������������Ϩ��ȡ���� OFF_TO_ON or ON_TO_OFF��
     Ҳ���ǣ� ��ǰ֡�� >= ĳλ���ϵ�����  ʱ���õƱ�����
  ���������������һ��9֡
     �м���ֻ�ƴӵ�һ֡��ʼ�����һ֡һֱ������
	 ������������ֻ�ƻ��ڵ�5֡��ʼ������
	 ��9֡��ʱ�����еƶ�������

        3          3
     4    2    [2]   4
    5		(1)		  5
					 
	  6				6
	    7		  7
		  8		8
			 9

  ������Ҫ�Ȼ��������ʾ��ͼ��
     �����һ�����ִ���֡��
	 ��2-17�����ִ�[]λ�ÿ�ʼ˳ʱ����ת��()����
	 ��������������������鶨��Ӧ���ǣ�   
	    9,
		2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1	               

*/
uchar code DISP_MODE01_TB[] = { 
	9, 
	2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1
	};

uchar code DISP_MODE01_LR[] = { 
	9, 
	6,7,8,9,8,7,6,5,4,3,2,1,2,3,4,5,
	};

// from left top to right bottom
uchar code DISP_MODE01_LTRB[] = { 
	7, 
	4,4,5,6,7,7,6,6,5,4,3,2,1,2,3,4
	};

uchar code DISP_MODE01_ONE_BY_ONE[] = { 
	16, 
	2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1
	};

uchar code DISP_MODE01_ONE_BY_TWO[] = { 
	16, 
	9,2,10,3,11,4,12,5,13,6,14,7,15,8,16,1
	};

uint i,n=0;

void  delay(uint t);
void disp01(uchar ledOnCnt, uint maxBrightPer, uint minBrightPer, uchar speed, uchar cnt);
void disp_heartbeat(uint maxBrightPer, uchar speed, uchar cnt);
void disp99(uchar speed, uchar cnt);

void dispMode01(uchar* dispArr, uchar optStartEnd, uchar optOnOff, uint brightPer, uchar speed, uchar cnt);

void delay(uint time) {
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<250;j++);
}

void setBitData(uchar b) {
	SCLK = 0;
	SIN = b;
    SCLK = 1;
}

void setGSData(uint gsData12bit){
	uchar i=0;
	for(i=0; i<12; i++){
		if (((gsData12bit<<i) & 0x800 ) == 0x800) {
			setBitData(1);
		} else {
			setBitData(0);
		}
	}
}

void Timer0Init(void)		//2us@33.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ

	TL0 = 0xDF;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ

	TF0 = 0;		//���TF0��־
	TR0 = 0;		//��ʱ��0��ʼ��ʱ

	ET0 = 1;
	EA = 1;
}

void main () {

  uchar i,j,m,n;
  Timer0Init();

  // // VPRG����ΪL��ʹ�乤����GS mode (����Ӳ��ֱ������)
  // VPRG = 0;
  // // BLANK����ΪH���ر��������
  // BLANK = 1;

  // // ����12bit X 16��PWM��ֵGSn(n=0-15)����192bit
  // // ÿ�����ݵ�ֵ��Χ��0-4095
  // // ��Ϊ��ͨ����λ�Ĵ������䣬���Դ���˳���ǵ���ģ�GS15��GS14������GS0

  // // GSn������OUTn��PWM������GSn / 4095 = 0% - 100%��
  // setGSData(0); // OUT15
  // setGSData(0);
  // setGSData(0);
  // setGSData(0);
  // setGSData(0);
  // setGSData(0); // OUT10
  // setGSData(0);
  // setGSData(0);
  // setGSData(0);
  // setGSData(0);
  // setGSData(0); // OUT5
  // setGSData(0);
  // setGSData(0);
  // setGSData(0);
  // setGSData(0);
  // setGSData(0); // OUT0

  // // ����GS���ݺ󣬴���XLAT�������أ�����λ�Ĵ���������һ��������GS�Ĵ���
  // XLAT = 0;
  // XLAT = 1;

  // // BLANK����ΪL�����������
  // BLANK = 0;
  
  // ׼��������ϣ�Ϊ����TLC5940оƬ��������������Ҫ��GSCLK����ʱ���ź�
  // ������ʱ��0����ʼ��������źţ�Ƶ��Խ��Ч��Խ�ã�
  // TLC5940��������ʱ���źŽ��д�0-4095�ļ������ߵ͵�ƽÿ����һ����һ�Σ���
  // һ�߼���һ�߼���GSn���趨ֵ��һ������GSn��ֵ�����л�OUTn�ĵ�ƽһֱ������4095Ϊֹ���������BLANK����Ϊֹ����
  // �ٴ�0��ʼ���¼�������0������4095����;��BLANK����ʱ�ļ���������һ���ߵ͵�ƽ���л����ڡ�
  // �������Ǹ�5940�ṩ��ʱ��Ƶ��Խ�죬��������PWMƵ�ʾ�Խ�졣��������PWM��Ƶ�� = ʱ���źŵ�Ƶ�� / 4095����Blank��;����ʱ�ļ����������4095ͬ����
  // ����ʱ��Ƶ����8M�������Ӹ�OUT�������PWMƵ����8MHz / 4095 = 1.953KHz��
  // ����TLC5940�Ĺٷ��ĵ���GSCLK��֧�����30MHz��ʱ��Ƶ�ʣ�Ҳ������߿������7.3KHz��PWM�źš�
  // ����ǿ���LED�����ȣ���ô�����۸о�������˸�����Ƶ��Ӧ����50Hz���ϣ�
  // ��������Ӧ�����ٸ�GSCLK�ṩ������50 X 4095 = 200KHz�ķ�����
  // ʵ�⣺
  // ʹ��STC15F100ϵ�е�Ƭ����ָ���ڲ�����Ƶ��Ϊ33MHz��16λ�Զ����ض�ʱ����ÿ1us�л�һ��GSCLK�ĵ�ƽ��
  // ���⣬ͨ��Blank��;���ߵķ������Ƽ�����200��������4096�������ǳ���˸����
  // ������PWM�ľ��ȾͿ�����0-200֮��ָ��������һ���Ӧ���㹻�ˡ�
  // �����Ҫ���߾��ȱ����������оƬ֧�ֵ�4096�����ȣ�����Ҫ�����ٶȵ�GSCLK������У����Կ���ʹ��ALEʱ�ӷ�Ƶ�����
  // ��������̫��׼ȷ����ʱ������Ĵ�����Ӧ����ÿ���4096�ε�ʱ������������һ��blank�����������TLC5940�Լ������Զ���������ģ���
  // ����ʲôʱ������BLANK�Ǹ����⣬��Ҫ��ϸ���ԣ��������⻰
  //n = 0;
  //TR0 = 1;

  while(1){
// disp01 (uchar ledOnCnt, uchar maxBrightPer, uchar minBrightPer, uchar speed, uchar cnt)

//void dispMode01(uchar* dispArr, uchar optStartEnd, uchar optOnOff, uint brightPer, uchar speed, uchar cnt);
	disp_heartbeat(15, 5, 3);

	dispMode01(DISP_MODE01_ONE_BY_ONE, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_ONE_BY_ONE, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);
	dispMode01(DISP_MODE01_ONE_BY_ONE, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_ONE_BY_ONE, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);

	dispMode01(DISP_MODE01_LR, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_LR, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);
	dispMode01(DISP_MODE01_LR, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_LR, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);

	dispMode01(DISP_MODE01_TB, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_TB, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);
	dispMode01(DISP_MODE01_TB, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_TB, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);

	dispMode01(DISP_MODE01_ONE_BY_TWO, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_ONE_BY_TWO, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);
	dispMode01(DISP_MODE01_ONE_BY_TWO, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_ONE_BY_TWO, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);

	dispMode01(DISP_MODE01_LTRB, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_LTRB, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);
	dispMode01(DISP_MODE01_LTRB, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 40, 1); 
	dispMode01(DISP_MODE01_LTRB, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 40, 1);
	

	dispMode01(DISP_MODE01_LR, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 20, 1); 
	dispMode01(DISP_MODE01_LR, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 20, 1);
	dispMode01(DISP_MODE01_LR, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 20, 1); 
	dispMode01(DISP_MODE01_LR, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 20, 1);

	dispMode01(DISP_MODE01_TB, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 20, 1); 
	dispMode01(DISP_MODE01_TB, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 20, 1);
	dispMode01(DISP_MODE01_TB, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 20, 1); 
	dispMode01(DISP_MODE01_TB, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 20, 1);

	for (i = 0; i < 2; ++i)
	{
		dispMode01(DISP_MODE01_LR, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_LR, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);
		dispMode01(DISP_MODE01_LR, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_LR, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);

		dispMode01(DISP_MODE01_TB, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_TB, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);
		dispMode01(DISP_MODE01_TB, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_TB, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);

		dispMode01(DISP_MODE01_ONE_BY_ONE, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_ONE_BY_ONE, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);
		dispMode01(DISP_MODE01_ONE_BY_ONE, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_ONE_BY_ONE, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);

		dispMode01(DISP_MODE01_LTRB, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_LTRB, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);
		dispMode01(DISP_MODE01_LTRB, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_LTRB, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);

		dispMode01(DISP_MODE01_ONE_BY_TWO, START_TO_END, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_ONE_BY_TWO, START_TO_END, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);
		dispMode01(DISP_MODE01_ONE_BY_TWO, END_TO_START, OFF_TO_ON, USER_MAX_BRIGHT, 10, 1); 
		dispMode01(DISP_MODE01_ONE_BY_TWO, END_TO_START, ON_TO_OFF, USER_MAX_BRIGHT, 10, 1);
	}


	for(i=5; i<=8; i+=1){
		disp01(i, USER_MAX_BRIGHT, 20, 20, 1);
	}

	for(i=7; i>=1; i-=1){
		disp01(i, USER_MAX_BRIGHT, 20, 20, 1);
	}

	disp01(1, USER_MAX_BRIGHT, USER_MAX_BRIGHT, 10, 8);

	//disp01(16, USER_MAX_BRIGHT, 20, 5, 15);

	//for(i=16; i>=2; i-=4){
	//	disp01(i, USER_MAX_BRIGHT, 20, 20, 1);
	//}

  }

}



// ��ʾģʽ1��ÿ��OUT�������䰵���� ��OUT0��OUT15 ÿ·�������һ��������Ȳ�
void disp01(uchar ledOnCnt, uint maxBrightPer, uint minBrightPer, uchar speed, uchar cnt){

	uchar i, j = 0;
	uchar nCnt = 0;
	uint maxBright, minBright, offsetBright = 0;

	if(maxBrightPer>100) maxBrightPer=100;
	if(minBrightPer>maxBrightPer) minBrightPer=maxBrightPer;

	maxBright = MAX_BRIGHT / 100 * maxBrightPer;
	minBright = MAX_BRIGHT / 100 * minBrightPer;

	offsetBright = (maxBright - minBright) / ledOnCnt;

	for (nCnt=0; nCnt<cnt; nCnt++){
		for (j=0;j<16;j++){
			TR0=0;
			BLANK = 1;

			for (i=0;i<16;i++){
				// LED��Ŵ��ڵ�ǰ��ת�����������ڵ��жϣ�ע����λ���
				if(i>=j && i<j+ledOnCnt){
					setGSData(minBright + (offsetBright * (i - j)));
				} else if((j+ledOnCnt>16) && i<(j+ledOnCnt-16)) {
					setGSData(maxBright - (offsetBright * (j+ledOnCnt-16)-i));
				} else {
					setGSData(0);
				}
			}

			XLAT = 0;
			XLAT = 1;

			n=0;
			BLANK = 0;
			TR0=1;

			delay(speed);
		}
	}
}

// ��ʾģʽ6������ģʽ
// maxBrightPer ���������
// speed        �������ٶ�
// cnt          ����������
void disp_heartbeat(uint maxBrightPer, uchar speed, uchar cnt) {

	char nowBright = 0;
	uchar nCnt = 0;

	uint maxBright, minBright = 0;

	uint offsetBright = 1; // ÿ�����ȸı�ķ��ȣ�ֵԽС�仯Խ˳��

	if(maxBrightPer>100) maxBrightPer=100;

	maxBright = MAX_BRIGHT / 100 * maxBrightPer;

	for (nCnt=0; nCnt<cnt; nCnt++){

		for (nowBright=minBright; nowBright<maxBright; nowBright+=offsetBright){
			TR0=0;
			BLANK = 1;

			for (i=0;i<16;i++){
				setGSData(nowBright);
			}

			XLAT = 0;
			XLAT = 1;

			n=0;
			BLANK = 0;
			TR0=1;
			delay(speed);
		}

		delay(speed*10);

		for (nowBright=maxBright; nowBright>minBright; nowBright-=offsetBright){
			TR0=0;
			BLANK = 1;

			for (i=0;i<16;i++){
				setGSData(nowBright);
			}

			XLAT = 0;
			XLAT = 1;

			n=0;
			BLANK = 0;
			TR0=1;
			delay(speed);
		}

		//delay(speed*40);
		delay(speed*2);
	}
}

//uchar code DISP_MODE01_B2T_ON[] = { 
//	9, 
//	2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1
//	};
void dispMode01(uchar* dispArr, uchar optStartEnd, uchar optOnOff, uint brightPer, uchar speed, uchar cnt) {
	uchar idx, idxMax = 0;
	uchar nCnt, nLed = 0;
	uint bright;
	uint bright1;
	uint bright2;

	if(brightPer>100) brightPer=100;
	bright = MAX_BRIGHT / 100 * brightPer;

	if(optOnOff == OFF_TO_ON){
		bright1 = bright;
		bright2 = 0;
	} else {
		bright1 = 0;
		bright2 = bright;
	}

	idxMax =  dispArr[0];

	for (nCnt=0; nCnt<cnt; nCnt++){

		if (optStartEnd == END_TO_START)
		{
			for (idx=idxMax; idx>0; idx--){
				TR0=0;
				for (nLed=16; nLed>=1; nLed--) {
					if(idx<=dispArr[16-nLed+1]){
						setGSData(bright1);
					} else {
						setGSData(bright2);
					}
				}
				XLAT = 0;
				XLAT = 1;
			
				TR0=1;

				delay(speed);
			}
		} else {
			for (idx=1; idx<=idxMax; idx++){
				TR0=0;
				for (nLed=16; nLed>=1; nLed--) {
					if(idx>=dispArr[16-nLed+1]){
						setGSData(bright1);
					} else {
						setGSData(bright2);
					}
				}
				XLAT = 0;
				XLAT = 1;
			
				TR0=1;

				delay(speed);
			}
		}
		
		
	}
}

void disp99(uchar speed, uchar cnt) {

	uchar i, j = 0;

	

	TR0=0;

	BLANK = 1;

	

	for (i=16;i>0;i--){

		if(i==2) {

			setGSData(MAX_BRIGHT);

		} else {

			setGSData(0);

		}

	}

	

	XLAT = 0;

	XLAT = 1;

	

	n=0;

	BLANK = 0;

	TR0=1;

	

	delay(speed);

}



void timer0() interrupt 1

{

	GSCLK ^= 1;

	

	n++;

	if (n > MAX_GRAY_SCAL * 2) {

		BLANK = 1;BLANK = 0;

		n=0;

	}

}

