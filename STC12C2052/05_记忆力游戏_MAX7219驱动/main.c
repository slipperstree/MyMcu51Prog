/***************************************************/
/* ̰ʳ��_MAX7219����  */
/*  */
/***************************************************/     
#include <STC12x20xx.h>
#include <memoryGame.h>

#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#define uchar unsigned char
#define uint unsigned int

#define TRUE 1
#define FALSE 0

uchar i;
uint n;
uint a, b, c;
uint intensity; //����0-15

uint tt;
uint waitTime;
#define TIME_STOP 0 // ����һ������ʱ�䣬���ظ�������0��ʾ����������ͣNEXT�����ĵ��ã��ȴ��û�����

void init();
void display();

void setBitData(uchar);
void setByte(uchar);

void setDecodeMode(uchar);
void setIntensity(uchar);
void setScanLimit(uchar);
void setShutdownMode(uchar);
void setDisplayTestMode(uchar);
void setRowData(uchar, uchar);

void delay(uint);
void load();

void checkBtn();

sbit DIO = P1^4;	// ������������
sbit LOAD = P1^3;	// �����źš�����������Ч
sbit SCLK = P3^4;	// ��������ʱ���źš���������������Ч

// �����Ȱ�ť����ʹ���ⲿ�жϣ���Ϊ104Eϵ�в�֧���ⲿ�жϣ�104W��֧�֣���Ϊ��ͨ�ã�ʹ����ѯ��ʽ��
// ��io��2k���ϵ���������vcc��Ȼ�����ӿ��ص�GND�����ذ��µ͵�ƽ��
// �����ܿ��Բ�ʹ�ã�io���������������ߵ�ƽ����
//sbit BTN = P1^3;
//uchar isBtnPressing = 0;

// ����ťIO�ڶ��壨����ʵ����Ҫ�����޸ģ�
sbit BTN_U = P3^7;
sbit BTN_D = P1^2;
sbit BTN_L = P1^1;
sbit BTN_R = P1^0;
uchar isBtnUPressing = 0;
uchar isBtnDPressing = 0;
uchar isBtnLPressing = 0;
uchar isBtnRPressing = 0;

// ��������ʱ����ʱ�������ÿ��ָ������������flg��
// ֪ͨ��������ô�����
#define IS_NOT_KEEP_PRESSING		0
#define IS_KEEP_PRESSING			1
#define TIME_KEEP_PRESSING_TRIGGER	1000
uint ttKeepPressingU;
uint ttKeepPressingD;
uint ttKeepPressingL;
uint ttKeepPressingR;

void setBitData(uchar bitData){
	SCLK = 0;
	DIO =  bitData;
	SCLK = 1;
}

// ����һ��8λ��
void setByte(uchar byteData){
	for (i = 0; i < 8; i++)
	{
		// ��������ִӸ�λ����λ�����ж��Ƿ�Ϊ1����Ϊ1�����øߵ�ƽ���������õ͵�ƽ
		// �жϵķ�������������λ����Ҫ�жϵ�λ�ƶ������λȻ���0x80��1000 0000�����룬
		// ��������Ȼ��0x80��1000 0000���ͱ�ʾ���λ��1���������λ����0
		if (((byteData<<i) & 0x80) == 0x80) {
			setBitData(1);
		} else {
			setBitData(0);
		}
	}
}

// ����ģʽ����
#define DECODE_MODE_ALL_NOT_USE		0x00	// ��������ܾ���ʹ�����빦��
#define DECODE_MODE_DIG0_ONLY		0x01 	// ֻ��DIG0������ܽ������룬��������ܲ�ʹ�����빦��
#define DECODE_MODE_DIG0123_ONLY 	0x0f 	// ��DIG0-3������ܽ������룬��������ܲ�ʹ�����빦��
#define DECODE_MODE_ALL_USE 		0xff	// �����7��0ȫ����������ģʽ
void setDecodeMode(uchar mode) {
	// ָ��Ĵ�����ַ���ã�0xX9
	// D15-D12:����
	// D11,10,9,8: 1,0,0,1
	setByte(0x09);
	setByte(mode);
	load();
}

