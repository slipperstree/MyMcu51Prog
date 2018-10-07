/*------------------------------------------------------------------*/
/* --- STC MCU Limited ---------------------------------------------*/
/* --- STC12C56xx Series MCU A/D Conversion Demo -------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966----------------------*/
/* --- Web: www.STCMCU.com -----------------------------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

#include "STC12x20xx.h"
#include "intrins.h"

#define FOSC    11059200L
#define BAUD    9600

#define uchar unsigned char
#define uint unsigned int

typedef unsigned char BYTE;
typedef unsigned int WORD;

/*Declare SFR associated with the ADC */
//sfr AUXR        =   0x8e;
//sfr ADC_CONTR   =   0xC5;           //ADC control register
//sfr ADC_DATA    =   0xC6;           //ADC high 8-bit result register
//sfr ADC_LOW2    =   0xBE;           //ADC low 2-bit result register
//sfr P1M0        =   0x91;           //P1 mode control register0
//sfr P1M1        =   0x92;           //P1 mode control register1

/*Define ADC operation const for ADC_CONTR*/
#define ADC_POWER   0x80            //ADC power control bit
#define ADC_FLAG    0x10            //ADC complete flag
#define ADC_START   0x08            //ADC start control bit
#define ADC_SPEEDLL 0x00            //420 clocks
#define ADC_SPEEDL  0x20            //280 clocks
#define ADC_SPEEDH  0x40            //140 clocks
#define ADC_SPEEDHH 0x60            //70 clocks

void InitUart();
void SendData(BYTE dat);
void Delay(WORD n);
void InitADC();

BYTE ch = 0;                        //ADC channel NO. 0:使用P1.0作为AD输入

uint i;
uint tt;
uint now;
uint isBtnPushing;

// 按最小单位计算可以最大定时多少份
// 数字越大每升高/降低一个时间单位时旋钮转动所需要的角度越小，但相对不稳定（相邻的刻度越近越容易跳来跳去）
// 数字越小每升高/降低一个时间单位时旋钮转动所需要的角度越大，也越稳定
#define TIME_PART 40

// 倒计时最小单位（秒）
// 目前分几个阶段
// 5分钟以内，30秒为单位
#define TIME_STEP1_LIMIT (5 * 60)
#define TIME_STEP1_UNIT 30
#define TIME_STEP1_STEPS (TIME_STEP1_LIMIT/TIME_STEP1_UNIT)
// 5分钟以上20分以内，1分钟为单位
#define TIME_STEP2_LIMIT (20 * 60)
#define TIME_STEP2_UNIT (1 * 60)
#define TIME_STEP2_STEPS (TIME_STEP1_STEPS+(TIME_STEP2_LIMIT-TIME_STEP1_LIMIT)/TIME_STEP2_UNIT)
// 20分钟以上，5分钟为单位
#define TIME_STEPLAST_UNIT (5 * 60)

#define TIME_UNIT 30 // 倒计时最小单位（秒）

uint alermSec;		// 计算用倒计时剩余秒数
uint alermDisp;		// 倒计时内容显示用(由倒计时剩余秒数计算得出)

// 当前状态
uchar status;
#define STATUS_SETTING 1 // 设置模式（此模式下，数码管闪烁，AD转换有效）
#define STATUS_WORKING 2 // 工作模式（此模式下，数码管不闪烁，AD转换无效，进行倒计时）
#define STATUS_POWER_DOWN 3 // 掉电模式（此模式下，只有外部中断继续工作用于唤醒系统，其他均无效，最省电的模式）

uint ttPowerDownSetting = 0;		// 设置模式时，无操作进入掉电模式的时间
uint ttPowerDownBeeping = 0;		// 鸣响时，无操作进入掉电模式的时间，跟设置的无操作时间分开是因为鸣响允许的时间稍微长一些
#define T_POWER_DOWN_SETTING ((T_BLANK_INTERVAL_ON + T_BLANK_INTERVAL_OFF) * 15)  // 设置状态数码管闪烁15次的时间
#define T_POWER_DOWN_BEEPING ((T_BLANK_INTERVAL_ON + T_BLANK_INTERVAL_OFF) * 30)  // 设置状态数码管闪烁40次的时间

