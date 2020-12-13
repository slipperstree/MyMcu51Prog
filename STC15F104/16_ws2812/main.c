/********************************************************/
/*作 者：芒果
/*修改日期：2020.11.23
/*版    本：V1.4
/*程序功能：用51单片机控制ws2812 RGB输出，显示全色彩
/*备        注：51单片机晶振11.0592M/24M/33M@STC15L104E测试通过，其他型号需要根据实际需要调整延时部分
/********************************************************/

//#include<reg52.h>
#include "STC15104E.h"
#include "intrins.h"

sbit WS2812 = P3^5;

// 已经验证过的单片机，其他单片机和频率下不保证可以正常运行（1T·/6T/12T和晶振频率一致的话应该是通用的）
// 不到1T的单片机就不用考虑32M以下的晶振了，太慢了不能满足时序
// #define MCU_1T_11M_TEST_BY_STC15F10x
// #define MCU_1T_24M_TEST_BY_STC15F10x
// #define MCU_1T_33M_TEST_BY_STC15F10x
#define MCU_6T_32M_TEST_BY_STC89C52x // 烧录的时候需要开启6T模式

#define numLEDs 16		//灯的个数(灯珠越多需要的缓存内存越多每个灯珠需要3个字节，128byte以下小内存单片机最多可驱动29只灯珠)
#define MAX_BRIGHT  12	//亮度（0-(MAX_BRIGHT-1)）,推荐50以下，不然太亮电流太大带不动，调试的时候5左右就够了
                        //另外，渐变色算法要用到这个最大亮度除以3，所以尽量使用能被3整除的数字

// 精确延时用
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

// 如果使用的单片机不在已经验证列表中且都有问题的话，可以尝试自己添加宏适配自己的MCU的延时
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

// 复位延时 同样需要根据自己单片机的情况微调(一般来说上电后等的个数对了，但是是全白色，就增大这里的延时一直到OK为止)
// 这个延时需要大于50us以满足时序要求，但不可过久
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

// 从这里以下基本上不需要修改 --------------------------------------
unsigned char buf_R[numLEDs] = {0}; //颜色缓存
unsigned char buf_G[numLEDs] = {0};
unsigned char buf_B[numLEDs] = {0};

// 粗略延时，用于控制动画速度用，不用很精确
void HAL_Delay(unsigned int t)
{
	unsigned int x, y;
	for (x = 114; x > 0; x--)
		for (y = t; y > 0; y--)
			;
}

//发送24位数据
//1码，高电平850ns 低电平400ns 误差正负150ns
//0码，高电平400ns 低电平850ns 误差正负150ns
void Send_2811_24bits(unsigned char G8, unsigned char R8, unsigned char B8)
{
	unsigned char n = 0;

	// 这里用CY寄存器(左移后最高位为1则CY自动被硬件置1)巧妙的在1和0之间或者保持1或者变成0，
	// 从而发送码1或码0

	//发送G8位
	n=8;
	G8 <<= 1;
	while(n){
		WS2812 = 1;
		// 如果主频较高可在此处适当增加_nop_():
		delaySomeNOP();
		// 将下面的dat <<= 1;移至此处也可以
		WS2812 = CY;
		WS2812 = 0;
		G8 <<= 1;
		n--;
	}
	//发送R8位
	n=8;
	R8 <<= 1;
	while(n){
		WS2812 = 1;
		// 如果主频较高可在此处适当增加_nop_():
		delaySomeNOP();
		// 将下面的dat <<= 1;移至此处也可以
		WS2812 = CY;
		WS2812 = 0;
		R8 <<= 1;
		n--;
	}
	//发送B8位
	n=8;
	B8 <<= 1;
	while(n){
		WS2812 = 1;
		// 如果主频较高可在此处适当增加_nop_():
		delaySomeNOP();
		// 将下面的dat <<= 1;移至此处也可以
		WS2812 = CY;
		WS2812 = 0;
		B8 <<= 1;
		n--;
	}
		
	
}

//复位码
void RGB_Rst()
{
	WS2812 = 0;
	Delay50us();
}
//把24位数据GRB码转RGB
void Set_Colour(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char i;
	for (i = 0; i < numLEDs; i++)
	{
		buf_R[i] = r; //缓冲
		buf_G[i] = g;
		buf_B[i] = b;
	}
	for (i = 0; i < numLEDs; i++)
	{
		Send_2811_24bits(buf_G[i], buf_R[i], buf_B[i]); //发送显示
	}
}
//某一个点显示的颜色
void SetPointColour(unsigned int num, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char i;
	for (i = 0; i < numLEDs; i++)
	{
		buf_R[num] = r; //缓冲
		buf_G[num] = g;
		buf_B[num] = b;
	}
	for (i = 0; i < numLEDs; i++)
	{
		Send_2811_24bits(buf_G[i], buf_R[i], buf_B[i]); //发送显示
	}
}

//颜色交换24位不拆分发
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

//复位
void PixelUpdate()
{
	RGB_Rst();
}
//颜色
unsigned long Color(unsigned char r, unsigned char g, unsigned char b)
{
	return ((unsigned long)r << 16) | ((unsigned long)g << 8) | b;
}

//颜色算法
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

//彩虹
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

//稍微不同的是，这使得彩虹均匀分布
void rainbowCycle(unsigned int wait)
{
	unsigned int i, j;
	PixelUpdate();
	for (j = 0; j < MAX_BRIGHT * 5; j++)
	{ // 5 cycles of all colors on wheel  车轮上所有颜色的5个循环
		for (i = 0; i < numLEDs; i++)
		{
			SetPixelColor(i, Wheel(((i * MAX_BRIGHT / numLEDs) + j) & (MAX_BRIGHT-1)));
		}
		PixelUpdate();
		HAL_Delay(wait);
	}
}

//Theatre-style crawling lights. 每隔几个(intervalCnt最好是灯珠个数的约数)亮一个并平移（转圈）
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

// 这个函数是可以用的只是STC15F104只有128byte小内存不够用所以暂时注释掉了，换大内存的MCU就可以使用了
//Theatre-style crawling lights with rainbow effect
//带有彩虹效果的戏剧式爬行灯
void theaterChaseRainbow(unsigned int intervalCnt, unsigned int times, unsigned int wait)
{
	int j, q;
	unsigned int i;
	for (j = 0; j < times; j++)
	{ // cycle all MAX_BRIGHT colors in the wheel 在轮子上循环所有MAX_BRIGHT色
		for (q = 0; q < intervalCnt; q++)
		{
			for (i = 0; i < numLEDs; i = i + intervalCnt)
			{
				SetPixelColor(i + q, Wheel((i + j) % intervalCnt)); //turn every third pixel off 把每一个第三个像素
			}
			PixelUpdate();

			HAL_Delay(wait);

			for (i = 0; i < numLEDs; i = i + intervalCnt)
			{
				SetPixelColor(i + q, 0); //turn every third pixel off  把每一个第三个像素关掉
			}
		}
	}
}

// Fill the dots one after the other with a color
//用一种颜色填充这些圆点（按顺序依次点亮）
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
// 呼吸灯
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

	// For Test 调试延时的时候可以先尝试只点亮一个灯
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

		// 函数被注释掉了，需要同时放开
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
		
		
		// 渐变色测试
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
