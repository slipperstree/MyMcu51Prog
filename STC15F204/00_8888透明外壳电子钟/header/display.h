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

// 刷新显示用的内容，需要在main循环中调用
void DISPLAY_updateDisplay();

// 刷新显示，需要在main循环中调用
void DISPLAY_refreshDisplay();

// 设置亮度
void DISPLAY_setBrightness(unsigned char brightness);

// 设置呼吸效果
void DISPLAY_SetBreathMode(enum EnumBreathMode mode, enum EnumSpeed speed);

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
void DISPLAY_ShowTempreture_forAWhile(int interval);

// 显示一段向左滚动的文字
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
