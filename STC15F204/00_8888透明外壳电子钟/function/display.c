#include <string.h>

#include "../header/display.h"
#include "../header/ds1302.h"
#include "../header/common.h"

enum EnumDispMode dispMode = DISP_MODE_HHMM;
enum EnumDispMode dispModeBefore = DISP_MODE_HHMM;

// ������������� λ
sbit DIG1 = P3^2;
sbit DIG2 = P3^3;
sbit DIG3 = P3^4;
sbit DIG4 = P3^5;

// ������������� ��
sbit segA = P2^0;
sbit segB = P2^1;
sbit segC = P2^2;
sbit segD = P2^3;
sbit segE = P2^4;
sbit segF = P2^5;
sbit segG = P2^6;
sbit segDP = P2^7;

// λ ����/�ر� ��ƽ����
#define DIG_ON   0
#define DIG_OFF  1

//����ܶ�̬ˢ�µ�ǰλ
static_idata_uchar nowPos = 1;

//����ܸ�λ�ϵ���ʾ����
static_idata_uchar dispDat[4];

//���ڹ�����ʾ�õ��ַ���������
static_idata_uchar dispString[32];

//��־λ
static uchar bdata flagByte;

static_idata_uchar ttflash = 0;
#define FLAG_IS_FLASH_ON   1
#define FLAG_IS_FLASH_OFF  0
sbit flagIsFlash = flagByte^0;				//��˸״̬��־
sbit flagIsShowingForAWhile = flagByte^1;	//��ʱ��ʾ״̬��־
static_idata_int showForAWhileInterval = 0;			//��ʱ��ʾʱ�����

// ������PWM������������ ----------------------------------------------------------------
// ԭ���ǣ�PWM����û�г��� pwmBright ����ֵʱ��Ŀ���������ͨ���Ҳ���ǵ�����
// ���������� pwmBright ʱ��ϵ�ر�Ŀ�������
// �������ﵽPWM�ܿ�� PWM_WIDTH_ALL ʱ����ǰ��Ŀ�������һ��PWM Cycle�������л�����һֻĿ������ܣ��ܶ���ʼ
// ����������������ڵ�λʱ����ͨ���ʱ����� pwmBright/PWM_WIDTH_ALL �� �Ӷ���������
// Ҳ���ǵ�pwmBrightΪ1ʱ�����ΪPWM_WIDTH_ALLʱ������

// PWM����ܳ������ֵԽ��PWM�ľ��Ⱦ�Խ�ߣ������õ�������С��λ��Խ��ϸ
// ��ֵԽ������ܵ�ˢ��Ƶ��ҲԽ�ͣ�̫�����ֵ�ᵼ���������ʾ��˸
#define PWM_WIDTH_ALL 15

// ����ģʽ
enum EnumBreathMode breathMode = DISPLAY_BREATH_MODE_OFF;
enum EnumSpeed breathSpeed = DISPLAY_SPEED_LV_6;
#define BRTH_BRIT_ARR_SIZE 32
static uchar code breathBrightArray[] = {
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1
};
static_idata_uchar nowBreathBright = 0;

// PWM�������ã������Գ��� PWM_WIDTH_ALL�������޸�
static_idata_uchar pwmBright = 1;

// PWM�����ã������޸�
static_idata_uchar ttPWM = 0;

// ������PWM������������ ----------------------------------------------------------------

char* aniScrollLeftString;
static char aniScrollLeftLen = 0;
static char aniScrollLeftNowPos = 0;
static char aniScrollLeftEndPos = 0;
enum EnumDispMode aniScrollLeftNextMode;
enum EnumSpeed aniScrollLeftSpeed;

//����ÿˢ��һ�μ�������һ�������ڶ�������˸�ȶ�̬Ч��
static_idata_uchar frameCounter = 0;

idata uchar setYear, setMonth, setDay, setHour, setMinute, setSecond;

//��1,2,4λ��������ʾcode
static uchar code digit124[]={
							~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x6f, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71,	// 11-16:A-F
							~0x57,	// 17:Y
							~0x31,	// 18:R
							~0x6d,	// 19:S
							~0x78,	// 19:T
							~0x38,	// 20:L
							~0x76,	// 21:H
							~0x1e,	// 22:J
							~0x06,	// 23:I
							~0x73,	// 24:P
							~0x3e,	// 25:U
							~0x37,	// 26:N
							~0x3f,	// 27:O
							~0x40,	// 28:-
							~0x00,	// 29:Space
							}; 
