#include "../header/display.h"
#include "../header/ds1302.h"
#include "../header/common.h"

#define DISP_MODE_HHMM		1
#define DISP_MODE_MMDD		2
#define DISP_MODE_YYYY		3
#define DISP_MODE_MMSS		4

static uchar dispMode = DISP_MODE_HHMM;
static uchar dispModeBefore = DISP_MODE_HHMM;

// ������������� λ
sbit A1 = P0^0;
sbit A2 = P0^1;
sbit A3 = P0^2;
sbit A4 = P0^3;
sbit A5 = P0^4;
sbit A6 = P0^5;
sbit CSAD = P0^7;

// λѡ����
sbit WELA = P2^7;
// ��ѡ����
sbit DULA = P2^6;

//������ʾcode
static uchar code digit[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

//����ܶ�̬ˢ�µ�ǰλ
static uchar nowPos = 0;

//����ܸ�λ�ϵ���ʾ����
static uchar dispDat[6];

//��־λ
static uchar bdata flagByte;
sbit flagIsFlash = flagByte^0;				//��˸״̬��־
sbit flagIsShowingForAWhile = flagByte^1;	//��ʱ��ʾ״̬��־
static int showForAWhileInterval = 0;			//��ʱ��ʾʱ�����

//����ÿˢ��һ�μ�������һ�������ڶ�������˸�ȶ�̬Ч��
static uchar frameCounter = 0;

void showPosition(uchar pos, uchar dat){

	//��ָ��λ
	P0 = 0xff;
	WELA = 1;
	A1 = 1;
	A2 = 1;
	A3 = 1;
	A4 = 1;
	A5 = 1;
	A6 = 1;
	switch(pos)
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
		case 4:
			A5 = 0;
			break;
		case 5:
			A6 = 0;
			break;
		default:
			break;
	}
	WELA = 0;//����

	//������ܵĶ����������ߵ�ƽ��
	P0 = 0xff;
	DULA = 1;
	//��������ܵ�����
	P0 = dat;
	//�ر�����ܵĶ����������ߵ�ƽ��
	DULA = 0;
}

// ˢ����ʾ�õ����ݣ���Ҫ��mainѭ���е���
void DISPLAY_updateDisplay() {

	frameCounter++;

	// �л���˸״̬(��ʾ / ����ʾ)
	if (frameCounter % 8 == 0)
	{
		flagIsFlash = ~flagIsFlash;
	}

	// �����ǰ������ʱ��ʾ״̬�£����ж���û�е�ʱ�䣬�����ʱ���˾��л���ԭ������ʾģʽ
	if (flagIsShowingForAWhile)
	{
		showForAWhileInterval--;
		if (showForAWhileInterval <= 0)
		{
			// ʱ�䵽���ָ�֮ǰ��״̬
			flagIsShowingForAWhile = 0;
			dispMode = dispModeBefore;
		}
	}

	switch (dispMode)
	{
		case DISP_MODE_HHMM:
			// ��ʾHH:MM:SS
			dispDat[0] = digit[DS1302_GetHour() % 100 / 10];
			dispDat[1] = digit[DS1302_GetHour() % 10];		
			dispDat[2] = digit[DS1302_GetMinute() % 100 / 10];
			dispDat[3] = digit[DS1302_GetMinute() % 10] | (flagIsFlash ? 0x80 : 0x00);		// ��ʾdp;
			dispDat[4] = digit[DS1302_GetSecond() % 100 / 10];
			dispDat[5] = digit[DS1302_GetSecond() % 10];
			break;

		case DISP_MODE_MMDD:
			// ��ʾMM.DD
			dispDat[0] = digit[DS1302_GetMonth() % 100 / 10];
			dispDat[1] = digit[DS1302_GetMonth() % 10] | 0x80;		// ��ʾdp;;		
			dispDat[2] = digit[DS1302_GetDay() % 100 / 10];
			dispDat[3] = digit[DS1302_GetDay() % 10];
			dispDat[4] = digit[DS1302_GetHour() % 100 / 10];
			dispDat[5] = digit[DS1302_GetHour() % 10];
			break;
		
		case DISP_MODE_YYYY:
			// ��ʾYYYY
			dispDat[0] = digit[2];
			dispDat[1] = digit[0];
			dispDat[2] = digit[DS1302_GetYear() % 100 / 10];
			dispDat[3] = digit[DS1302_GetYear() % 10];
			dispDat[4] = digit[DS1302_GetMonth() % 100 / 10];
			dispDat[5] = digit[DS1302_GetMonth() % 10];
			break;

		case DISP_MODE_MMSS:
			// ��ʾMM:SS
			dispDat[0] = digit[DS1302_GetMinute() % 100 / 10];
			dispDat[1] = digit[DS1302_GetMinute() % 10];
			dispDat[2] = digit[DS1302_GetSecond() % 100 / 10];
			dispDat[3] = digit[DS1302_GetSecond() % 10];
			dispDat[4] = 0x00;
			dispDat[5] = 0xff;
			break;

		default:
			break;
	}
}

// ˢ����ʾ����Ҫ��mainѭ���е���
void DISPLAY_refreshDisplay() {
	nowPos++;
	if (nowPos>=6)
	{
		nowPos=0;
	}
	showPosition(nowPos, dispDat[nowPos]);
}

// ��ʾһС��ָ��mode��Ȼ���л���ԭ������ʾ״̬
void showModeForAWhile(uchar mode, int interval) {

	// ��ǰΪ����ʱ��ʾ״̬�������л�
	if (flagIsShowingForAWhile == 0)
	{
		//��ס֮ǰ��״̬�Ա㵽ʱ���ָ�
		dispModeBefore = dispMode;

		//�л�״̬
		dispMode = mode;

		//����ʱ������ʼ
		showForAWhileInterval = interval;
		flagIsShowingForAWhile = 1;
	}
}

void DISPLAY_ShowHHMM(){
	dispMode = DISP_MODE_HHMM;
}

void DISPLAY_ShowMMDD(){
	dispMode = DISP_MODE_MMDD;
}

// ��ʾһС��MMDD��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowMMDD_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_MMDD, interval);
}

void DISPLAY_ShowYYYY(){
	dispMode = DISP_MODE_YYYY;
}

// ��ʾһС��YYYY��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowYYYY_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_YYYY, interval);
}

void DISPLAY_ShowMMSS(){
	dispMode = DISP_MODE_MMSS;
}

// ��ʾһС��MMSS��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowMMSS_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_MMSS, interval);
}
