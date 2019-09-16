#ifndef __DS1302_H_
#define __DS1302_H_

//==============================================================================
//	DS1302初使化，增加了初使化标识
//	先读DS1302的标识，如果读出标识正确，则说明1302已经初使化过，不再初使化
//==============================================================================
void DS1302_init();

//=========================================================================
//        读时间
//out:    nian,yue,ri,shi,fen,miao
//Author: ChenLing
//Date:   2012.12.17
//注意，本函数已经考虑过DS1302读取的是BCD码的问题，已经将BCD码转换成了十进制数字了
//=========================================================================
void DS1302_GetTimeFromDS1302();

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Sec(unsigned char nonBcdData);

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Min(unsigned char nonBcdData);

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Hour(unsigned char nonBcdData);

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Day(unsigned char nonBcdData);

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Month(unsigned char nonBcdData);

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Year(unsigned char nonBcdData);

unsigned char DS1302_GetMonth();

unsigned char DS1302_GetDay();

unsigned char DS1302_GetHour();

unsigned char DS1302_GetMinute();

#endif