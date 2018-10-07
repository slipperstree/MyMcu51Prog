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

BYTE ch = 0;                        //ADC channel NO. 0:ʹ��P1.0��ΪAD����

uint i;
uint tt;
uint now;
uint isBtnPushing;

// ����С��λ����������ʱ���ٷ�
// ����Խ��ÿ����/����һ��ʱ�䵥λʱ��ťת������Ҫ�ĽǶ�ԽС������Բ��ȶ������ڵĿ̶�Խ��Խ����������ȥ��
// ����ԽСÿ����/����һ��ʱ�䵥λʱ��ťת������Ҫ�ĽǶ�Խ��ҲԽ�ȶ�
#define TIME_PART 40

// ����ʱ��С��λ���룩
// Ŀǰ�ּ����׶�
// 5�������ڣ�30��Ϊ��λ
#define TIME_STEP1_LIMIT (5 * 60)
#define TIME_STEP1_UNIT 30
#define TIME_STEP1_STEPS (TIME_STEP1_LIMIT/TIME_STEP1_UNIT)
// 5��������20�����ڣ�1����Ϊ��λ
#define TIME_STEP2_LIMIT (20 * 60)
#define TIME_STEP2_UNIT (1 * 60)
#define TIME_STEP2_STEPS (TIME_STEP1_STEPS+(TIME_STEP2_LIMIT-TIME_STEP1_LIMIT)/TIME_STEP2_UNIT)
// 20�������ϣ�5����Ϊ��λ
#define TIME_STEPLAST_UNIT (5 * 60)

#define TIME_UNIT 30 // ����ʱ��С��λ���룩

uint alermSec;		// �����õ���ʱʣ������
uint alermDisp;		// ����ʱ������ʾ��(�ɵ���ʱʣ����������ó�)

// ��ǰ״̬
uchar status;
#define STATUS_SETTING 1 // ����ģʽ����ģʽ�£��������˸��ADת����Ч��
#define STATUS_WORKING 2 // ����ģʽ����ģʽ�£�����ܲ���˸��ADת����Ч�����е���ʱ��
#define STATUS_POWER_DOWN 3 // ����ģʽ����ģʽ�£�ֻ���ⲿ�жϼ����������ڻ���ϵͳ����������Ч����ʡ���ģʽ��

uint ttPowerDownSetting = 0;		// ����ģʽʱ���޲����������ģʽ��ʱ��
uint ttPowerDownBeeping = 0;		// ����ʱ���޲����������ģʽ��ʱ�䣬�����õ��޲���ʱ��ֿ�����Ϊ���������ʱ����΢��һЩ
#define T_POWER_DOWN_SETTING ((T_BLANK_INTERVAL_ON + T_BLANK_INTERVAL_OFF) * 15)  // ����״̬�������˸15�ε�ʱ��
#define T_POWER_DOWN_BEEPING ((T_BLANK_INTERVAL_ON + T_BLANK_INTERVAL_OFF) * 30)  // ����״̬�������˸40�ε�ʱ��

uint ttBlank;		// ����״̬ʱ��˸��ʱ��
#define T_BLANK_INTERVAL_ON 300 	// ��˸ʱ����(�����뵥λ���������ٶ��й���Ҫ����)
#define T_BLANK_INTERVAL_OFF 100 	// ��˸ʱ����(�����뵥λ���������ٶ��й���Ҫ����)
uchar stsBlanking;	// ��ǰ����˸״̬ 1����ʾ�� 0���ر���
#define STS_BLANK_ON	1
#define STS_BLANK_OFF	0

// ����ģʽʱ��Ϊ��ʡ�磬��ʾ����ʱ�������Ժ�͹رմ󲿷ֵ������
// ֻ��ʾָ�������ݣ�����һ����˸�ĵ��ʾ���ڼ�ʱ��
uchar isShowDummy = 0;
uint showDummySec = 0;
#define T_SHOW_DUMMY 3 // ����ʱ��ʾn���Զ��ر���Ļʡ��(��һֱ�ܿ���ʱ��Ļ�������һ���ܴ��ֵ����)
uchar stsDummy=1;
uchar stsDummyLast=4;

