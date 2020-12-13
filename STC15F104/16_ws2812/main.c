/********************************************************/
/*�� �ߣ�â��
/*�޸����ڣ�2020.11.23
/*��    ����V1.4
/*�����ܣ���51��Ƭ������ws2812 RGB�������ʾȫɫ��
/*��        ע��51��Ƭ������11.0592M/24M/33M@STC15L104E����ͨ���������ͺ���Ҫ����ʵ����Ҫ������ʱ����
/********************************************************/

//#include<reg52.h>
#include "STC15104E.h"
#include "intrins.h"

sbit WS2812 = P3^5;

// �Ѿ���֤���ĵ�Ƭ����������Ƭ����Ƶ���²���֤�����������У�1T��/6T/12T�;���Ƶ��һ�µĻ�Ӧ����ͨ�õģ�
// ����1T�ĵ�Ƭ���Ͳ��ÿ���32M���µľ����ˣ�̫���˲�������ʱ��
// #define MCU_1T_11M_TEST_BY_STC15F10x
// #define MCU_1T_24M_TEST_BY_STC15F10x
// #define MCU_1T_33M_TEST_BY_STC15F10x
#define MCU_6T_32M_TEST_BY_STC89C52x // ��¼��ʱ����Ҫ����6Tģʽ

#define numLEDs 16		//�Ƶĸ���(����Խ����Ҫ�Ļ����ڴ�Խ��ÿ��������Ҫ3���ֽڣ�128byte����С�ڴ浥Ƭ����������29ֻ����)
#define MAX_BRIGHT  12	//���ȣ�0-(MAX_BRIGHT-1)��,�Ƽ�50���£���Ȼ̫������̫������������Ե�ʱ��5���Ҿ͹���
                        //���⣬����ɫ�㷨Ҫ�õ����������ȳ���3�����Ծ���ʹ���ܱ�3����������

// ��ȷ��ʱ��
#define delay1NOP()	    _nop_()
#define delay2NOP()	    ;delay1NOP();_nop_()
#define delay3NOP()	    ;delay2NOP();_nop_()
#define	delay4NOP()	    ;delay3NOP();_nop_()
#define	delay5NOP()	    ;delay4NOP();_nop_()
#define	delay6NOP()	    ;delay5NOP();_nop_()
#define	delay7NOP()	    ;delay6NOP();_nop_()
#define	delay8NOP()	    ;delay7NOP();_nop_()
#define	delay9NOP()	    ;delay8NOP();_nop_()
#define	delay10NOP()	;delay9NOP();_nop_()
#define	delay11NOP()	;delay10NOP();_nop_()
#define	delay12NOP()	;delay11NOP();_nop_()
#define	delay13NOP()	;delay12NOP();_nop_()
#define	delay14NOP()	;delay13NOP();_nop_()
#define	delay15NOP()	;delay14NOP();_nop_()
#define	delay16NOP()	;delay15NOP();_nop_()
#define	delay17NOP()	;delay16NOP();_nop_()
#define	delay18NOP()	;delay17NOP();_nop_()
#define	delay19NOP()	;delay18NOP();_nop_()
#define	delay20NOP()	;delay19NOP();_nop_()

// ���ʹ�õĵ�Ƭ�������Ѿ���֤�б����Ҷ�������Ļ������Գ����Լ���Ӻ������Լ���MCU����ʱ
#ifdef MCU_1T_11M_TEST_BY_STC15F10x
	#define delaySomeNOP() ;
#endif
#ifdef MCU_1T_24M_TEST_BY_STC15F10x
	#define delaySomeNOP() delay6NOP();
#endif
#ifdef MCU_1T_33M_TEST_BY_STC15F10x
	#define delaySomeNOP() delay1NOP();
#endif
#ifdef MCU_6T_32M_TEST_BY_STC89C52x
	#define delaySomeNOP() ;
#endif

