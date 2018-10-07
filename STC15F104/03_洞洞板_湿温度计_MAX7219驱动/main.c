/***************************************************/
/* ������_ʪ�¶ȼ�_MAX7219����  */
/*  */
/***************************************************/     
//#include<STC15104E.h>
#include<STC15F104W.h>
//#include <reg51.h>
#include <absacc.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>

#include <readDHT11.h>

#define uchar unsigned char
#define uint unsigned int

#define TRUE 1
#define FALSE 0

uchar i;
int n;
int a, b, c;
int myWendu, myShidu;
int numForShow;
int intensity; //����0-15

void init();
void display(int);

void setBitData(uchar);
void setByte(uchar);

void setDecodeMode(uchar);
void setIntensity(uchar);
void setScanLimit(uchar);
void setShutdownMode(uchar);
void setDisplayTestMode(uchar);
void showDigit(uchar, uchar, uchar);

void delay(int);
void load();

void flashMax7219(int);
void checkBtn();
int getNumForShow();

sbit DIO = P3^5;	// ������������
sbit LOAD = P3^4;	// �����źš�����������Ч
sbit SCLK = P3^3;	// ��������ʱ���źš���������������Ч

// �����ܹ��翪�أ������ܿ��Բ�ʹ�ã�io�����ռ��ɣ���Ҫ��������;��Ҫ�޸Ĵ��뽫��ش���ɾ����
// �����������ʱ�����ϸ���̫���޷����������ݣ���ÿ��ֻ�ڵ���ǰ��һ�����ݣ������Ժ������һ���������޷���ȷ�������ˡ�
// Ȼ��ʱͨ����io��ͨ��PNP�������ܶϿ�MAX7219�ĵ���ͨ�����临λ����λĬ�ϲ�������Ȼ��������һ�ε����ݣ����ѭ������
sbit SWITCH = P3^1;

// �����Ȱ�ť����ʹ���ⲿ�жϣ���Ϊ104Eϵ�в�֧���ⲿ�жϣ�104W��֧�֣���Ϊ��ͨ�ã�ʹ����ѯ��ʽ��
// ��io��2k���ϵ���������vcc��Ȼ�����ӿ��ص�GND�����ذ��µ͵�ƽ��
// �����ܿ��Բ�ʹ�ã�io���������������ߵ�ƽ����
sbit BTN = P3^0;
uchar isBtnPressing = 0;

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

// ֱ������ָ����ַ
void write_MAX7219(uchar address,uchar date)
{
        setByte(address);
        setByte(date);
        load();
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

// ָ��no(1-8)���������ʾ����num(0-9)
// ��������������ʾ����ʾС���㣨TRUE:��ʾ/FALSE:����ʾ��
void showDigit(uchar no, uchar num, uchar showDotPoint) {

	// ����ָ��Ĵ�����ַ��0xX1-0xX8
	// ��ʽ��D15-D12:���⣨������������0��
	//       D11-D8: 1-8��4λ�����Ʊ��룺����1��0,0,0,1��
	setByte(no);

	// ������ʾ����
	// ��ʽ��D7:��ʾС���㣨1������
	//       D6-D4:���⣨������������0��
	//       D3-D0:����0-9��4λ�����Ʊ��룺����2��0,0,1,0��
	if (showDotPoint == TRUE) {
		// �����ʾС��������Ҫ�����ֵ����λ��D7������Ϊ1(���λ��1���)
		setByte(num | 0x80);
	} else {
		setByte(num);
	}

	load();
}

void init()
{
	setDecodeMode(DECODE_MODE_ALL_USE);				// �����7��0ȫ����������ģʽ
	setIntensity(intensity);						// ����(0-15)
	setScanLimit(3);								// ɨ����ʾλ��(0-7)
	setShutdownMode(SHUTDOWN_MODE_NORMAL);			// ��������ģʽ
	setDisplayTestMode(DISPLAY_TEST_MODE_NORMAL);	// ��������ģʽ
}

main()
{
	int test = 0;
	n=0;
	intensity=0; // Ĭ������(0-15)

	// ������������������
	// PXM1��PXM0
	// 0     0  ׼˫��ڣ���ͳIO��
	// 0     1  ���������ǿ���� �������ɴ�20mA,�������ã�
	// 1     0  ��Ϊ���루���裩
	// 1     1  ��©���紫ͳ8051��P0��
	P3M0 = 0xFF;
	P3M1 = 0x00;

//test------------------------------
//init();
// display(5552);init();
// display(5889);
//display(1555);
//setShutdownMode(SHUTDOWN_MODE_NORMAL);			// ��������ģʽ
//delay(10);
//SWITCH = 1;
//test------------------------------

	numForShow = getNumForShow();
	while(1){
		flashMax7219(1000);
// init();
// display(test++);
// delay(100);
	}
}

void display(int show) {
	showDigit(4, show%10, FALSE);
	showDigit(3, show/10%10, FALSE);
	showDigit(2, show/100%10, TRUE);
	showDigit(1, show/1000%10, FALSE);
}

void delay(int tt){
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

void flashMax7219(int time)
{
	SWITCH = 0; // ON �����ܵ�ͨ ��ʼ��MAX7219����
	display(numForShow);  init();
	delay(time);

	numForShow = getNumForShow();

	SWITCH = 1; // ON �����ܽ�ֹ MAX7219�ϵ�
	display(numForShow);  init(); // ������MAX7219�������ʲô�����ĵ�ʣ��������ɣ�ʣ�������Դ������
	delay(1);// �����ʱ̫���п�������ܻ�û����ȫ�رգ����º���д����ʱ��ƽ����ʧ�ܣ�̫����������Ե���˸��ͣ�٣�����ܹر�ʱ��̫����
}

int getNumForShow(){
	uchar ret = 0;

	ret = readDHT11();
	switch (ret) {
		case READ_DHT11_RET_SUCCESS:
			myWendu = getWendu();
			myShidu = getShidu();
			numForShow = myWendu*100 + myShidu;
			break;
		case READ_DHT11_RET_TIMEOUT:
			numForShow = 9999;
			break;
		case READ_DHT11_RET_CHECKESUM_WRONG:
			numForShow = 5555;
			break;
		default:
			numForShow = ret;
			break;
	}

	return numForShow;
}

// ������а�ť����ѯ
void checkBtn(){
	int tmp = 0;
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
}