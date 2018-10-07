/*************************************************************
外接TLC5940芯片输出16路PWM控制LED亮度
晶振，无特殊要求，越高越好，建议不低于24M
MAX_GRAY_SCAL：输出PWM精度，最高为4096，是TCL5940的上限
MAX_BRIGHT：   输出的最大亮度，最高为MAX_GRAY_SCAL

硬件连接：
|-----------------------------------------|
|TLC5940		51单片机/外设             |
|-----------------------------------------|
|VPRG			GND                       |
|SIN			P3.1                      |
|SCLK			P3.2                      |
|XLAT			P3.3                      |
|BLANK			P3.4                      |
|GSCLK			P3.5                      |
|IREF			通过一个2-3K的电阻接地    |
|OUT0-15		接LED负极（LED正极接VCC） |
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
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	
	TL0 = 0xDF;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	
	
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}

void main () {
	
  uchar i,j,m,n;
  uchar r,g,b;
  
  Timer0Init();
  
  #if test
  // // VPRG设置为L，使其工作在GS mode (可用硬件直接拉低)
  // VPRG = 0;

  // // BLANK设置为H，关闭所有输出
  // BLANK = 1;

  // // 传送12bit X 16组PWM数值GSn(n=0-15)，共192bit
  // // 每组数据的值范围是0-4095
  // // 因为是通过移位寄存器传输，所以传送顺序是倒序的：GS15，GS14。。。GS0
  // // GSn决定了OUTn的PWM调宽。（GSn / 4095 = 0% - 100%）
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

  // // 送完GS数据后，创造XLAT的上升沿，将移位寄存器的数据一次性送入GS寄存器
  // XLAT = 0;
  // XLAT = 1;

  // // BLANK设置为L，打开所有输出
  // BLANK = 0;
  
  // 准备工作完毕，为了让TLC5940芯片正常工作，还需要向GSCLK输送时钟信号
  // 开启定时器0，开始输出方波信号（频率越快效果越好）
  // TLC5940会根据这个时钟信号进行从0-4095的计数（高低电平每交替一次数一次），
  // 一边计数一边检查各GSn的设定值，一旦到达GSn的值，则切换OUTn的电平一直计数到4095为止（或计数到BLANK拉高为止），
  // 再从0开始重新计数。从0计数到4095（或途中BLANK拉高时的计数）就是一个高低电平的切换周期。
  // 所以我们给5940提供的时钟频率越快，最后输出的PWM频率就越快。最后输出的PWM的频率 = 时钟信号的频率 / 4095（或Blank中途拉高时的计数，后面的4095同）。
  // 比如时钟频率是8M，则最后从各OUT口输出的PWM频率是8MHz / 4095 = 1.953KHz。
  // 根据TLC5940的官方文档，GSCLK可支持最高30MHz的时钟频率，也就是最高可以输出7.3KHz的PWM信号。
  // 如果是控制LED的亮度，那么让人眼感觉不到闪烁的最低频率应该是50Hz以上，
  // 所以我们应该至少给GSCLK提供不低于50 X 4095 = 200KHz的方波。
  // 实测：
  // 使用STC15F100系列单片机，指定内部晶振频率为33MHz，16位自动重载定时器，每1us切换一次GSCLK的电平，
  // 另外，通过Blank中途拉高的方法控制计数在200（而不是4096，否则会非常闪烁），
  // 这样，PWM的精度就可以在0-200之间指定。对于一般的应用足够了。
  // 如果需要更高精度比如想用足该芯片支持的4096级精度，则需要更高速度的GSCLK输出才行，可以考虑使用ALE时钟分频输出，
  // 但这样不太好准确计数时钟输出的次数（应该在每输出4096次的时候拉高再拉低一次blank以清除计数，TLC5940自己不会自动清除计数的），
  // 所以什么时候拉高BLANK是个问题，需要仔细调试，这是题外话
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

// 显示模式1，每个OUT都从亮变暗，且 从OUT0到OUT15 每路输出都有一个相对亮度差
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
				// LED编号处于当前旋转点亮的区域内的判断，注意首位相接
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
			
			// 为了视觉上的统一，根据LED实际的排列，微调每行之间的间隔
			// 5-9行的间距是一样的，使用参数指定的间隔
			// 4-5行间隔是标准的3/2，3-4行间隔最短几乎同时，2-3行比3-4行稍长，1-2行大概是标准的一半
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