// ��λ��ʱ ͬ����Ҫ�����Լ���Ƭ�������΢��(һ����˵�ϵ��ȵĸ������ˣ�������ȫ��ɫ���������������ʱһֱ��OKΪֹ)
// �����ʱ��Ҫ����50us������ʱ��Ҫ�󣬵����ɹ���
void Delay50us()
{
	#ifdef MCU_1T_11M_TEST_BY_STC15F10x || \
	       MCU_1T_24M_TEST_BY_STC15F10x || \
		   MCU_1T_33M_TEST_BY_STC15F10x
	unsigned char i, j;

	delay2NOP();
	i = 2;
	j = 150;
	do
	{
		while (--j)
			;
	} while (--i);
	#endif

	#ifdef MCU_6T_32M_TEST_BY_STC89C52x
		delay20NOP();delay20NOP();delay20NOP();delay20NOP();delay20NOP();
		delay20NOP();delay20NOP();delay20NOP();delay20NOP();delay20NOP();
		delay20NOP();delay20NOP();
	#endif
}

// ���������»����ϲ���Ҫ�޸� --------------------------------------
unsigned char buf_R[numLEDs] = {0}; //��ɫ����
unsigned char buf_G[numLEDs] = {0};
unsigned char buf_B[numLEDs] = {0};

// ������ʱ�����ڿ��ƶ����ٶ��ã����úܾ�ȷ
void HAL_Delay(unsigned int t)
{
	unsigned int x, y;
	for (x = 114; x > 0; x--)
		for (y = t; y > 0; y--)
			;
}

//����24λ����
//1�룬�ߵ�ƽ850ns �͵�ƽ400ns �������150ns
//0�룬�ߵ�ƽ400ns �͵�ƽ850ns �������150ns
void Send_2811_24bits(unsigned char G8, unsigned char R8, unsigned char B8)
{
	unsigned char n = 0;

	// ������CY�Ĵ���(���ƺ����λΪ1��CY�Զ���Ӳ����1)�������1��0֮����߱���1���߱��0��
	// �Ӷ�������1����0

	//����G8λ
	n=8;
	G8 <<= 1;
	while(n){
		WS2812 = 1;
		// �����Ƶ�ϸ߿��ڴ˴��ʵ�����_nop_():
		delaySomeNOP();
		// �������dat <<= 1;�����˴�Ҳ����
		WS2812 = CY;
		WS2812 = 0;
		G8 <<= 1;
		n--;
	}
	//����R8λ
	n=8;
	R8 <<= 1;
	while(n){
		WS2812 = 1;
		// �����Ƶ�ϸ߿��ڴ˴��ʵ�����_nop_():
		delaySomeNOP();
		// �������dat <<= 1;�����˴�Ҳ����
		WS2812 = CY;
		WS2812 = 0;
		R8 <<= 1;
		n--;
	}
	//����B8λ
	n=8;
	B8 <<= 1;
	while(n){
		WS2812 = 1;
		// �����Ƶ�ϸ߿��ڴ˴��ʵ�����_nop_():
		delaySomeNOP();
		// �������dat <<= 1;�����˴�Ҳ����
		WS2812 = CY;
		WS2812 = 0;
		B8 <<= 1;
		n--;
	}
		
	
}

//��λ��
void RGB_Rst()
{
	WS2812 = 0;
	Delay50us();
}
//��24λ����GRB��תRGB
void Set_Colour(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char i;
	for (i = 0; i < numLEDs; i++)
	{
		buf_R[i] = r; //����
		buf_G[i] = g;
		buf_B[i] = b;
	}
	for (i = 0; i < numLEDs; i++)
	{
		Send_2811_24bits(buf_G[i], buf_R[i], buf_B[i]); //������ʾ
	}
}
//ĳһ������ʾ����ɫ
void SetPointColour(unsigned int num, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char i;
	for (i = 0; i < numLEDs; i++)
	{
		buf_R[num] = r; //����
		buf_G[num] = g;
		buf_B[num] = b;
	}
	for (i = 0; i < numLEDs; i++)
	{
		Send_2811_24bits(buf_G[i], buf_R[i], buf_B[i]); //������ʾ
	}
}

