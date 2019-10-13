#ifndef __DISPLAY_H_
#define __DISPLAY_H_

enum EnumDispMode {
	DISP_MODE_HHMM,
	DISP_MODE_MMDD,
	DISP_MODE_YYYY,
	DISP_MODE_MMSS,
	DISP_MODE_ANI_SCROLL_LEFT,
	DISP_MODE_SET_YEAR,
	DISP_MODE_SET_MONTH,
	DISP_MODE_SET_DAY,
	DISP_MODE_SET_HOUR,
	DISP_MODE_SET_MINUTE,
	DISP_MODE_TEMPRETURE,
	DISP_MODE_SET_COUNTDOWN_MINUTE,
	DISP_MODE_COUNTDOWN,
    };

enum EnumSpeed {
	DISPLAY_SPEED_LV_1 = 64,
	DISPLAY_SPEED_LV_2 = 32,
	DISPLAY_SPEED_LV_3 = 16,
	DISPLAY_SPEED_LV_4 = 8,
	DISPLAY_SPEED_LV_5 = 4,
	DISPLAY_SPEED_LV_6 = 2,
	DISPLAY_SPEED_LV_7 = 1
	};

enum EnumBreathMode {
	DISPLAY_BREATH_MODE_ON,
	DISPLAY_BREATH_MODE_OFF
	};

// ˢ����ʾ�õ����ݣ���Ҫ��mainѭ���е���
void DISPLAY_updateDisplay();

// ˢ����ʾ����Ҫ��mainѭ���е���
void DISPLAY_refreshDisplay();

// ��������
void DISPLAY_setBrightness(unsigned char brightness);

// ���ú���Ч��
void DISPLAY_SetBreathMode(enum EnumBreathMode mode, enum EnumSpeed speed);

void DISPLAY_ShowHHMM();
void DISPLAY_ShowMMDD();
void DISPLAY_ShowYYYY();
void DISPLAY_ShowMMSS();

// ��ʾһС��MMDD��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowMMDD_forAWhile(int interval);
// ��ʾһС��YYYY��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowYYYY_forAWhile(int interval);
// ��ʾһС��MMSS��Ȼ���л���ԭ������ʾ״̬
void DISPLAY_ShowMMSS_forAWhile(int interval);
void DISPLAY_ShowTempreture_forAWhile(int interval);

// ��ʾһ���������������
void DISPLAY_ShowAniScrollLeft(
	char* str, 
	unsigned char startPos, unsigned char endPos, 
	enum EnumSpeed speed,
	enum EnumDispMode nextMode);

void DISPLAY_SetYearMode();
void DISPLAY_SetMonthMode();
void DISPLAY_SetDayMode();
void DISPLAY_SetHourMode();
void DISPLAY_SetMinuteMode();
void DISPLAY_SetComplite();

enum EnumDispMode DISPLAY_GetDispMode();

void DISPLAY_SetYearAdd();
void DISPLAY_SetYearMinus();
void DISPLAY_SetMonthAdd();
void DISPLAY_SetMonthMinus();
void DISPLAY_SetDayAdd();
void DISPLAY_SetDayMinus();
void DISPLAY_SetHourAdd();
void DISPLAY_SetHourMinus();
void DISPLAY_SetMinuteAdd();
void DISPLAY_SetMinuteMinus();

void DISPLAY_SetCountDownMode();
void DISPLAY_SetCountdownMinuteAdd();
void DISPLAY_SetCountdownMinuteMinus();
void DISPLAY_StartCountDown();
#endif