uint ttBlank;		// 设置状态时闪烁计时用
#define T_BLANK_INTERVAL_ON 300 	// 闪烁时间间隔(不是秒单位，跟晶振速度有关需要调整)
#define T_BLANK_INTERVAL_OFF 100 	// 闪烁时间间隔(不是秒单位，跟晶振速度有关需要调整)
uchar stsBlanking;	// 当前的闪烁状态 1：显示中 0：关闭中
#define STS_BLANK_ON	1
#define STS_BLANK_OFF	0

// 工作模式时，为了省电，显示倒计时几秒钟以后就关闭大部分的数码管
// 只显示指定的内容（比如一个闪烁的点表示正在计时）
uchar isShowDummy = 0;
uint showDummySec = 0;
#define T_SHOW_DUMMY 3 // 倒计时显示n秒自动关闭屏幕省电(想一直能看见时间的话就设置一个很大的值即可)
uchar stsDummy=1;
uchar stsDummyLast=4;

uchar isBeeping;	// 蜂鸣器是否应该鸣响（倒计时时间到）  1：鸣响  0：不鸣响
uint ttBeep;
#define T_BEEP_INTERVAL_ON 40 	// 哔哔哔哔的时间间隔(不是秒单位，跟晶振速度有关需要调整)
#define T_BEEP_INTERVAL_OFF 250 // 两次哔哔哔哔之间的时间间隔(不是秒单位，跟晶振速度有关需要调整)
uchar stsBeep;		// 当前的BEEP鸣响状态

uint adcDataDisp;		// 最终显示在数码管上的采样结果
#define ADC_AVR_CNT 10 	// 多次采样次数
uchar nowAdcAvrCnt = 0;	// 当前采样次数
uint adcDataAvr;		// 采样平均值

void init(void);
void setBitData(uchar);
void showDigit(uchar, uchar, uchar);
void btnPressed();
void btnPressing();
void btnReleased();

sbit LED_DIO = P1^5;	// 串行数据输入
sbit LED_RCLK = P1^6;	// 锁存信号——上升沿有效
sbit LED_SCLK = P1^7;	// 串行数据时钟信号————上升沿有效

sbit btn = P1^4;	// 按钮按下接到GND拉低（同时按钮还接到INT0脚上，作为外部中断）

sbit beep = P1^3;	// 蜂鸣器 低电平响

void setBitData(uchar bitData){
	LED_DIO =  bitData;
	LED_SCLK = 0;
	LED_SCLK = 1;
}

