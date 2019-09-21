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
	DISP_MODE_SET_MINUTE
    };

enum SPEED {
	DISPLAY_SPEED_LV_1 = 16,
	DISPLAY_SPEED_LV_2 = 8,
	DISPLAY_SPEED_LV_3 = 4,
	DISPLAY_SPEED_LV_4 = 2
	};

// ˢ����ʾ�õ����ݣ���Ҫ��mainѭ���е���
void DISPLAY_updateDisplay();

// ˢ����ʾ����Ҫ��mainѭ���е���
void DISPLAY_refreshDisplay();

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

// ��ʾһ���������������
void DISPLAY_ShowAniScrollLeft(
	char* str, 
	unsigned char startPos, unsigned char endPos, 
	enum SPEED speed,
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

#endif