//��3λ�������Ӳ�����ǵ������ӵģ�������ʾcode��һ��
static uchar code digit3[]={
							~0x3f,~0x30,~0x5b,~0x79,~0x74,~0x6d,~0x6f,~0x38,~0x7f,~0x7d, // 0-9
							~0x77,~0x7c,~0x39,~0x5e,~0x79,~0x71, 	// 11-16:A-F
							~0x57,	// 17:Y
							~0x31,	// 18:R
							~0x6d,	// 19:S
							~0x78,	// 19:T
							~0x38,	// 20:L
							~0x76,	// 21:H
							~0x1e,	// 22:J
							~0x06,	// 23:I
							~0x73,	// 24:P
							~0x3e,	// 25:U
							~0x37,	// 26:N
							~0x3f,	// 27:O
							~0x40,	// 28:-
							~0x00,	// 29:Space
							};

#define CHAR_DAT_INDEX_Y 17
#define CHAR_DAT_INDEX_R 18
#define CHAR_DAT_INDEX_S 19
#define CHAR_DAT_INDEX_T 19
#define CHAR_DAT_INDEX_L 20
#define CHAR_DAT_INDEX_H 21
#define CHAR_DAT_INDEX_J 22
#define CHAR_DAT_INDEX_I 23
#define CHAR_DAT_INDEX_P 24
#define CHAR_DAT_INDEX_U 25
#define CHAR_DAT_INDEX_N 26
#define CHAR_DAT_INDEX_O 27
#define CHAR_DAT_INDEX_MINUS 28 //��-��
#define CHAR_DAT_INDEX_SP 29 	//�� ��

uchar getDispDatByChar(char ch, uchar isDigit3) {
	uchar ret = 0xff;
	switch (ch)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ret = isDigit3 ? digit3[ch-0x30] : digit124[ch-0x30];
			break;
		case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
			// A��ascii=65����ȥ54�õ�11�������Ǳ���������ֿ����A�������±�
			ret = isDigit3 ? digit3[ch-54] : digit124[ch-54];
			break;
		case 'Y': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_Y] : digit124[CHAR_DAT_INDEX_Y]; break;
		case 'R': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_R] : digit124[CHAR_DAT_INDEX_R]; break;
		case 'S': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_S] : digit124[CHAR_DAT_INDEX_S]; break;
		case 'T': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_T] : digit124[CHAR_DAT_INDEX_T]; break;
		case 'L': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_L] : digit124[CHAR_DAT_INDEX_L]; break;
		case 'H': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_H] : digit124[CHAR_DAT_INDEX_H]; break;
		case 'J': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_J] : digit124[CHAR_DAT_INDEX_J]; break;
		case 'I': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_I] : digit124[CHAR_DAT_INDEX_I]; break;
		case 'P': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_P] : digit124[CHAR_DAT_INDEX_P]; break;
		case 'U': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_U] : digit124[CHAR_DAT_INDEX_U]; break;
		case 'N': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_N] : digit124[CHAR_DAT_INDEX_N]; break;
		case 'O': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_O] : digit124[CHAR_DAT_INDEX_O]; break;
		case '-': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_MINUS] : digit124[CHAR_DAT_INDEX_MINUS]; break;
		case ' ': ret = isDigit3 ? digit3[CHAR_DAT_INDEX_SP] : digit124[CHAR_DAT_INDEX_SP]; break;
	default:
		break;
	}

	return ret;
}

uchar getDispDatByString(char* str, char pos, uchar isDigit3) {

	// Ĭ�Ϸ��ؿո�
	if (pos < 0 || pos >= strlen(str)) return 0xff;
	return getDispDatByChar(str[pos], isDigit3);
}

uchar getShiWei(uchar dat){
	return dat % 100 / 10;
}

uchar getGeWei(uchar dat){
	return dat % 10;
}