//��ɫ����24λ����ַ�
void SetPixelColor(unsigned char num, unsigned long c)
{
	unsigned char i;
	for (i = 0; i < numLEDs; i++)
	{
		buf_R[num] = (unsigned char)(c >> 16);
		buf_G[num] = (unsigned char)(c >> 8);
		buf_B[num] = (unsigned char)(c);
	}
	for (i = 0; i < numLEDs; i++)
	{
		Send_2811_24bits(buf_G[i], buf_R[i], buf_B[i]);
	}
}

//��λ
void PixelUpdate()
{
	RGB_Rst();
}
//��ɫ
unsigned long Color(unsigned char r, unsigned char g, unsigned char b)
{
	return ((unsigned long)r << 16) | ((unsigned long)g << 8) | b;
}

//��ɫ�㷨
unsigned long Wheel(unsigned char WheelPos)
{
	WheelPos = (MAX_BRIGHT-1) - WheelPos;
	if (WheelPos < (MAX_BRIGHT/3))
	{
		return Color((MAX_BRIGHT-1) - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < (MAX_BRIGHT/3*2))
	{
		WheelPos -= (MAX_BRIGHT/3);
		return Color(0, WheelPos * 3, (MAX_BRIGHT-1) - WheelPos * 3);
	}
	WheelPos -= (MAX_BRIGHT/3*2);
	return Color(WheelPos * 3, (MAX_BRIGHT-1) - WheelPos * 3, 0);
}

//�ʺ�
void rainbow(unsigned int wait)
{
	unsigned int i, j;

	for (j = 0; j < MAX_BRIGHT; j++)
	{
		for (i = 0; i < numLEDs; i++)
		{
			SetPixelColor(i, Wheel((i + j) & (MAX_BRIGHT-1)));
		}
		PixelUpdate();
		HAL_Delay(wait);
	}
}

//��΢��ͬ���ǣ���ʹ�òʺ���ȷֲ�
void rainbowCycle(unsigned int wait)
{
	unsigned int i, j;
	PixelUpdate();
	for (j = 0; j < MAX_BRIGHT * 5; j++)
	{ // 5 cycles of all colors on wheel  ������������ɫ��5��ѭ��
		for (i = 0; i < numLEDs; i++)
		{
			SetPixelColor(i, Wheel(((i * MAX_BRIGHT / numLEDs) + j) & (MAX_BRIGHT-1)));
		}
		PixelUpdate();
		HAL_Delay(wait);
	}
}

//Theatre-style crawling lights. ÿ������(intervalCnt����ǵ��������Լ��)��һ����ƽ�ƣ�תȦ��
void theaterChase(unsigned long c, unsigned int intervalCnt, unsigned int times, int wait)
{
	int j, q;
	unsigned int i;
	PixelUpdate();
	for (j = 0; j < times; j++)
	{
		for (q = 0; q < intervalCnt; q++)
		{
			for (i = 0; i < numLEDs; i = i + intervalCnt)
			{
				SetPixelColor(i + q, c); //turn every intervalCnt pixel on
			}
			PixelUpdate();
			HAL_Delay(wait);

			for (i = 0; i < numLEDs; i = i + intervalCnt)
			{
				SetPixelColor(i + q, 0); //turn every intervalCnt pixel off
			}
			PixelUpdate();
		}
	}
}

// ��������ǿ����õ�ֻ��STC15F104ֻ��128byteС�ڴ治����������ʱע�͵��ˣ������ڴ��MCU�Ϳ���ʹ����
//Theatre-style crawling lights with rainbow effect
//���вʺ�Ч����Ϸ��ʽ���е�
void theaterChaseRainbow(unsigned int intervalCnt, unsigned int times, unsigned int wait)
{
	int j, q;
	unsigned int i;
	for (j = 0; j < times; j++)
	{ // cycle all MAX_BRIGHT colors in the wheel ��������ѭ������MAX_BRIGHTɫ
		for (q = 0; q < intervalCnt; q++)
		{
			for (i = 0; i < numLEDs; i = i + intervalCnt)
			{
				SetPixelColor(i + q, Wheel((i + j) % intervalCnt)); //turn every third pixel off ��ÿһ������������
			}
			PixelUpdate();

			HAL_Delay(wait);

			for (i = 0; i < numLEDs; i = i + intervalCnt)
			{
				SetPixelColor(i + q, 0); //turn every third pixel off  ��ÿһ�����������عص�
			}
		}
	}
}

// Fill the dots one after the other with a color
//��һ����ɫ�����ЩԲ�㣨��˳�����ε�����
void colorWipe(unsigned long c, unsigned int wait)
{
	unsigned int i = 0;
	for (i = 0; i < numLEDs; i++)
	{
		SetPixelColor(i, c);
		PixelUpdate();
		HAL_Delay(wait);
	}
}

unsigned char absSub(unsigned char a, unsigned char b){
	return a>b?a-b:b-a;
}
// ������
void breath(unsigned char gA, unsigned char rA, unsigned char bA,
 						unsigned char gB, unsigned char rB, unsigned char bB,
						unsigned int wait) {
	unsigned char maxInterval,g,r,b,i;
	maxInterval = absSub(gA, gB);
	if (maxInterval < absSub(rA, rB)) maxInterval = absSub(rA, rB);
	if (maxInterval < absSub(bA, bB)) maxInterval = absSub(bA, bB);
	i=maxInterval;
	while(i--){
		g = gA>gB ? gA - (absSub(gA, gB)/maxInterval)*(maxInterval-i) : gA + (absSub(gA, gB)/maxInterval)*(maxInterval-i);
		r = rA>rB ? rA - (absSub(rA, rB)/maxInterval)*(maxInterval-i) : rA + (absSub(rA, rB)/maxInterval)*(maxInterval-i);
		b = bA>bB ? bA - (absSub(bA, bB)/maxInterval)*(maxInterval-i) : bA + (absSub(bA, bB)/maxInterval)*(maxInterval-i);
		HAL_Delay(wait);
		colorWipe(Color(r, g, b), 1);
	}	
}


void Delay3000ms()		//@32.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 61;
	j = 204;
	k = 243;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void main()
{
	unsigned char r,g,b;

	#define C_SPEED 5

	// For Test ������ʱ��ʱ������ȳ���ֻ����һ����
	PixelUpdate();
	Send_2811_24bits(0, (MAX_BRIGHT-1), 0);
	Send_2811_24bits((MAX_BRIGHT-1), 0, 0);
	Send_2811_24bits(0, 0, (MAX_BRIGHT-1));
	// Send_2811_24bits(0, (MAX_BRIGHT-1), 0);
	// Send_2811_24bits((MAX_BRIGHT-1), 0, 0);
	// Send_2811_24bits(0, 0, (MAX_BRIGHT-1));
	// Send_2811_24bits(0, (MAX_BRIGHT-1), 0);
	// Send_2811_24bits((MAX_BRIGHT-1), 0, 0);
	// Send_2811_24bits(0, 0, (MAX_BRIGHT-1));
	// Send_2811_24bits(0, (MAX_BRIGHT-1), 0);
	// Send_2811_24bits((MAX_BRIGHT-1), 0, 0);
	// Send_2811_24bits(0, 0, (MAX_BRIGHT-1));
	// Send_2811_24bits(0, (MAX_BRIGHT-1), 0);
	// Send_2811_24bits((MAX_BRIGHT-1), 0, 0);
	// Send_2811_24bits(0, 0, (MAX_BRIGHT-1));
	// Send_2811_24bits(0, 0, (MAX_BRIGHT-1));

	PixelUpdate();
	//Delay3000ms();
	
	while (1)
	{
		rainbow(500/C_SPEED);
		rainbowCycle(500/C_SPEED);
		theaterChase(Color(10,0,MAX_BRIGHT), 4, 4, 1500/C_SPEED);
		theaterChase(Color(10,0,MAX_BRIGHT), 2, 4, 1500/C_SPEED);
		theaterChase(Color(0,MAX_BRIGHT,10), 2, 4, 1500/C_SPEED);
		theaterChase(Color(0,MAX_BRIGHT,10), 4, 4, 1500/C_SPEED);
		theaterChase(Color(MAX_BRIGHT,10,0), 4, 4, 1500/C_SPEED);
		theaterChase(Color(MAX_BRIGHT,10,0), 2, 4, 1500/C_SPEED);

		// ������ע�͵��ˣ���Ҫͬʱ�ſ�
		theaterChaseRainbow(4, 10, 1000/C_SPEED);theaterChaseRainbow(4, 9, 900/C_SPEED);theaterChaseRainbow(4, 8, 800/C_SPEED);
		theaterChaseRainbow(4, 7, 700/C_SPEED);theaterChaseRainbow(4, 6, 600/C_SPEED);theaterChaseRainbow(4, 5, 500/C_SPEED);
		theaterChaseRainbow(4, 4, 400/C_SPEED);theaterChaseRainbow(4, 3, 300/C_SPEED);theaterChaseRainbow(4, 2, 200/C_SPEED);
		theaterChaseRainbow(4, 20, 100/C_SPEED);theaterChaseRainbow(4, 20, 100/C_SPEED);
		theaterChaseRainbow(4, 2, 200/C_SPEED);theaterChaseRainbow(4, 3, 300/C_SPEED);theaterChaseRainbow(4, 4, 400/C_SPEED);
		theaterChaseRainbow(4, 5, 500/C_SPEED);theaterChaseRainbow(4, 6, 600/C_SPEED);theaterChaseRainbow(4, 7, 700/C_SPEED);
		theaterChaseRainbow(4, 8, 800/C_SPEED);theaterChaseRainbow(4, 9, 900/C_SPEED);theaterChaseRainbow(4, 10, 1000/C_SPEED);
		
		colorWipe(Color(MAX_BRIGHT,10,0),1000/C_SPEED);
		colorWipe(Color(10,0,MAX_BRIGHT),1000/C_SPEED);
		colorWipe(Color(0,10,MAX_BRIGHT),1000/C_SPEED);
		colorWipe(Color(0,MAX_BRIGHT,0),1000/C_SPEED);
		
		
		// ����ɫ����
		breath(MAX_BRIGHT,0,0,           0, MAX_BRIGHT, 0, 1000/C_SPEED);
		breath(0, MAX_BRIGHT, 0,           0,0,MAX_BRIGHT, 1000/C_SPEED);
		breath(0,0,MAX_BRIGHT,           MAX_BRIGHT,0,0, 1000/C_SPEED);
		breath(MAX_BRIGHT,0,0,           0, MAX_BRIGHT, MAX_BRIGHT, 1000/C_SPEED);
		breath(0,          MAX_BRIGHT, MAX_BRIGHT,  MAX_BRIGHT, MAX_BRIGHT, 0, 1000/C_SPEED);
		breath(MAX_BRIGHT, MAX_BRIGHT, 0,           MAX_BRIGHT, 0, MAX_BRIGHT, 1000/C_SPEED);
		breath(MAX_BRIGHT, 0, MAX_BRIGHT,           0,0,0, 1000/C_SPEED);
		breath(0,0,0,           MAX_BRIGHT,0,0, 1000/C_SPEED);
	}
}