// 指定no(1-4)号数码管显示数字num(0-9)，第三个参数是显示不显示小数点（1/0）
// num设置为10表示关闭这一位的输出
void showDigit(uchar no, uchar num, uchar showDotPoint){
	if (no == 1) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
	}
	else if (no == 2) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
		setBitData(0);
	}
	else if (no == 3) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
		setBitData(0);
		setBitData(0);
	}
	else if (no == 4) {
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(0);
		setBitData(1);
		setBitData(0);
		setBitData(0);
		setBitData(0);
	}

	if (num == 10)
	{
		setBitData(showDotPoint);
		setBitData(1)           ;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
		setBitData(1)       	;
	}
	else{
		if (num == 0) {
			setBitData(showDotPoint);
			setBitData(1)           ;
			setBitData(0)       	;
			setBitData(0)       	;
			setBitData(0)       	;
			setBitData(0)       	;
			setBitData(0)       	;
			setBitData(0)       	;
		}
		else if (num == 1) {
			setBitData(showDotPoint) ;
			setBitData(1)            ;
			setBitData(1)            ;
			setBitData(1)            ;
			setBitData(1)            ;
			setBitData(0)            ;
			setBitData(0)            ;
			setBitData(1)            ;
		}
		else if (num == 2) {
			setBitData(showDotPoint) ;
			setBitData(0)                ;
			setBitData(1)                ;
			setBitData(0)                ;
			setBitData(0)                ;
			setBitData(1)                ;
			setBitData(0)                ;
			setBitData(0)                ;
		}
		else if (num == 3) {
			setBitData(showDotPoint)     ;
			setBitData(0)                ;
			setBitData(1)                ;
			setBitData(1)                ;
			setBitData(0)                ;
			setBitData(0)                ;
			setBitData(0)                ;
			setBitData(0)                ;
		}
		else if (num == 4) {
			setBitData(showDotPoint);
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(1)           ;
			setBitData(1)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(1)           ;
		}
		else if (num == 5) {
			setBitData(showDotPoint);
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(1)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(1)           ;
			setBitData(0)           ;
		}
		else if (num == 6) {
			setBitData(showDotPoint);
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(1)           ;
			setBitData(0)           ;
		}
		else if (num == 7) {
			setBitData(showDotPoint);
			setBitData(1)           ;
			setBitData(1)           ;
			setBitData(1)           ;
			setBitData(1)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
		}
		else if (num == 8) {
			setBitData(showDotPoint);
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
			setBitData(0)           ;
		}
		else if (num == 9) {
			setBitData(showDotPoint) ;
			setBitData(0)            ;
			setBitData(0)            ;
			setBitData(1)            ;
			setBitData(0)            ;
			setBitData(0)            ;
			setBitData(0)            ;
			setBitData(0)            ;
		}
	}
	
	LED_RCLK = 1;
	LED_RCLK = 0;
}

void init(void)
{
    // 在STC12C2052AD的内部RC固定频率5.126Mhz的情况下
    // 此初值为每次中断为1ms
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x16;			//设置定时初值
	TH0 = 0xEC;			//设置定时初值
	TF0 = 0;			//清除TF0标志
    
    IT0=1;		// 外部中断0设置为下降沿触发(0为低电平触发)
    EX0=1;		// 允许外部中断0

    EA=1;		// 各种中断总开关打开
    ET0=1;		// 定时器0有效
    TR0=1;		// 定时器0开始运行

    LED_RCLK = 0;
	LED_SCLK = 0;
    
    status = STATUS_SETTING; // 上电初始状态为设置中(闪烁)
    now = 0;
    isBtnPushing = 0;
    beep = 1;		// 上电初始状态为不鸣叫（低电平时鸣叫）
    ttBlank = 0;
    stsBlanking = STS_BLANK_ON;
    alermDisp = 0;
    adcDataDisp = 0;
    isBeeping = 0;	// 上电初始状态为不鸣叫（低电平时鸣叫）
    isShowDummy = 0;
    showDummySec = 0;
}