void showPosition(uchar pos, uchar dispDat, uchar pwmOnOff){

	// ������λѡ�ߵ�ƽ�������õ���������������IO���0��λѡ�ʸߵ�ƽ��
	// ����������͵�ƽ����

	// ��ֹ��Ӱ�����������ر�������ʾ
	DIG4 = DIG2 = DIG3 = DIG1 = 1;

	P2 = dispDat;

	switch (pos)
	{
		case 0:
			DIG1 = pwmOnOff;
			break;
		case 1:
			DIG2 = pwmOnOff;
			break;
		case 2:
			DIG3 = pwmOnOff;
			break;
		case 3:
			DIG4 = pwmOnOff;
			break;
		default:
			break;
	}
}

// ˢ����ʾ�õ����ݣ���Ҫ��mainѭ���е���
void DISPLAY_updateDisplay() {

	frameCounter++;
	ttflash++;

	// �л���˸״̬(��ʾ / ����ʾ)
	if (ttflash == DISPLAY_SPEED_LV_2)
	{
		ttflash = 0;
		flagIsFlash = ~flagIsFlash;
	}

	// ����������ֵ�ǰӦ����ʾ��λ��
	if (dispMode == DISP_MODE_ANI_SCROLL_LEFT &&
			frameCounter % aniScrollLeftSpeed == 0)
	{
		// ���ι���������ɺ󣬽�����һ��״̬
		if (aniScrollLeftNowPos == aniScrollLeftEndPos)
		{
			aniScrollLeftNowPos = 0;
			dispMode = aniScrollLeftNextMode;
		} else {
			aniScrollLeftNowPos++;
		}
	}

	// ����Ч��
	if (breathMode == DISPLAY_BREATH_MODE_ON &&
			frameCounter % breathSpeed == 0)
	{
		nowBreathBright++;
		if (nowBreathBright == BRTH_BRIT_ARR_SIZE) {
			nowBreathBright = 0;
		}
		pwmBright = breathBrightArray[nowBreathBright];
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
		// ����& 0x7f��Ϊ����ʾdp
		case DISP_MODE_HHMM:
			// ��ʾHH:MM
			dispDat[0] = digit124[getShiWei(DS1302_GetHour())];
			dispDat[1] = digit124[getGeWei(DS1302_GetHour())]		& (flagIsFlash ? 0x7f : 0xff);		
			dispDat[2] =   digit3[getShiWei(DS1302_GetMinute())]	& (flagIsFlash ? 0x7f : 0xff);
			dispDat[3] = digit124[getGeWei(DS1302_GetMinute())];
			break;

		case DISP_MODE_MMDD:
			// ��ʾMM.DD
			dispDat[0] = digit124[getShiWei(DS1302_GetMonth())];
			dispDat[1] = digit124[getGeWei(DS1302_GetMonth())] 		& 0x7f;
			dispDat[2] =   digit3[getShiWei(DS1302_GetDay())];
			dispDat[3] = digit124[getGeWei(DS1302_GetDay())];
			break;
		
		case DISP_MODE_YYYY:
			// ��ʾYYYY
			dispDat[0] = digit124[2];
			dispDat[1] = digit124[0];
			dispDat[2] =   digit3[getShiWei(DS1302_GetYear())];
			dispDat[3] = digit124[getGeWei(DS1302_GetYear())];
			break;

		case DISP_MODE_MMSS:
			// ��ʾMM:SS
			dispDat[0] = digit124[getShiWei(DS1302_GetMinute())];
			dispDat[1] = digit124[getGeWei(DS1302_GetMinute())] 	& 0x7f;
			dispDat[2] =   digit3[getShiWei(DS1302_GetSecond())] 	& 0x7f;
			dispDat[3] = digit124[getGeWei(DS1302_GetSecond())];
			break;
		
		case DISP_MODE_ANI_SCROLL_LEFT:
			// ��ʾһ���������������
			dispDat[0] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos  , 0);
			dispDat[1] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos+1, 0);
			dispDat[2] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos+2, 1);
			dispDat[3] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos+3, 0);
			break;
		
		case DISP_MODE_SET_YEAR:
			// ������[YYYY]
			dispDat[0] = digit124[2];
			dispDat[1] = digit124[0];
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setYear)] : 0xff;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setYear)] : 0xff;
			break;
		
		case DISP_MODE_SET_MONTH:
			// ������[Y-MM]
			dispDat[0] = digit124[getGeWei(setYear)];
			dispDat[1] = getDispDatByChar('-', 0);
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setMonth)] : 0xff;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setMonth)] : 0xff;
			break;
		
		case DISP_MODE_SET_DAY:
			// ������[M-DD]
			dispDat[0] = digit124[getGeWei(setMonth)];
			dispDat[1] = getDispDatByChar('-', 0);
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setDay)] : 0xff;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setDay)] : 0xff;
			break;
		
		case DISP_MODE_SET_HOUR:
			// ����Сʱ[HH:MM]
			dispDat[0] = flagIsFlash ? digit124[getShiWei(setHour)]        : 0xff;
			dispDat[1] = flagIsFlash ? digit124[getGeWei(setHour)]  & 0x7f : 0x7f;
			dispDat[2] = digit3[getShiWei(setMinute)] & 0x7f ;
			dispDat[3] = digit124[getGeWei(setMinute)];
			break;
		
		case DISP_MODE_SET_MINUTE:
			// ���÷���[HH:MM]
			dispDat[0] = digit124[getShiWei(setHour)];
			dispDat[1] = digit124[getGeWei(setHour)] & 0x7f ;
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setMinute)] & 0x7f : 0x7f;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setMinute)]       : 0xff;
			break;

		default:
			break;
	}
}

