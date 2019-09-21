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

// 刷新显示用的内容，需要在main循环中调用
void DISPLAY_updateDisplay();

// 刷新显示，需要在main循环中调用
void DISPLAY_refreshDisplay();

void DISPLAY_ShowHHMM();
void DISPLAY_ShowMMDD();
void DISPLAY_ShowYYYY();
void DISPLAY_ShowMMSS();

// 显示一小会MMDD，然后切换回原来的显示状态
void DISPLAY_ShowMMDD_forAWhile(int interval);
// 显示一小会YYYY，然后切换回原来的显示状态
void DISPLAY_ShowYYYY_forAWhile(int interval);
// 显示一小会MMSS，然后切换回原来的显示状态
void DISPLAY_ShowMMSS_forAWhile(int interval);

// 显示一段向左滚动的文字
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