uchar isBeeping;	// �������Ƿ�Ӧ�����죨����ʱʱ�䵽��  1������  0��������
uint ttBeep;
#define T_BEEP_INTERVAL_ON 40 	// �������ٵ�ʱ����(�����뵥λ���������ٶ��й���Ҫ����)
#define T_BEEP_INTERVAL_OFF 250 // ������������֮���ʱ����(�����뵥λ���������ٶ��й���Ҫ����)
uchar stsBeep;		// ��ǰ��BEEP����״̬

uint adcDataDisp;		// ������ʾ��������ϵĲ������
#define ADC_AVR_CNT 10 	// ��β�������
uchar nowAdcAvrCnt = 0;	// ��ǰ��������
uint adcDataAvr;		// ����ƽ��ֵ

void init(void);
void setBitData(uchar);
void showDigit(uchar, uchar, uchar);
void btnPressed();
void btnPressing();
void btnReleased();

sbit LED_DIO = P1^5;	// ������������
sbit LED_RCLK = P1^6;	// �����źš�����������Ч
sbit LED_SCLK = P1^7;	// ��������ʱ���źš���������������Ч

sbit btn = P1^4;	// ��ť���½ӵ�GND���ͣ�ͬʱ��ť���ӵ�INT0���ϣ���Ϊ�ⲿ�жϣ�

sbit beep = P1^3;	// ������ �͵�ƽ��

void setBitData(uchar bitData){
	LED_DIO =  bitData;
	LED_SCLK = 0;
	LED_SCLK = 1;
}

