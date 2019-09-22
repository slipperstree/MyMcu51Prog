#include <string.h>

#include "../header/display.h"
#include "../header/ds1302.h"
#include "../header/common.h"

enum EnumDispMode dispMode = DISP_MODE_HHMM;
enum EnumDispMode dispModeBefore = DISP_MODE_HHMM;

// 数码管引脚连接 位
sbit DIG1 = P3^2;
sbit DIG2 = P3^3;
sbit DIG3 = P3^4;
sbit DIG4 = P3^5;

// 数码管引脚连接 段
sbit segA = P2^0;
sbit segB = P2^1;
sbit segC = P2^2;
sbit segD = P2^3;
sbit segE = P2^4;
sbit segF = P2^5;
sbit segG = P2^6;
sbit segDP = P2^7;

// 位 点亮/关闭 电平定义
#define DIG_ON   0
#define DIG_OFF  1

//数码管动态刷新当前位
static_idata_uchar nowPos = 1;

//数码管各位上的显示内容
static_idata_uchar dispDat[4];

//用于滚动显示用的字符串缓冲区
static_idata_uchar dispString[32];

//标志位
static uchar bdata flagByte;

static_idata_uchar ttflash = 0;
#define FLAG_IS_FLASH_ON   1
#define FLAG_IS_FLASH_OFF  0
sbit flagIsFlash = flagByte^0;				//闪烁状态标志
sbit flagIsShowingForAWhile = flagByte^1;	//暂时显示状态标志
static_idata_int showForAWhileInterval = 0;			//暂时显示时间计数

// 用于软PWM，控制亮度用 ----------------------------------------------------------------
// 原理是，PWM计数没有超过 pwmBright 的数值时，目标数码管是通电的也就是点亮的
// 当计数超过 pwmBright 时则断电关闭目标数码管
// 最后计数达到PWM总宽度 PWM_WIDTH_ALL 时，当前的目标数码管一个PWM Cycle结束，切换到下一只目标数码管，周而复始
// 这样，所有数码管在单位时间内通电的时间就是 pwmBright/PWM_WIDTH_ALL ， 从而控制亮度
// 也就是当pwmBright为1时，最暗，为PWM_WIDTH_ALL时，最亮

// PWM宽度总长，这个值越大，PWM的精度就越高，可设置的亮度最小单位就越精细
// 但值越大数码管的刷新频率也越低，太大的数值会导致数码管显示闪烁
#define PWM_WIDTH_ALL 15

// 呼吸模式
enum EnumBreathMode breathMode = DISPLAY_BREATH_MODE_OFF;
enum EnumSpeed breathSpeed = DISPLAY_SPEED_LV_6;
#define BRTH_BRIT_ARR_SIZE 32
static uchar code breathBrightArray[] = {
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1
};
static_idata_uchar nowBreathBright = 0;

// PWM亮度设置，不可以超过 PWM_WIDTH_ALL，不用修改
static_idata_uchar pwmBright = 1;

// PWM计数用，不用修改
static_idata_uchar ttPWM = 0;

// 用于软PWM，控制亮度用 ----------------------------------------------------------------

char* aniScrollLeftString;
static char aniScrollLeftLen = 0;
static char aniScrollLeftNowPos = 0;
static char aniScrollLeftEndPos = 0;
enum EnumDispMode aniScrollLeftNextMode;
enum EnumSpeed aniScrollLeftSpeed;

//画面每刷新一次计数器加一，可用于动画，闪烁等动态效果
static_idata_uchar frameCounter = 0;

idata uchar setYear, setMonth, setDay, setHour, setMinute, setSecond;

//第1,2,4位用数字显示code
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
//第3位数码管在硬件上是倒过来接的，数字显示code不一样
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
#define CHAR_DAT_INDEX_MINUS 28 //【-】
#define CHAR_DAT_INDEX_SP 29 	//【 】

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
			// A的ascii=65，减去54得到11，正好是本程序定义的字库里的A的数组下标
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

	// 默认返回空格
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

	// 共阳，位选高电平点亮（用的是三极管驱动，IO输出0，位选呈高电平）
	// 共阳，段码低电平点亮

	// 防止残影，先清屏，关闭所有显示
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

