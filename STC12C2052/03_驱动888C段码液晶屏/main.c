/*********************************************************************************************/
/* 测试单片机 STC15F100系列单片机 测试段码液晶屏[P3.0-P3.5依次输出交替高低电平]              */
/*********************************************************************************************/

#include<STC12x20xx.h>


#define uchar unsigned char
#define uint unsigned int

sbit PIN1_COM4 = P1^7;
sbit PIN2_COM3 = P1^6;
sbit PIN3_COM2 = P1^5;
sbit PIN4_COM1 = P1^4;

sbit PIN5 = P1^3;  // 1E, 1G, 1F, 4G
sbit PIN6 = P1^2;  // 1D, 1C, 1B, 1A
sbit PIN7 = P1^1;  // 2E, 2G, 2F, 4D
sbit PIN8 = P1^0;  // 2D, 2C, 2B, 2A
sbit PIN9 = P3^7;  // 3E, 3G, 3F, 4A
sbit PIN10 = P3^5; // 3D, 3C, 3B, 3A

// map[有几个COM，每个com对应几段， 每段的定义一般为2位不变]
// 比如888C这种段码屏，4个com
// 每个com控制6个段，比如4G，1A，4D，2A，4A，3A，每段用0x1A这种形式表示，1A表示第一位数字的A段。碰到G段用9表示（16进制里没有G）。
uchar code map_com[4][6] = { 
	{0x49, 0x1A, 0x4D, 0x2A, 0x4A, 0x3A},
	{0x1F, 0x1B, 0x2F, 0x2B, 0x3F, 0x3B},
	{0x19, 0x1C, 0x29, 0x2C, 0x39, 0x3C},
	{0x1E, 0x1D, 0x2E, 0x2D, 0x3E, 0x3D}
};

void setPinValueByPinNum(uchar nPinNum, uchar value);
void setValueByCom(uchar nCom, uchar nDig, uchar num);
void setValue(uchar nCom);
void Timer0Init();
void delay(uint t);
void delay(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
	for(j=0;j<250;j++);
}

uchar nowCom=1; // 当前刷新到第几个COM(1-4)
int ttMs=0;
int ttSec=0;

int numForShow = 0;

uchar idxTmp;

main()
{
	// 上电默认是准双向口
	// 7-0位设置为1的话跟P1M1的对应的位结合起来看，如果是00则是准双向口，如果是10则是输入态，也就是高阻态
	P1M0 = 0x00;
	P1M1 = 0x00;

	P33 = 0;
	P34 = 0;

	Timer0Init();

	while(1){
		
	}
}

void Timer0Init()		//100微秒@5.019MHz