// ָ��no(1-4)���������ʾ����num(0-9)����������������ʾ����ʾС���㣨1/0��
// num����Ϊ10��ʾ�ر���һλ�����
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
    // ��STC12C2052AD���ڲ�RC�̶�Ƶ��5.126Mhz�������
    // �˳�ֵΪÿ���ж�Ϊ1ms
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x16;			//���ö�ʱ��ֵ
	TH0 = 0xEC;			//���ö�ʱ��ֵ
	TF0 = 0;			//���TF0��־
    
    IT0=1;		// �ⲿ�ж�0����Ϊ�½��ش���(0Ϊ�͵�ƽ����)
    EX0=1;		// �����ⲿ�ж�0

    EA=1;		// �����ж��ܿ��ش�
    ET0=1;		// ��ʱ��0��Ч
    TR0=1;		// ��ʱ��0��ʼ����

    LED_RCLK = 0;
	LED_SCLK = 0;
    
    status = STATUS_SETTING; // �ϵ��ʼ״̬Ϊ������(��˸)
    now = 0;
    isBtnPushing = 0;
    beep = 1;		// �ϵ��ʼ״̬Ϊ�����У��͵�ƽʱ���У�
    ttBlank = 0;
    stsBlanking = STS_BLANK_ON;
    alermDisp = 0;
    adcDataDisp = 0;
    isBeeping = 0;	// �ϵ��ʼ״̬Ϊ�����У��͵�ƽʱ���У�
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
		btn = 1; // ȡ״̬֮ǰ��Ҫ���øߵ�ƽ
		if (isBtnPushing == 0 && btn == 0)
		{
			// ����������ʱһС��
			for (i = 0; i < 1000; ++i){;}
				if (isBtnPushing == 0 && btn == 0) {
					isBtnPushing = 1; // ��ťΪ���µ�״̬++++
					btnPressed();
				}
		}

		if (isBtnPushing == 1 && btn == 0){
			btnPressing();
		}

		if (isBtnPushing == 1 && btn == 1){
			// �ж���ť���ſ�
			isBtnPushing = 0;
			btnReleased();
		}

		// ���������״̬ �����������ʾ���������� ����˸���
		if (status == STATUS_SETTING)
		{
			if (stsBlanking == STS_BLANK_OFF)
			{
				// ����10��ΪԼ���õ������������������ܵĶξ��ر����
				showDigit(1, 10, 1);
				showDigit(2, 10, 1);
				showDigit(3, 10, 1);
				showDigit(4, 10, 1);
			} else {
				// ���ù�������ʾ�Ĳ��ǵ���ʱ�����ݶ���adc������������
				showDigit(1, adcDataDisp / 1000, 1);
				showDigit(2, adcDataDisp % 1000 / 100, 0);
				showDigit(3, adcDataDisp % 100 / 10, 1);
				showDigit(4, adcDataDisp % 10, 1);
			}
		}
		// �����������״̬
		else {
			// ���ʡ��ģʽ����ʾ������Ϣ��ʾ�������м��ɣ�
			if (isShowDummy == 1 && stsDummy != stsDummyLast)
			{
				if (stsDummy == 1)
				{
					showDigit(1, 10, 0); // ����һ����
					showDigit(2, 10, 1);
					showDigit(3, 10, 1);
					showDigit(4, 10, 1);
				} 
				else if (stsDummy == 2)
				{
					showDigit(1, 10, 1); // ����һ����
					showDigit(2, 10, 0);
					showDigit(3, 10, 1);
					showDigit(4, 10, 1);
				}
				else if (stsDummy == 3)
				{
					showDigit(1, 10, 1); // ����һ����
					showDigit(2, 10, 1);
					showDigit(3, 10, 0);
					showDigit(4, 10, 1);
				}
				else if (stsDummy == 4)
				{
					showDigit(1, 10, 1); // ����һ����
					showDigit(2, 10, 1);
					showDigit(3, 10, 1);
					showDigit(4, 10, 0);
				}
			}
			else
			{
				// ������ʾ����ʱ����
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
		// ������״̬�л�������״̬������ʱ״̬��
		// ȡ�õ�ǰ�趨��ʱ�䣬ͬʱΪ�˼��㷽�㣬�������ʱ�任�������
		alermDisp = adcDataDisp;
		alermSec = alermDisp / 100 * 60 + alermDisp % 100;
		status = STATUS_WORKING;
		isShowDummy = 0;

		// ͬʱ�������������λ(����ģʽ�²��������ģʽ)
		ttPowerDownSetting = 0;
		ttPowerDownBeeping = 0;
	} else {
		if (status == STATUS_WORKING && isBeeping)
		{
			isBeeping = 0;
			beep = 1;

			// ������״̬�ص�����״̬
			status = STATUS_SETTING;
			// ��ʼADC����
			ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
		}
		else if (status == STATUS_WORKING && isShowDummy == 1)
		{
			// �����ǰ����ʡ��ģʽ�£����°�ť��ʾ�뿴һ�µ���ʱ��ʣ��ʱ�䣬��ʱ���л�Ϊ����״̬
			// ֻ�Ǵ�ʡ��ģʽ�ص�����ģʽ����Ȼ���ģʽ����ָ��ʱ����ֻ��Զ��ص�ʡ��ģʽ��
			isShowDummy = 0;
			showDummySec = 0;
		}
		else
		{
			// �ӹ���״̬�ص�����״̬�������˴ӵ���ģʽ�����Ժ�ġ����ԡ�����״̬��
			status = STATUS_SETTING;
			// ��ʼADC����
			ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
		}

		// ��֤���κ�ʱ���°�ť������ֹ����
		isBeeping = 0;
		beep = 1;
	}
}

void btnPressing()
{
	// ��ť������
	//beep = 0;
}

void btnReleased()
{
	//beep = 1;
}

void timer0() interrupt 1
{
	// ����״̬ ���� ������û������ʱ �Ž��е���ʱ
	if (status == STATUS_WORKING && isBeeping == 0)
	{
		tt++;
		// �ֶ�������620���жϴ�ԼΪ1s
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
		    		// �л���ʡ��ģʽ
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

    // ����״̬�£�����LED��˸
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

	    // ����״̬�£�һ��ʱ���޲�����������ģʽ
	    ttPowerDownSetting++;
    }

    // ����������ʱ�������������ٵ�Ч��
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

    	// ����״̬�£�һ��ʱ���޲���ҲҪ�������ģʽ����ֹ����û�˺ľ����
	    ttPowerDownBeeping++;
    }

    // һ��ʱ���޲����������ģʽ
    if (ttPowerDownSetting >= T_POWER_DOWN_SETTING || ttPowerDownBeeping >= T_POWER_DOWN_BEEPING)
    {
    	// ״̬����Ϊ����ģʽ
    	status = STATUS_POWER_DOWN;

    	// ����֮ǰ�رշ�����
    	isBeeping = 0;
    	beep = 1;

    	// ����֮ǰ�ر���������
    	// ����10��ΪԼ���õ������������������ܵĶξ��ر����
		showDigit(1, 10, 1);
		showDigit(2, 10, 1);
		showDigit(3, 10, 1);
		showDigit(4, 10, 1);

		// �������ģʽ
    	PCON = 0x02;

    	// ���漸��nop�������ǣ�����ģʽ���ⲿ�жϻ��Ѻ󲻻����Ͻ����ж϶��ǻ��������������߼���
    	// Ϊ�����жϳ���������ִ�У�����Ӽ������������ȥ
    	_nop_();
    	_nop_();
    	_nop_();
    	_nop_();
    }
}