void main()
{
    //InitUart();                   //Init UART, use to show ADC result
    InitADC();                      //Init ADC sfr
    AUXR |= 0x10;                   //set EADCI
    IE = 0xa0;                      //Enable ADC interrupt and Open master interrupt switch
                                    //Start A/D conversion
    
    init();
	while(1){
		btn = 1; // 取状态之前先要设置高电平
		if (isBtnPushing == 0 && btn == 0)
		{
			// 防抖动，延时一小会
			for (i = 0; i < 1000; ++i){;}
				if (isBtnPushing == 0 && btn == 0) {
					isBtnPushing = 1; // 按钮为按下的状态++++
					btnPressed();
				}
		}

		if (isBtnPushing == 1 && btn == 0){
			btnPressing();
		}

		if (isBtnPushing == 1 && btn == 1){
			// 判定按钮被放开
			isBtnPushing = 0;
			btnReleased();
		}

		// 如果是设置状态 控制数码管显示采样的数字 并闪烁输出
		if (status == STATUS_SETTING)
		{
			if (stsBlanking == STS_BLANK_OFF)
			{
				// 数字10作为约定好的特殊参数，所有数码管的段均关闭输出
				showDigit(1, 10, 1);
				showDigit(2, 10, 1);
				showDigit(3, 10, 1);
				showDigit(4, 10, 1);
			} else {
				// 设置过程中显示的不是倒计时的内容而是adc采样到的内容
				showDigit(1, adcDataDisp / 1000, 1);
				showDigit(2, adcDataDisp % 1000 / 100, 0);
				showDigit(3, adcDataDisp % 100 / 10, 1);
				showDigit(4, adcDataDisp % 10, 1);
			}
		}
		// 如果不是设置状态
		else {
			// 如果省电模式（显示少量信息表示还在运行即可）
			if (isShowDummy == 1 && stsDummy != stsDummyLast)
			{
				if (stsDummy == 1)
				{
					showDigit(1, 10, 0); // 点亮一个点
					showDigit(2, 10, 1);
					showDigit(3, 10, 1);
					showDigit(4, 10, 1);
				} 
				else if (stsDummy == 2)
				{
					showDigit(1, 10, 1); // 点亮一个点
					showDigit(2, 10, 0);
					showDigit(3, 10, 1);
					showDigit(4, 10, 1);
				}
				else if (stsDummy == 3)
				{
					showDigit(1, 10, 1); // 点亮一个点
					showDigit(2, 10, 1);
					showDigit(3, 10, 0);
					showDigit(4, 10, 1);
				}
				else if (stsDummy == 4)
				{
					showDigit(1, 10, 1); // 点亮一个点
					showDigit(2, 10, 1);
					showDigit(3, 10, 1);
					showDigit(4, 10, 0);
				}
			}
			else
			{
				// 正常显示倒计时内容
				showDigit(1, alermDisp / 1000, 1);
				showDigit(2, alermDisp % 1000 / 100, 0);
				showDigit(3, alermDisp % 100 / 10, 1);
				showDigit(4, alermDisp % 10, 1);
			}
		}
	}
}

void btnPressed()
{
	if (status == STATUS_SETTING)
	{
		// 从设置状态切换到工作状态（倒计时状态）
		// 取得当前设定的时间，同时为了计算方便，根据这个时间换算出秒数
		alermDisp = adcDataDisp;
		alermSec = alermDisp / 100 * 60 + alermDisp % 100;
		status = STATUS_WORKING;
		isShowDummy = 0;

		// 同时掉电计数变量复位(工作模式下不进入掉电模式)
		ttPowerDownSetting = 0;
		ttPowerDownBeeping = 0;
	} else {
		if (status == STATUS_WORKING && isBeeping)
		{
			isBeeping = 0;
			beep = 1;

			// 从鸣响状态回到设置状态
			status = STATUS_SETTING;
			// 开始ADC采样
			ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
		}
		else if (status == STATUS_WORKING && isShowDummy == 1)
		{
			// 如果当前处于省电模式下，按下按钮表示想看一下倒计时的剩余时间，此时不切换为设置状态
			// 只是从省电模式回到正常模式（当然这个模式持续指定时间后又会自动回到省电模式）
			isShowDummy = 0;
			showDummySec = 0;
		}
		else
		{
			// 从工作状态回到设置状态（包含了从掉电模式醒来以后的“假性”工作状态）
			status = STATUS_SETTING;
			// 开始ADC采样
			ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
		}

		// 保证在任何时候按下按钮都能中止鸣响
		isBeeping = 0;
		beep = 1;
	}
}

void btnPressing()
{
	// 按钮按下中
	//beep = 0;
}

void btnReleased()
{
	//beep = 1;
}