// ˢ����ʾ����Ҫ��mainѭ���е���
void DISPLAY_refreshDisplay() {
	
	// PWM���ȿ���
	ttPWM++;
	if (ttPWM <= pwmBright)
	{
		showPosition(nowPos, dispDat[nowPos], DIG_ON);
	} else {
		showPosition(nowPos, dispDat[nowPos], DIG_OFF);
	}
	
	// ÿ���һ��PWM��������Ⱦ�ˢ����ʾ��һλ�����
	if (ttPWM >= PWM_WIDTH_ALL){
		ttPWM = 0;
		nowPos++;
		if (nowPos>=4)
		{
			nowPos=0;
		}
	}
}

// ��������
void DISPLAY_setBrightness(uchar brightness){
	if (brightness <= PWM_WIDTH_ALL) {
		pwmBright = brightness;
	}
}

// �����Ƿ�򿪺���Ч��(���Ƚ���)��ע������򿪺���Ч��������ӹ��ߴ�����
void DISPLAY_SetBreathMode(enum EnumBreathMode mode, enum EnumSpeed speed){
	breathMode = mode;
	breathSpeed = speed;
}

// ��ʾһС��ָ��mode��Ȼ���л���ԭ������ʾ״̬
void showModeForAWhile(enum EnumDispMode mode, int interval) {

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

enum EnumDispMode DISPLAY_GetDispMode(){
	return dispMode;
}

// ��ʾһ���������������
void DISPLAY_ShowAniScrollLeft(
	char* str, 
	uchar startPos, uchar endPos, 
	enum EnumSpeed speed,
	enum EnumDispMode nextMode){

	if (dispMode != DISP_MODE_ANI_SCROLL_LEFT)
	{
		aniScrollLeftString = str;
		aniScrollLeftLen = strlen(str);
		aniScrollLeftSpeed = speed;
		aniScrollLeftNowPos = startPos-4;
		aniScrollLeftEndPos = endPos-4;
		aniScrollLeftNextMode = nextMode;

		dispMode = DISP_MODE_ANI_SCROLL_LEFT;
	}
}

void updateSetDateTimeDispBuff(){
	uchar i=0;
	// ��Ƭ��rom̫С��װ����stdio.h��������sprintf
	dispString[i++] = '2';
	dispString[i++] = '0';
	dispString[i++] = getShiWei(setYear) + 0x30;
	dispString[i++] = getGeWei(setYear) + 0x30;
	dispString[i++] = '-';
	dispString[i++] = getShiWei(setMonth) + 0x30;
	dispString[i++] = getGeWei(setMonth) + 0x30;
	dispString[i++] = '-';
	dispString[i++] = getShiWei(setDay) + 0x30;
	dispString[i++] = getGeWei(setDay) + 0x30;
	dispString[i++] = ' ';
	dispString[i++] = getShiWei(setHour) + 0x30;
	dispString[i++] = getGeWei(setHour) + 0x30;
	dispString[i++] = getShiWei(setMinute) + 0x30;
	dispString[i++] = getGeWei(setMinute) + 0x30;
	dispString[i++] = ' ';
	dispString[i++] = ' ';
	dispString[i++] = ' ';
	dispString[i++] = ' ';
	dispString[i++] = '2';
	dispString[i++] = '0';
	dispString[i++] = getShiWei(setYear) + 0x30;
	dispString[i++] = getGeWei(setYear) + 0x30;
	dispString[i++] = 0x00;
}

// ������
void DISPLAY_SetYearMode(){

	setYear = DS1302_GetYear();
	setMonth = DS1302_GetMonth();
	setDay = DS1302_GetDay();
	setHour = DS1302_GetHour();
	setMinute = DS1302_GetMinute();

	// ��������ʱ������ʾ��buff
	updateSetDateTimeDispBuff();

	// ��ʼ���Ŷ������������Ժ��������YYYY״̬
	DISPLAY_ShowAniScrollLeft(
		dispString, 0, strlen(dispString), 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_YEAR);
}

// ������
void DISPLAY_SetMonthMode(){

	// ��������ʱ������ʾ��buff
	updateSetDateTimeDispBuff();

	// �����������µ�λ�ã�Ȼ���������MM״̬
	DISPLAY_ShowAniScrollLeft(
		dispString, 4, 6, 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_MONTH);
}

// ������
void DISPLAY_SetDayMode(){

	// ��������ʱ������ʾ��buff
	updateSetDateTimeDispBuff();

	// �����������յ�λ�ã�Ȼ���������MM״̬
	DISPLAY_ShowAniScrollLeft(
		dispString, 7, 9, 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_DAY);
}

// ����Сʱ
void DISPLAY_SetHourMode(){

	// ��������ʱ������ʾ��buff
	updateSetDateTimeDispBuff();

	// ����������Сʱ�ͷ��ӵ�λ�ã�Ȼ���������HH״̬
	DISPLAY_ShowAniScrollLeft(
		dispString, 10, 14, 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_HOUR);
}

// ���÷���
void DISPLAY_SetMinuteMode(){

	// ��������ʱ������ʾ��buff
	updateSetDateTimeDispBuff();

	// ���ù���ֱ�ӽ�������MM״̬
	dispMode = DISP_MODE_SET_MINUTE;
}

// ���óɹ���д��1302���޸ĺ������ʱ�������ʾһ�κ󣬻ص�Ĭ����ʾ״̬HH:MM
void DISPLAY_SetComplite(){

	DS1302_WriteTime_Year(setYear);
	DS1302_WriteTime_Month(setMonth);
	DS1302_WriteTime_Day(setDay);
	DS1302_WriteTime_Hour(setHour);
	DS1302_WriteTime_Minute(setMinute);
	DS1302_WriteTime_Sec(0);

	// ��������ʱ������ʾ��buff
	updateSetDateTimeDispBuff();

	// ����������ʾһ�κ󣬻ص�Ĭ����ʾ״̬HH:MM�����ʱ������
	DISPLAY_ShowAniScrollLeft(
		dispString, 0, strlen(dispString)-8, 
		DISPLAY_SPEED_LV_3,
		DISP_MODE_HHMM);
}

void DISPLAY_SetYearAdd(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setYear++;}
void DISPLAY_SetYearMinus(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setYear--;}
void DISPLAY_SetMonthAdd(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setMonth++;}
void DISPLAY_SetMonthMinus(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setMonth--;}
void DISPLAY_SetDayAdd(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setDay++;}
void DISPLAY_SetDayMinus(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setDay--;}
void DISPLAY_SetHourAdd(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setHour++;}
void DISPLAY_SetHourMinus(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setHour--;}
void DISPLAY_SetMinuteAdd(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setMinute++;}
void DISPLAY_SetMinuteMinus(){ttflash = 0; flagIsFlash=FLAG_IS_FLASH_ON; setMinute--;}
