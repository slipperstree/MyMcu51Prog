#include "../header/display.h"
#include "../header/ds1302.h"
#include "../header/common.h"

#define DISP_MODE_HHMM		1
#define DISP_MODE_MMDD		2

uchar dispMode = DISP_MODE_HHMM;

// ������������� λ
sbit A1 = P3^2;
sbit A2 = P3^3;
sbit A3 = P3^4;
sbit A4 = P3^5;

// ������������� ��
sbit segA = P2^0;
sbit segB = P2^1;
sbit segC = P2^2;
sbit segD = P2^3;
sbit segE = P2^4;
sbit segF = P2^5;
sbit segG = P2^6;
sbit segDP = P2^7;

//��124λ��������ʾcode
uchar code digit124[17]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71}; // 0-F
//��3λ�������Ӳ�����ǵ������ӵģ�������ʾcode��һ����Ҫ��
uchar code digit3[17]={~0x3f,~0x30,~0x5b,~0x79,~0x74,~0x6d,~0x6f,~0x38,~0x7f,~0x7d, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71}; // 0-F(wrong)
//����ܶ�̬ˢ�µ�ǰλ
uchar nowPos = 1;

//����ܸ�λ�ϵ���ʾ����
uchar dispDat[4];
// uchar d1 = 0;
// uchar d2 = 0;
// uchar d3 = 0;
// uchar d4 = 0;

void showPosition(uchar pos, uchar dispDat){

	// ������λѡ�ߵ�ƽ�������õ���������������IO���0��λѡ�ʸߵ�ƽ��
	// ����������͵�ƽ����

	// ��ֹ��Ӱ�����������ر�������ʾ
	A4 = A2 = A3 = A1 = 1;

	P2 = dispDat;

	switch (pos)
	{
		case 0:
			A1 = 0;
			break;
		case 1:
			A2 = 0;
			break;
		case 2:
			A3 = 0;
			break;
		case 3:
			A4 = 0;
			break;
		default:
			break;
	}
}

// ˢ����ʾ�õ����ݣ���Ҫ��mainѭ���е���
void DISPLAY_updateDisplay() {

	switch (dispMode)
	{
		case DISP_MODE_HHMM:
			// ��ʾHH:MM
			dispDat[0] = digit124[DS1302_GetHour() % 100 / 10];
			dispDat[1] = digit124[DS1302_GetHour() % 10] 			& 0x7f;		// ����& 0x7f��Ϊ����ʾdp
			dispDat[2] = digit3[DS1302_GetMinute() % 100 / 10] 		& 0x7f;		// ����& 0x7f��Ϊ����ʾdp
			dispDat[3] = digit124[DS1302_GetMinute() % 10];
			break;

		case DISP_MODE_MMDD:
			// ��ʾMM.DD
			dispDat[0] = digit124[DS1302_GetMonth() % 100 / 10];
			dispDat[1] = digit124[DS1302_GetMonth() % 10] 			& 0x7f;		// ����& 0x7f��Ϊ����ʾdp
			dispDat[2] = digit3[DS1302_GetDay() % 100 / 10];
			dispDat[3] = digit124[DS1302_GetDay() % 10];
			break;
		
		default:
			break;
	}
}

// ˢ����ʾ����Ҫ��mainѭ���е���
void DISPLAY_refreshDisplay() {
	nowPos++;
	if (nowPos>=4)
	{
		nowPos=0;
	}
	showPosition(nowPos, dispDat[nowPos]);
}

void DISPLAY_ShowHHMM(){
	dispMode = DISP_MODE_HHMM;
}

void DISPLAY_ShowMMDD(){
	dispMode = DISP_MODE_MMDD;
}