void timer0() interrupt 1
{
	// 工作状态 并且 蜂鸣器没有鸣响时 才进行倒计时
	if (status == STATUS_WORKING && isBeeping == 0)
	{
		tt++;
		// 手动调整，620次中断大约为1s
	    if (tt >= 620) {
	    	tt = 0;
	    	alermSec--;
	    	alermDisp = alermSec / 60 * 100 + alermSec % 60;
	    	if (alermSec == 0)
	    	{
	    		isBeeping = 1;
	    	}

	    	if (isShowDummy == 0)
	    	{
	    		showDummySec++;
	    		if (showDummySec > T_SHOW_DUMMY)
		    	{
		    		// 切换到省电模式
		    		isShowDummy = 1;
		    		showDummySec = 0;
		    	}
	    	}
	    	else
	    	{
	    		stsDummyLast = stsDummy;
	    		stsDummy++;
	    		if (stsDummy > 4)
	    		{
	    			stsDummy = 1;
	    		}
	    	}
	    }
	}

    // 设置状态下，控制LED闪烁
    if (status == STATUS_SETTING)
    {
    	ttBlank++;
	    if (stsBlanking == STS_BLANK_OFF && ttBlank >= T_BLANK_INTERVAL_OFF) {
	    	ttBlank = 0;
	    	stsBlanking = STS_BLANK_ON;
	    } else if (stsBlanking == STS_BLANK_ON && ttBlank >= T_BLANK_INTERVAL_ON)
	    {
	    	ttBlank = 0;
	    	stsBlanking = STS_BLANK_OFF;
	    }

	    // 设置状态下，一定时间无操作则进入掉电模式
	    ttPowerDownSetting++;
    }

    // 蜂鸣器鸣响时，控制哔哔哔哔的效果
    if (isBeeping == 1)
    {
    	ttBeep++;
    	if (stsBeep == 0 && ttBeep >= T_BEEP_INTERVAL_OFF)
    	{
    		stsBeep = 1;
    		beep = 0;
    		ttBeep = 0;
    	} else if (stsBeep == 1 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 2;
    		beep = 1;
    		ttBeep = 0;
    	} else if (stsBeep == 2 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 3;
    		beep = 0;
    		ttBeep = 0;
    	} else if (stsBeep == 3 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 4;
    		beep = 1;
    		ttBeep = 0;
    	} else if (stsBeep == 4 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 5;
    		beep = 0;
    		ttBeep = 0;
    	} else if (stsBeep == 5 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 6;
    		beep = 1;
    		ttBeep = 0;
    	} else if (stsBeep == 6 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 7;
    		beep = 0;
    		ttBeep = 0;
    	} else if (stsBeep == 7 && ttBeep >= T_BEEP_INTERVAL_ON)
    	{
    		stsBeep = 0;
    		beep = 1;
    		ttBeep = 0;
    	}

    	// 鸣响状态下，一定时间无操作也要进入掉电模式，防止家里没人耗尽电池
	    ttPowerDownBeeping++;
    }

    // 一定时间无操作进入掉电模式
    if (ttPowerDownSetting >= T_POWER_DOWN_SETTING || ttPowerDownBeeping >= T_POWER_DOWN_BEEPING)
    {
    	// 状态设置为掉电模式
    	status = STATUS_POWER_DOWN;

    	// 掉电之前关闭蜂鸣器
    	isBeeping = 0;
    	beep = 1;

    	// 掉电之前关闭数码管输出
    	// 数字10作为约定好的特殊参数，所有数码管的段均关闭输出
		showDigit(1, 10, 1);
		showDigit(2, 10, 1);
		showDigit(3, 10, 1);
		showDigit(4, 10, 1);

		// 进入掉电模式
    	PCON = 0x02;

    	// 下面几个nop的作用是，掉电模式被外部中断唤醒后不会马上进入中断而是会从这里接着往下走几步
    	// 为了让中断程序最优先执行，这里加几个空语句跳过去
    	_nop_();
    	_nop_();
    	_nop_();
    	_nop_();
    }
}

// 外部中断INT0,用于掉电唤醒
void exint0() interrupt 0
{
	// 如果是从掉电状态唤醒
	if (status == STATUS_POWER_DOWN)
	{
		// 掉电唤醒重新计时
		ttPowerDownSetting = 0;
		ttPowerDownBeeping = 0;

		// 回到设置模式
		// 由于唤醒中断程序和一般程序共用了同一个按钮，而且当被唤醒时本中断优先被执行
		// 执行完以后才会去执行按钮子程序，而在按钮子程序中会判断当前STATUS，如果是设置模式就切换到工作模式
		// 如果是工作模式就切换到设置模式
		// 而我最终的目的是唤醒以后进入设置模式，所以这里先设置为工作模式（会马上被按钮程序切换成设置模式的）
		status = STATUS_WORKING;
	}
}