{
	EA=1;		// 各种中断总开关打开
    ET0=1;		// 定时器0有效

	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x02;		//设置定时器模式
	TL0 = 0xD6;		//设置定时初值
	TH0 = 0xD6;		//设置定时重载值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void timer0() interrupt 1
{
	TR0 = 0;

	ttMs++;
	ttSec++;

	//P1M0 = 0xF0;
	// 不停置反com跟seg的电平
	PIN4_COM1 = ~PIN4_COM1;
	PIN3_COM2 = ~PIN3_COM2;
	PIN2_COM3 = ~PIN2_COM3;
	PIN1_COM4 = ~PIN1_COM4;

	PIN5  = ~PIN5;
	PIN6  = ~PIN6;
	PIN7  = ~PIN7;
	PIN8  = ~PIN8;
	PIN9  = ~PIN9;
	PIN10  = ~PIN10;


	// 切换COM
	if (ttMs >= 30) // 1mS
	{
		ttMs=0;

		nowCom++;
		if (nowCom > 4)
		{
			nowCom = 1;
		}

		// reset all pins
		for (idxTmp = 1; idxTmp <= 10; ++idxTmp)
		{
			setPinValueByPinNum(idxTmp, 0);
		}

		P1M0 = 0xF0;// 先将4个COM全部设置高阻态

		//setValue(nowCom);// 设置新的内容
		setValueByCom(nowCom, 1, numForShow % 1000 / 100);
		setValueByCom(nowCom, 2, numForShow % 100 / 10);
		setValueByCom(nowCom, 3, numForShow % 10);
		setValueByCom(nowCom, 4, 10);// wendu mark C

		// 打开其中一个COM
		switch(nowCom){
			case 1:
				// 设置COM1为准双向口，其他3个为高阻态(高阻态COM相关的段不点亮)
				P1M0 = 0xE0; // 1110 0000 (高4位为com4-com1，低4位是seg段用，一直保持准双向口)
				break;
			case 2:
				// 设置COM2为准双向口，其他3个为高阻态
				P1M0 = 0xD0; // 1101 0000
				break;
			case 3:
				// 设置COM3为准双向口，其他3个为高阻态
				P1M0 = 0xB0; // 1011 0000
				break;
			case 4:
				// 设置COM4为准双向口，其他3个为高阻态
				P1M0 = 0x70; // 0111 0000
				break;
		}
	}

	// 显示数字加一
	if (ttSec >= 10000) // 1S
	{
		ttSec = 0;
		numForShow++;
		//P34 = ~P34;
	}

	TR0 = 1;
}


// 指定的com(1-4)，指定位(1-4)，指定内容（数字）
void setValueByCom(uchar nCom, uchar nDig, uchar num ){
	uchar i = 0;
	uchar mapComVal;
	uchar pinNum;

	// 指定COM置低电平
	setPinValueByPinNum(nCom, 0);

	for (pinNum = 5; pinNum <= 10; ++pinNum)
	{
		mapComVal = map_com[nCom-1][pinNum-5];
		switch(num){
			case 0:
				if (//ABCDEF
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 1:
				if (//BC
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 2:
				if (//ABDEG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 3:
				if (//ABCDG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 4:
				if (//BCFG(9 is G)
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 5:
				if (//ACDFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 6:
				if (//ACDEFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 7:
				if (//ABC
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 8:
				if (//ABCDEFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 9:
				if (//ABCDFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 10:	// 第4位的特殊符号摄氏度
				if (//AD
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
		}
	}
}

void setPinValueByPinNum(uchar nPinNum, uchar value) {
	switch(nPinNum){
		case 1:
			PIN1_COM4 = value;
			break;
		case 2:
			PIN2_COM3 = value;
			break;
		case 3:
			PIN3_COM2 = value;
			break;
		case 4:
			PIN4_COM1 = value;
			break;
		case 5:
			PIN5 = value;
			break;
		case 6:
			PIN6 = value;
			break;
		case 7:
			PIN7 = value;
			break;
		case 8:
			PIN8 = value;
			break;
		case 9:
			PIN9 = value;
			break;
		case 10:
			PIN10 = value;
			break;
	}
}

// dummy show 123C
void setValue(uchar nCom){
	switch(nCom){
		case 1:
			PIN4_COM1 = 0;

			PIN5 = 0;
			PIN6 = 0;
			PIN7 = 1;
			PIN8 = 1;
			PIN9 = 1;
			PIN10 = 1;
			break;
		case 2:
			PIN3_COM2 = 0;
			
			PIN5 = 0;
			PIN6 = 1;
			PIN7 = 0;
			PIN8 = 1;
			PIN9 = 0;
			PIN10 = 1;
			break;
		case 3:
			PIN2_COM3 = 0;
			
			PIN5 = 0;
			PIN6 = 1;
			PIN7 = 1;
			PIN8 = 0;
			PIN9 = 1;
			PIN10 = 1;
			break;
		case 4:
			PIN1_COM4 = 0;
			
			PIN5 = 0;
			PIN6 = 0;
			PIN7 = 1;
			PIN8 = 1;
			PIN9 = 0;
			PIN10 = 1;
			break;
		}
}