// 刷新显示用的内容，需要在main循环中调用
void DISPLAY_updateDisplay() {

	frameCounter++;
	ttflash++;

	// 切换闪烁状态(显示 / 不显示)
	if (ttflash == DISPLAY_SPEED_LV_2)
	{
		ttflash = 0;
		flagIsFlash = ~flagIsFlash;
	}

	// 向左滚动文字当前应该显示的位置
	if (dispMode == DISP_MODE_ANI_SCROLL_LEFT &&
			frameCounter % aniScrollLeftSpeed == 0)
	{
		// 本次滚动动画完成后，进入下一个状态
		if (aniScrollLeftNowPos == aniScrollLeftEndPos)
		{
			aniScrollLeftNowPos = 0;
			dispMode = aniScrollLeftNextMode;
		} else {
			aniScrollLeftNowPos++;
		}
	}

	// 呼吸效果
	if (breathMode == DISPLAY_BREATH_MODE_ON &&
			frameCounter % breathSpeed == 0)
	{
		nowBreathBright++;
		if (nowBreathBright == BRTH_BRIT_ARR_SIZE) {
			nowBreathBright = 0;
		}
		pwmBright = breathBrightArray[nowBreathBright];
	}

	// 如果当前正在暂时显示状态下，则判断有没有到时间，如果到时间了就切换回原来的显示模式
	if (flagIsShowingForAWhile)
	{
		showForAWhileInterval--;
		if (showForAWhileInterval <= 0)
		{
			// 时间到，恢复之前的状态
			flagIsShowingForAWhile = 0;
			dispMode = dispModeBefore;
		}
	}

	switch (dispMode)
	{
		// 最后的& 0x7f是为了显示dp
		case DISP_MODE_HHMM:
			// 显示HH:MM
			dispDat[0] = digit124[getShiWei(DS1302_GetHour())];
			dispDat[1] = digit124[getGeWei(DS1302_GetHour())]		& (flagIsFlash ? 0x7f : 0xff);		
			dispDat[2] =   digit3[getShiWei(DS1302_GetMinute())]	& (flagIsFlash ? 0x7f : 0xff);
			dispDat[3] = digit124[getGeWei(DS1302_GetMinute())];
			break;

		case DISP_MODE_MMDD:
			// 显示MM.DD
			dispDat[0] = digit124[getShiWei(DS1302_GetMonth())];
			dispDat[1] = digit124[getGeWei(DS1302_GetMonth())] 		& 0x7f;
			dispDat[2] =   digit3[getShiWei(DS1302_GetDay())];
			dispDat[3] = digit124[getGeWei(DS1302_GetDay())];
			break;
		
		case DISP_MODE_YYYY:
			// 显示YYYY
			dispDat[0] = digit124[2];
			dispDat[1] = digit124[0];
			dispDat[2] =   digit3[getShiWei(DS1302_GetYear())];
			dispDat[3] = digit124[getGeWei(DS1302_GetYear())];
			break;

		case DISP_MODE_MMSS:
			// 显示MM:SS
			dispDat[0] = digit124[getShiWei(DS1302_GetMinute())];
			dispDat[1] = digit124[getGeWei(DS1302_GetMinute())] 	& 0x7f;
			dispDat[2] =   digit3[getShiWei(DS1302_GetSecond())] 	& 0x7f;
			dispDat[3] = digit124[getGeWei(DS1302_GetSecond())];
			break;
		
		case DISP_MODE_ANI_SCROLL_LEFT:
			// 显示一段向左滚动的文字
			dispDat[0] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos  , 0);
			dispDat[1] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos+1, 0);
			dispDat[2] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos+2, 1);
			dispDat[3] = getDispDatByString(aniScrollLeftString, aniScrollLeftNowPos+3, 0);
			break;
		
		case DISP_MODE_SET_YEAR:
			// 设置年[YYYY]
			dispDat[0] = digit124[2];
			dispDat[1] = digit124[0];
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setYear)] : 0xff;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setYear)] : 0xff;
			break;
		
		case DISP_MODE_SET_MONTH:
			// 设置月[Y-MM]
			dispDat[0] = digit124[getGeWei(setYear)];
			dispDat[1] = getDispDatByChar('-', 0);
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setMonth)] : 0xff;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setMonth)] : 0xff;
			break;
		
		case DISP_MODE_SET_DAY:
			// 设置日[M-DD]
			dispDat[0] = digit124[getGeWei(setMonth)];
			dispDat[1] = getDispDatByChar('-', 0);
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setDay)] : 0xff;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setDay)] : 0xff;
			break;
		
		case DISP_MODE_SET_HOUR:
			// 设置小时[HH:MM]
			dispDat[0] = flagIsFlash ? digit124[getShiWei(setHour)]        : 0xff;
			dispDat[1] = flagIsFlash ? digit124[getGeWei(setHour)]  & 0x7f : 0x7f;
			dispDat[2] = digit3[getShiWei(setMinute)] & 0x7f ;
			dispDat[3] = digit124[getGeWei(setMinute)];
			break;
		
		case DISP_MODE_SET_MINUTE:
			// 设置分钟[HH:MM]
			dispDat[0] = digit124[getShiWei(setHour)];
			dispDat[1] = digit124[getGeWei(setHour)] & 0x7f ;
			dispDat[2] = flagIsFlash ? digit3[getShiWei(setMinute)] & 0x7f : 0x7f;
			dispDat[3] = flagIsFlash ? digit124[getGeWei(setMinute)]       : 0xff;
			break;

		default:
			break;
	}
}

