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
DISP_MODE01 定义说明

 数组共17个数字。
  第一个数字指明一个动画循环里包含几帧，比如9表示整个动画分9个帧
  后面的16个数字分别表示16只灯应该在动画的第n帧之后（而不是只在第n帧）被点亮（或被熄灭，取决于 OFF_TO_ON or ON_TO_OFF）
     也就是： 当前帧数 >= 某位置上的数字  时，该灯被点亮
  比如下面这个动画一共9帧
     中间那只灯从第一帧开始到最后一帧一直被点亮
	 左右最外侧的两只灯会在第5帧开始被点亮
	 第9帧的时候，所有灯都被点亮

        3          3
     4    2    [2]   4
    5		(1)		  5
					 
	  6				6
	    7		  7
		  8		8
			 9

  根据需要先画出上面的示意图，
     数组第一个数字代表帧数
	 第2-17个数字从[]位置开始顺时针旋转到()结束
	 比如上面这个动画的数组定义应该是：   
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
  Timer0Init();

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

// 显示模式6，心跳模式
// maxBrightPer ：最高亮度
// speed        ：跳动速度
// cnt          ：跳动次数
void disp_heartbeat(uint maxBrightPer, uchar speed, uchar cnt) {

	char nowBright = 0;
	uchar nCnt = 0;

	uint maxBright, minBright = 0;

	uint offsetBright = 1; // 每次亮度改变的幅度，值越小变化越顺滑

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