// �ⲿ�ж�INT0,���ڵ��绽��
void exint0() interrupt 0
{
	// ����Ǵӵ���״̬����
	if (status == STATUS_POWER_DOWN)
	{
		// ���绽�����¼�ʱ
		ttPowerDownSetting = 0;
		ttPowerDownBeeping = 0;

		// �ص�����ģʽ
		// ���ڻ����жϳ����һ���������ͬһ����ť�����ҵ�������ʱ���ж����ȱ�ִ��
		// ִ�����Ժ�Ż�ȥִ�а�ť�ӳ��򣬶��ڰ�ť�ӳ����л��жϵ�ǰSTATUS�����������ģʽ���л�������ģʽ
		// ����ǹ���ģʽ���л�������ģʽ
		// �������յ�Ŀ���ǻ����Ժ��������ģʽ����������������Ϊ����ģʽ�������ϱ���ť�����л�������ģʽ�ģ�
		status = STATUS_WORKING;
	}
}

/*----------------------------
ADC �жϷ������ ��ADת�����ʱ������ж�
----------------------------*/
uchar adcSteps = 0;
void adc_isr() interrupt 5 using 1
{
    ADC_CONTR &= !ADC_FLAG;         //Clear ADC interrupt flag

    nowAdcAvrCnt++;
    if (nowAdcAvrCnt < ADC_AVR_CNT)
    {
    	// ÿ��ȡADת���ĸ�8λ������ƽ��ֱֵ�����������ﵽȡƽ��ֵ�Ĵ���
    	adcDataAvr = (adcDataAvr + ADC_DATA) / 2;
    	// û�дﵽȡƽ��ֵָ���Ĵ���ʱ������һ�β���
    	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
    } else {
    	// ������������
    	nowAdcAvrCnt = 0;
    	
    	// ��ȡ����ƽ��ֵ�����ʱ��������ʾ
    	adcSteps = adcDataAvr * TIME_PART / 0xff;
    	// ���ж��Ƿ񳬹��˵�һ�׶�
    	if ( adcSteps <= TIME_STEP1_STEPS)
    	{
    		// ���ڵ�1�׶�
    		adcDataDisp = (adcSteps * TIME_STEP1_UNIT) / 60 *100 + (adcSteps * TIME_STEP1_UNIT) % 60 ;
    	}
    	else if ( adcSteps <= TIME_STEP2_STEPS)
    	{
    		// ���ڵ�2�׶�
    		adcDataDisp = (TIME_STEP1_LIMIT + ((adcSteps - TIME_STEP1_STEPS) * TIME_STEP2_UNIT)) / 60 *100 
    						+ (TIME_STEP1_LIMIT + ((adcSteps - TIME_STEP1_STEPS) * TIME_STEP2_UNIT)) % 60;
    	}
    	else
    	{
    		// ����last�׶�
    		adcDataDisp = (TIME_STEP2_LIMIT + ((adcSteps - TIME_STEP2_STEPS) * TIME_STEPLAST_UNIT)) / 60 *100 
    						+ (TIME_STEP2_LIMIT + ((adcSteps - TIME_STEP2_STEPS) * TIME_STEPLAST_UNIT)) % 60;
    	}

    	// һ��ƽ��ֵȫ��ȡ�ú��жϵ�ǰ�����������״̬�������һ�����������ֹͣ����������ʱ�׶�û�б�Ҫ���в�����ʡ�磩
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