/*----------------------------
ADC 中断服务程序 当AD转换完成时会产生中断
----------------------------*/
uchar adcSteps = 0;
void adc_isr() interrupt 5 using 1
{
    ADC_CONTR &= !ADC_FLAG;         //Clear ADC interrupt flag

    nowAdcAvrCnt++;
    if (nowAdcAvrCnt < ADC_AVR_CNT)
    {
    	// 每次取AD转换的高8位并计算平均值直到采样次数达到取平均值的次数
    	adcDataAvr = (adcDataAvr + ADC_DATA) / 2;
    	// 没有达到取平均值指定的次数时继续下一次采样
    	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
    } else {
    	// 采样次数清零
    	nowAdcAvrCnt = 0;
    	
    	// 将取到的平均值换算成时间用于显示
    	adcSteps = adcDataAvr * TIME_PART / 0xff;
    	// 先判断是否超过了第一阶段
    	if ( adcSteps <= TIME_STEP1_STEPS)
    	{
    		// 处于第1阶段
    		adcDataDisp = (adcSteps * TIME_STEP1_UNIT) / 60 *100 + (adcSteps * TIME_STEP1_UNIT) % 60 ;
    	}
    	else if ( adcSteps <= TIME_STEP2_STEPS)
    	{
    		// 处于第2阶段
    		adcDataDisp = (TIME_STEP1_LIMIT + ((adcSteps - TIME_STEP1_STEPS) * TIME_STEP2_UNIT)) / 60 *100 
    						+ (TIME_STEP1_LIMIT + ((adcSteps - TIME_STEP1_STEPS) * TIME_STEP2_UNIT)) % 60;
    	}
    	else
    	{
    		// 处于last阶段
    		adcDataDisp = (TIME_STEP2_LIMIT + ((adcSteps - TIME_STEP2_STEPS) * TIME_STEPLAST_UNIT)) / 60 *100 
    						+ (TIME_STEP2_LIMIT + ((adcSteps - TIME_STEP2_STEPS) * TIME_STEPLAST_UNIT)) % 60;
    	}

    	// 一次平均值全部取得后，判断当前如果处于设置状态则继续下一组采样，否则停止采样（倒计时阶段没有必要进行采样，省电）
    	if (status == STATUS_SETTING)
	    {
	    	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
	    }
    }

    //SendData(ch);                   //Show Channel NO.
    //SendData(0x22);
    //SendData(ADC_DATA);             //Get ADC high 8-bit result and Send to UART

//if you want show 10-bit result, uncomment next line
//    SendData(ADC_LOW2);             //Show ADC low 2-bit result
    
    //if (++ch > 7) ch = 0;           //switch to next channel
}

/*----------------------------
Initial ADC sfr
----------------------------*/
void InitADC()
{
	//Set P1.0 as Open-Drain mode
    P1 = 0x01;
    P1M0 = 0x01;
    P1M1 = 0x01;     

    ADC_DATA = 0;                   //Clear previous result
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
    Delay(2);                       //ADC power-on delay and Start A/D conversion
}

/*----------------------------
Initial UART
----------------------------*/
void InitUart()
{
    // SCON = 0x5a;                    //8 bit data ,no parity bit
    // TMOD = 0x20;                    //T1 as 8-bit auto reload
    // TH1 = TL1 = -(FOSC/12/32/BAUD); //Set Uart baudrate
    // TR1 = 1;                        //T1 start running
}

/*----------------------------
Send one byte data to PC
Input: dat (UART data)
Output:-
----------------------------*/
void SendData(BYTE dat)
{
    // while (!TI);                    //Wait for the previous data is sent
    // TI = 0;                         //Clear TI flag
    // SBUF = dat;                     //Send current data
}

/*----------------------------
Software delay function
----------------------------*/
void Delay(WORD n)
{
    WORD x;

    while (n--)
    {
        x = 5000;
        while (x--);
    }
}