// ��������
// mode = 8
void setIntensity(uchar mode) {
	// ָ��Ĵ�����ַ���ã�0xXA
	// D15-D12:����
	// D11,10,9,8: 1,0,1,0
	setByte(0x0A);
	
	// ���ȴ�0��15��16���ȼ���ָ���D3��D0��������0��15�Ķ����Ʊ���
	// D7-D4:����
	setByte(mode);

	load();
}
	
// ɨ����ʾλ������(0-7)
void setScanLimit(uchar mode) {
	// ָ��Ĵ�����ַ���ã�0xXB
	// D15-D12:����
	// D11,10,9,8: 1,0,1,1
	setByte(0x0B);
	
	// ɨ��λ��������0��7��8��ѡ��ָ���D2��D0��������0��7�Ķ����Ʊ���
	// D7-D3:����
	// D2-D0:0-7��3λ�����Ʊ���
	setByte(mode);

	load();
}

// �ض�ģʽ����
#define SHUTDOWN_MODE_SHUTDOWN 	0x00 //�ض�ģʽ
#define SHUTDOWN_MODE_NORMAL	0x01 //��������ģʽ
void setShutdownMode(uchar mode) {

	// ָ��Ĵ�����ַ���ã�0xXC
	// D15-D12:����
	// D11,10,9,8: 1,1,0,0
	setByte(0x0C);
	
	// �ض�ģʽ������0��1��2��ѡ������D0����
	// D7-D1:����
	// D0:1: ��������ģʽ 0: �ض�ģʽ
	setByte(mode);

	load();
}

// ����ģʽ����
#define DISPLAY_TEST_MODE_NORMAL 	0x00 //��������ģʽ
#define DISPLAY_TEST_MODE_TEST		0x01 //����ģʽ(ȫ��ģʽ)
void setDisplayTestMode(uchar mode) {

	// ָ��Ĵ�����ַ���ã�0xXF
	// D15-D12:����
	// D11,10,9,8: 1,1,1,1
	setByte(0x0f);
	
	// ����ģʽ������0��1��2��ѡ������D0����
	// D7-D1:����
	// D0:0: ��������ģʽ 1: ����ģʽ(ȫ��ģʽ)
	setByte(mode);

	load();
}

void Timer0Init(void)		//1����@5.5296MHz
{
	tt = 0;

	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0x66;		//���ö�ʱ��ֵ
	TH0 = 0xEA;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ

	EA=1;
    ET0=1;
}

void init()
{
	setDecodeMode(DECODE_MODE_ALL_NOT_USE);			// �����7��0ȫ������������ģʽ
	setIntensity(intensity);						// ����(0-15)
	setScanLimit(7);								// ɨ����ʾλ��(0-7)
	setShutdownMode(SHUTDOWN_MODE_NORMAL);			// ��������ģʽ
	setDisplayTestMode(DISPLAY_TEST_MODE_NORMAL);	// ��������ģʽ
}

main()
{
	intensity=0; // Ĭ������(0-15)

	waitTime=200;

	init();

	mg_restart();

	Timer0Init();

	while(1){
		checkBtn();
		display();
	}
}

void display() {
	uchar row;
	for (row= 0; row < 8; ++row)
	{
		setRowData(row+1, mg_getRowDataForShow(row));
	}
}

// ָ��no(1-8)����ʾָ������
void setRowData(uchar row, uchar rowData) {

	// ����ָ��Ĵ�����ַ��0xX1-0xX8
	// ��ʽ��D15-D12:���⣨������������0��
	//       D11-D8: 1-8��4λ�����Ʊ��룺����1��0,0,0,1��
	setByte(row);

	// ������ʾ����
	setByte(rowData);

	load();
}

void delay(uint tt){
	for (a = 0; a < tt; ++a)
	{
		for (b = 0; b < 100; ++b)
		{

			for (c = 0; c < 100; ++c)
			{
				// �������ȵ�����ť�Ĵ���
				checkBtn();
			}
		}
	}
}

void load(){
	//_nop_();_nop_();_nop_();_nop_();
	LOAD = 1;
	//_nop_();_nop_();_nop_();_nop_();
	LOAD = 0;
	//_nop_();_nop_();_nop_();_nop_();
}