// 刷新显示，需要在main循环中调用
void DISPLAY_refreshDisplay() {
	
	// PWM亮度控制
	ttPWM++;
	if (ttPWM <= pwmBright)
	{
		showPosition(nowPos, dispDat[nowPos], DIG_ON);
	} else {
		showPosition(nowPos, dispDat[nowPos], DIG_OFF);
	}
	
	// 每完成一次PWM的整个宽度就刷新显示下一位数码管
	if (ttPWM >= PWM_WIDTH_ALL){
		ttPWM = 0;
		nowPos++;
		if (nowPos>=4)
		{
			nowPos=0;
		}
	}
}

// 设置亮度
void DISPLAY_setBrightness(uchar brightness){
	if (brightness <= PWM_WIDTH_ALL) {
		pwmBright = brightness;
	}
}

// 设置是否打开呼吸效果(亮度渐变)，注意如果打开呼吸效果则会无视光线传感器
void DISPLAY_SetBreathMode(enum EnumBreathMode mode, enum EnumSpeed speed){
	breathMode = mode;
	breathSpeed = speed;
}

// 显示一小会指定mode，然后切换回原来的显示状态
void showModeForAWhile(enum EnumDispMode mode, int interval) {

	// 当前为非暂时显示状态才允许切换
	if (flagIsShowingForAWhile == 0)
	{
		//记住之前的状态以便到时间后恢复
		dispModeBefore = dispMode;

		//切换状态
		dispMode = mode;

		//倒计时计数开始
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

// 显示一小会MMDD，然后切换回原来的显示状态
void DISPLAY_ShowMMDD_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_MMDD, interval);
}

void DISPLAY_ShowYYYY(){
	dispMode = DISP_MODE_YYYY;
}

// 显示一小会YYYY，然后切换回原来的显示状态
void DISPLAY_ShowYYYY_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_YYYY, interval);
}

void DISPLAY_ShowMMSS(){
	dispMode = DISP_MODE_MMSS;
}

// 显示一小会MMSS，然后切换回原来的显示状态
void DISPLAY_ShowMMSS_forAWhile(int interval){
	showModeForAWhile(DISP_MODE_MMSS, interval);
}

enum EnumDispMode DISPLAY_GetDispMode(){
	return dispMode;
}

// 显示一段向左滚动的文字
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
	// 单片机rom太小，装不下stdio.h，不能用sprintf
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

// 设置年
void DISPLAY_SetYearMode(){

	setYear = DS1302_GetYear();
	setMonth = DS1302_GetMonth();
	setDay = DS1302_GetDay();
	setHour = DS1302_GetHour();
	setMinute = DS1302_GetMinute();

	// 更新设置时画面显示用buff
	updateSetDateTimeDispBuff();

	// 开始播放动画，播放完以后进入设置YYYY状态
	DISPLAY_ShowAniScrollLeft(
		dispString, 0, strlen(dispString), 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_YEAR);
}

// 设置月
void DISPLAY_SetMonthMode(){

	// 更新设置时画面显示用buff
	updateSetDateTimeDispBuff();

	// 滚动到设置月的位置，然后进入设置MM状态
	DISPLAY_ShowAniScrollLeft(
		dispString, 4, 6, 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_MONTH);
}

// 设置日
void DISPLAY_SetDayMode(){

	// 更新设置时画面显示用buff
	updateSetDateTimeDispBuff();

	// 滚动到设置日的位置，然后进入设置MM状态
	DISPLAY_ShowAniScrollLeft(
		dispString, 7, 9, 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_DAY);
}

// 设置小时
void DISPLAY_SetHourMode(){

	// 更新设置时画面显示用buff
	updateSetDateTimeDispBuff();

	// 滚动到设置小时和分钟的位置，然后进入设置HH状态
	DISPLAY_ShowAniScrollLeft(
		dispString, 10, 14, 
		DISPLAY_SPEED_LV_3, 
		DISP_MODE_SET_HOUR);
}

// 设置分钟
void DISPLAY_SetMinuteMode(){

	// 更新设置时画面显示用buff
	updateSetDateTimeDispBuff();

	// 不用滚动直接进入设置MM状态
	dispMode = DISP_MODE_SET_MINUTE;
}

// 设置成功，写入1302，修改后的日期时间滚动显示一次后，回到默认显示状态HH:MM
void DISPLAY_SetComplite(){

	DS1302_WriteTime_Year(setYear);
	DS1302_WriteTime_Month(setMonth);
	DS1302_WriteTime_Day(setDay);
	DS1302_WriteTime_Hour(setHour);
	DS1302_WriteTime_Minute(setMinute);
	DS1302_WriteTime_Sec(0);

	// 更新设置时画面显示用buff
	updateSetDateTimeDispBuff();

	// 完整滚动显示一次后，回到默认显示状态HH:MM，完成时间设置
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
