#include "../header/sensorAdc.h"
#include "../header/uart.h"
#include "../header/common.h"

#define ADC_POWER   0x80            //ADC电源控制位 ON
#define ADC_FLAG    0x10            //ADC完成标志
#define ADC_START   0x08            //ADC起始控制位 START
#define ADC_SPEEDLL 0x00            //540个时钟
#define ADC_SPEEDL  0x20            //360个时钟
#define ADC_SPEEDH  0x40            //180个时钟
#define ADC_SPEEDHH 0x60            //90个时钟

// 启用温度传感器
//#define USE_TEMP_ADC

#define ADC_CH_BRIGHT  6

#ifdef USE_TEMP_ADC
	#define ADC_CH_TEMP    7
#endif
static_idata_uchar ch = ADC_CH_BRIGHT;          //ADC通道号

static_idata_uchar tempValue = 0;
static_idata_uchar brightValue = 0;
static_idata_uchar tempAdcValue = 0;
static_idata_uchar brightAdcValue = 0;
static_idata_uchar tempAdcValueAvg = 0;
static_idata_uchar brightAdcValueAvg = 0;

/*----------------------------
初始化ADC (STC15F204EA)
----------------------------*/
void ADC_init()
{
	#ifdef USE_TEMP_ADC
    	P1ASF = 0xc0;                   //设置 P1.6, P1.7 为AD口 [1100 0000]
	#else
		P1ASF = 0x40;                   //设置 P1.6 为AD口 [0100 0000]
	#endif
    ADC_RES = 0;                    //清除结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
    delay_ms(2);                    //ADC上电并延时
	EADC = 1;                       //使能ADC中断
	EA =1;                          //使能总中断
}

/*----------------------------
ADC中断服务程序
----------------------------*/
uchar timesB=0;
uchar timesT=0;
void adc_isr() interrupt 5 using 1
{
	//EADC = 0;

    ADC_CONTR &= !ADC_FLAG;         //清除ADC中断标志

	if (ch == ADC_CH_BRIGHT)
	{
		//ADC_RES:转换结果高8位
		//ADC_RESL:转换结果低2位

		if (timesB == 0)
		{
			//brightAdcValueAvg = ADC_RES*4+ADC_RESL;
			brightAdcValueAvg = ADC_RES;
		} else {
			brightAdcValueAvg = ADC_RES / 2 + ADC_RES / 2;
		}

		timesB++;
		if (timesB==10)
		{
			timesB=0;
			brightAdcValue = brightAdcValueAvg;

			//切换到另一个通道
			#ifdef USE_TEMP_ADC
				ch = ADC_CH_TEMP;
			#endif
		}
		
	} else {
		if (timesT == 0)
		{
			//tempAdcValueAvg = ADC_RES*4+ADC_RESL;
			tempAdcValueAvg = ADC_RES;
		} else {
			tempAdcValueAvg = ADC_RES;
			tempAdcValueAvg = tempAdcValueAvg / 2 + ADC_RES / 2;
		}

		timesT++;
		if(timesT == 100)
		{
			timesT=0;
			tempAdcValue = tempAdcValueAvg;

			ch = ADC_CH_BRIGHT;
		}
	}
	
	// 中断函数里面发送消息会拖慢显示,调试时可打开
	// delay_ms(1000);
    // //UART_SendByte(brightAdcValue);
    // UART_SendByte(0xff);
	// delay_ms(1000);

	// 继续下一次ADC
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;

	//EADC = 1;
}

uchar ADC_GetTempretureAdcValue(){
	return tempAdcValue;
}

uchar ADC_GetBrightAdcValue(){
	return brightAdcValue;
}

uchar ADC_GetTempreture(){
	//494-498   28.5
	//499-500   28.4
	//501-510   28.3
	//511-515   28.2
	//511-512   27.1
	//519-524   27.0-27.9
	//526-527   26.4
	//527-528   26.3
	//529-532   26.2
	//531-532   26.1
	//533	26.0
	//544   25.0
	// if (tempAdcValue < 439){
	// 	return 99;
	// } else if (tempAdcValue > 490 && tempAdcValue < 515){
	// 	return 28;
	// } else if (tempAdcValue > 516 && tempAdcValue < 524){
	// 	return 27;
	// } else if (tempAdcValue > 523 && tempAdcValue < 533){
	// 	return 26;
	// } else if (tempAdcValue > 534 && tempAdcValue < 544){
	// 	return 25;
	// } else if (tempAdcValue > 535 && tempAdcValue < 546){
	// 	return 24;
	// } else if (tempAdcValue > 547 && tempAdcValue < 558){
	// 	return 23;
	// } else {
	// 	return 1;
	// }

	return 10;
}

enum EnumADCBrightMode ADC_GetBright(){

	if (brightAdcValue > 240){
		return ADC_BRIGHT_MODE_NIGHT;

	} else if (brightAdcValue > 180){
		return ADC_BRIGHT_MODE_CLOUD;

	} else {
		return ADC_BRIGHT_MODE_DAY;
	}
}
