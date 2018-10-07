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
#define MAX_BRIGHT 250

sbit VPRG	=P3^0;
sbit SIN	=P3^1;
sbit SCLK	=P3^2;
sbit XLAT	=P3^3;
sbit BLANK	=P3^4;
sbit GSCLK	=P3^5;

uint i,n=0;

uint gsDataArray[16];

void  delay(uint t);

void disp01(uchar ledOnCnt, uint maxBrightPer, uint minBrightPer, uchar speed, uchar cnt);
void disp02_B2T_ON(uint brightPer, uchar speed, uchar cnt);
void disp03_B2T_OFF(uint brightPer, uchar speed, uchar cnt);
void disp04_T2B_ON(uint brightPer, uchar speed, uchar cnt);
void disp05_T2B_OFF(uint brightPer, uchar speed, uchar cnt);
void disp06(uint brightPer, uchar speed, uchar cnt);

void disp99(uchar speed, uchar cnt);

void delay(uint time)
{
uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<250;j++);
}

void setBitData(uchar b){
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

void setGSDataArray(uchar idx, uint gsData12bit){
	gsDataArray[idx] = gsData12bit;
}

// nLed: 1-5
void setRGBLED(uchar nLed, uchar R, uchar G, uchar B){
	uchar i;
	uint rBright, gBright, bBright = 0;
	
	rBright = MAX_BRIGHT * R / 255;
	gBright = MAX_BRIGHT * G / 255;
	bBright = MAX_BRIGHT * B / 255;
	
	setGSDataArray((nLed-1)*3 , rBright);
	setGSDataArray((nLed-1)*3 + 1, gBright);
	setGSDataArray((nLed-1)*3 + 2, bBright);
}

void flushTlc5940(){
	TR0=0;
	BLANK = 1;
		
	for (i=16;i>0;i--){
		setGSData(gsDataArray[i-1]);
	}
	
	XLAT = 0;
	XLAT = 1;
		
	n=0;
	BLANK = 0;
	TR0=1;
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
  uchar r,g,b;
  
  Timer0Init();
  
  #if test
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
  #endif test
  
  r=255;
  g=b=0;
  
  while(1){
	
	// G+
	for(g=0;g<240;g++){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// R-
	for(r=240;r>0;r--){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// B+
	for(b=0;b<240;b++){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// G-
	for(g=240;g>0;g--){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// R+
	for(r=0;r<240;r++){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// G+
	for(g=0;g<240;g++){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// B-
	for(b=240;b>0;b--){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
	
	// G-
	for(g=240;g>0;g--){
		setRGBLED(1, r, g, b);
		setRGBLED(2, g, b, r);
		setRGBLED(3, b, r, g);
		setRGBLED(4, r, g, b);
		setRGBLED(5, g, b, r);
		flushTlc5940();
		delay(5);
	}
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

void disp02_B2T_ON(uint brightPer, uchar speed, uchar cnt) {
	uchar row, idx = 0;
	uchar nCnt = 0;
	uint bright;
	
	if(brightPer>100) brightPer=100;
	
	bright = MAX_BRIGHT / 100 * brightPer;
	
	
	for (nCnt=0; nCnt<cnt; nCnt++){
		
		for (row=9; row>0; row--){
			TR0=0;
			//BLANK = 1;
			
			// LED16 ROW2
			if(row<=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED15 ROW3
			if(row<=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED14 ROW4
			if(row<=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED13 ROW5
			if(row<=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED12 ROW6
			if(row<=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED11 ROW7
			if(row<=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED10 ROW8
			if(row<=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED9 ROW9
			if(row<=9){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED8 ROW8
			if(row<=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED7 ROW7
			if(row<=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED6 ROW6
			if(row<=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED5 ROW5
			if(row<=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED4 ROW4
			if(row<=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED3 ROW3
			if(row<=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED2 ROW2
			if(row<=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED1 ROW1
			if(row<=1){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			
			XLAT = 0;
			XLAT = 1;
			
			//n=0;
			//BLANK = 0;
			TR0=1;
			
			// Ϊ���Ӿ��ϵ�ͳһ������LEDʵ�ʵ����У�΢��ÿ��֮��ļ��
			// 5-9�еļ����һ���ģ�ʹ�ò���ָ���ļ��
			// 4-5�м���Ǳ�׼��3/2��3-4�м����̼���ͬʱ��2-3�б�3-4���Գ���1-2�д���Ǳ�׼��һ��
			// if(row>5){
				// delay(speed);
			// } else if(row==5) {
				// delay(speed/3 * 2);
			// } else if(row==4) {
				// delay(speed/3);
			// } else if(row==3) {
				// delay(speed/3 * 2);
			// } else if(row==2) {
				// delay(speed/2);
			// } else if(row==1) {
				// delay(speed*5);
			// }
			delay(speed);
		}
		
	}
}

void disp03_B2T_OFF(uint brightPer, uchar speed, uchar cnt) {
	uchar row, idx = 0;
	uchar nCnt = 0;
	uint bright;
	
	if(brightPer>100) brightPer=100;
	
	bright = MAX_BRIGHT / 100 * brightPer;
	
	
	for (nCnt=0; nCnt<cnt; nCnt++){
		
		for (row=9; row>0; row--){
			TR0=0;
			//BLANK = 1;
			
			// LED16 ROW2
			if(row>=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED15 ROW3
			if(row>=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED14 ROW4
			if(row>=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED13 ROW5
			if(row>=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED12 ROW6
			if(row>=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED11 ROW7
			if(row>=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED10 ROW8
			if(row>=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED9 ROW9
			if(row>=9){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED8 ROW8
			if(row>=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED7 ROW7
			if(row>=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED6 ROW6
			if(row>=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED5 ROW5
			if(row>=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED4 ROW4
			if(row>=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED3 ROW3
			if(row>=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED2 ROW2
			if(row>=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED1 ROW1
			if(row>=1){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			
			XLAT = 0;
			XLAT = 1;
			
			//n=0;
			//BLANK = 0;
			TR0=1;
			delay(speed);
		}
		
	}
}

void disp04_T2B_ON(uint brightPer, uchar speed, uchar cnt) {
	uchar row, idx = 0;
	uchar nCnt = 0;
	uint bright;
	
	if(brightPer>100) brightPer=100;
	
	bright = MAX_BRIGHT / 100 * brightPer;
	
	
	for (nCnt=0; nCnt<cnt; nCnt++){
		
		for (row=1; row<=9; row++){
			TR0=0;
			//BLANK = 1;
			
			// LED16 ROW2
			if(row>=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED15 ROW3
			if(row>=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED14 ROW4
			if(row>=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED13 ROW5
			if(row>=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED12 ROW6
			if(row>=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED11 ROW7
			if(row>=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED10 ROW8
			if(row>=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED9 ROW9
			if(row>=9){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED8 ROW8
			if(row>=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED7 ROW7
			if(row>=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED6 ROW6
			if(row>=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED5 ROW5
			if(row>=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED4 ROW4
			if(row>=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED3 ROW3
			if(row>=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED2 ROW2
			if(row>=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED1 ROW1
			if(row>=1){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			
			XLAT = 0;
			XLAT = 1;
			
			//n=0;
			//BLANK = 0;
			TR0=1;
			
			delay(speed);
		}
		
	}
}

void disp05_T2B_OFF(uint brightPer, uchar speed, uchar cnt) {
	uchar row, idx = 0;
	uchar nCnt = 0;
	uint bright;
	
	if(brightPer>100) brightPer=100;
	
	bright = MAX_BRIGHT / 100 * brightPer;
	
	
	for (nCnt=0; nCnt<cnt; nCnt++){
		
		for (row=1; row<=9; row++){
			TR0=0;
			//BLANK = 1;
			
			// LED16 ROW2
			if(row<=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED15 ROW3
			if(row<=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED14 ROW4
			if(row<=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED13 ROW5
			if(row<=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED12 ROW6
			if(row<=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED11 ROW7
			if(row<=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED10 ROW8
			if(row<=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED9 ROW9
			if(row<=9){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED8 ROW8
			if(row<=8){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED7 ROW7
			if(row<=7){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED6 ROW6
			if(row<=6){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED5 ROW5
			if(row<=5){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED4 ROW4
			if(row<=4){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED3 ROW3
			if(row<=3){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED2 ROW2
			if(row<=2){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			// LED1 ROW1
			if(row<=1){
				setGSData(bright);
			} else {
				setGSData(0);
			}
			
			XLAT = 0;
			XLAT = 1;
			
			//n=0;
			//BLANK = 0;
			TR0=1;
			
			delay(speed);
		}
		
	}
}

void disp99(uchar speed, uchar cnt) {
	uchar i, j = 0;
	
	TR0=0;
	BLANK = 1;
	
	for (i=16;i>0;i--){
		//setGSData(MAX_BRIGHT);
		
		if(i==1) {
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