// ������а�ť����ѯ
void checkBtn(){
	int tmp = 0;
	/*
	if (BTN == 0 && isBtnPressing == 0)
	{
		// ��������
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN == 0) {
			isBtnPressing = 1;
			// ȷ�ϰ�ť�����£��������ȼ�һ�������������15�ٰ�����ص��������0
			intensity+=1;
			if (intensity > 7)
			{
				intensity = 0;
			}
			// ��������
			setIntensity(intensity);
		}
	}

	if (BTN == 1 && isBtnPressing == 1)
	{
		isBtnPressing = 0;
	}
	*/

	// U
	if (BTN_U == 0 && isBtnUPressing == 0)
	{
		// ��������
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_U == 0) {
			isBtnUPressing = 1;
			// ȷ�ϰ�ť������
			waitTime = mg_moveUp();
		}
	}

	if (isBtnUPressing == 1 && BTN_U == 1)
	{
		// �ɿ���ť
		isBtnUPressing = 0;

		// ��հ�ס���ŵļ���
		ttKeepPressingU = 0;

		//mg_UpRelease();
	}

	// D
	if (BTN_D == 0 && isBtnDPressing == 0)
	{
		// ��������
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_D == 0) {
			isBtnDPressing = 1;
			// ȷ�ϰ�ť������
			waitTime = mg_moveDown();
		}
	}

	if (BTN_D == 1 && isBtnDPressing == 1)
	{
		isBtnDPressing = 0;

		// ��հ�ס���ŵļ���
		ttKeepPressingD = 0;
	}

	if (BTN_L == 0 && isBtnLPressing == 0)
	{
		// ��������
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_L == 0) {
			isBtnLPressing = 1;
			// ȷ�ϰ�ť������
			waitTime = mg_moveLeft();
		}
	}

	if (BTN_L == 1 && isBtnLPressing == 1)
	{
		isBtnLPressing = 0;

		// ��հ�ס���ŵļ���
		ttKeepPressingL = 0;
	}

	// R
	if (BTN_R == 0 && isBtnRPressing == 0)
	{
		// ��������
		for (tmp = 0; tmp < 10000; ++tmp){;}
		if (BTN_R == 0) {
			isBtnRPressing = 1;
			// ȷ�ϰ�ť������
			waitTime = mg_moveRight();
		}
	}

	if (BTN_R == 1 && isBtnRPressing == 1)
	{
		isBtnRPressing = 0;

		// ��հ�ס���ŵļ���
		ttKeepPressingR = 0;
	}
}

void timer0() interrupt 1
{
	TL0 = 0x66;		//���ö�ʱ��ֵ
	TH0 = 0xEA;		//���ö�ʱ��ֵ

	// ���ǰһ�ε���next�����ķ���ֵΪ0����ʾ�ӳ���Ҫ������ֹͣ��������Next����
	// ��ôtt�������Լӣ�Ҳ������ͣnext�Ĵ���
	// �ȴ��û�����, ֱ��ĳ���û�����ָ�waitTime����
	if (waitTime != TIME_STOP)
	{
		tt++;

		if (tt >= waitTime)
		{
			tt=0;
			waitTime = mg_moveNext();
		}
	} else {
		tt=0;
	}

	
	
	// ���Ӱ�ť�����¼�
	if (BTN_U == 0 && isBtnUPressing == 1)
	{
		ttKeepPressingU++;
		if (ttKeepPressingU >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingU = 0;
			waitTime = mg_KeepPressingU();
		}
	}

	if (BTN_D == 0 && isBtnDPressing == 1)
	{
		ttKeepPressingD++;
		if (ttKeepPressingD >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingD = 0;
			waitTime = mg_KeepPressingD();
		}
	}

	if (BTN_L == 0 && isBtnLPressing == 1)
	{
		ttKeepPressingL++;
		if (ttKeepPressingL >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingL = 0;
			waitTime = mg_KeepPressingL();
		}
	}

	if (BTN_R == 0 && isBtnRPressing == 1)
	{
		ttKeepPressingR++;
		if (ttKeepPressingR >= TIME_KEEP_PRESSING_TRIGGER)
		{
			ttKeepPressingR = 0;
			waitTime = mg_KeepPressingR();
		}
	}
}