#include "../header/sensorAdc.h"
#include "../header/uart.h"
#include "../header/common.h"

#define ADC_POWER   0x80            //ADC��Դ����λ ON
#define ADC_FLAG    0x10            //ADC��ɱ�־
#define ADC_START   0x08            //ADC��ʼ����λ START
#define ADC_SPEEDLL 0x00            //540��ʱ��
#define ADC_SPEEDL  0x20            //360��ʱ��
#define ADC_SPEEDH  0x40            //180��ʱ��
#define ADC_SPEEDHH 0x60            //90��ʱ��

// �����¶ȴ�����
//#define USE_TEMP_ADC

#define ADC_CH_BRIGHT  6

#ifdef USE_TEMP_ADC
	#define ADC_CH_TEMP    7
#endif
static_idata_uchar ch = ADC_CH_BRIGHT;          //ADCͨ����

static_idata_uchar tempValue = 0;
static_idata_uchar brightValue = 0;
static_idata_uchar tempAdcValue = 0;
static_idata_uchar brightAdcValue = 0;
static_idata_uchar tempAdcValueAvg = 0;
static_idata_uchar brightAdcValueAvg = 0;

/*----------------------------
��ʼ��ADC (STC15F204EA)
----------------------------*/
void ADC_init()
{
	#ifdef USE_TEMP_ADC
    	P1ASF = 0xc0;                   //���� P1.6, P1.7 ΪAD�� [1100 0000]
	#else
		P1ASF = 0x40;                   //���� P1.6 ΪAD�� [0100 0000]
	#endif
    ADC_RES = 0;                    //�������Ĵ���
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
    delay_ms(2);                    //ADC�ϵ粢��ʱ
	EADC = 1;                       //ʹ��ADC�ж�
	EA =1;                          //ʹ�����ж�
}

/*----------------------------
ADC�жϷ������
----------------------------*/
uchar timesB=0;
uchar timesT=0;
void adc_isr() interrupt 5 using 1
{
	//EADC = 0;

    ADC_CONTR &= !ADC_FLAG;         //���ADC�жϱ�־

	if (ch == ADC_CH_BRIGHT)
	{
		//ADC_RES:ת�������8λ
		//ADC_RESL:ת�������2λ

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

			//�л�����һ��ͨ��
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
	
	// �жϺ������淢����Ϣ��������ʾ,����ʱ�ɴ�
	// delay_ms(1000);
    // //UART_SendByte(brightAdcValue);
    // UART_SendByte(0xff);
	// delay_ms(1000);

	// ������һ��ADC
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
