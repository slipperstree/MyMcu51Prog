#ifndef __DS1302_H_
#define __DS1302_H_

//==============================================================================
//	DS1302��ʹ���������˳�ʹ����ʶ
//	�ȶ�DS1302�ı�ʶ�����������ʶ��ȷ����˵��1302�Ѿ���ʹ���������ٳ�ʹ��
//==============================================================================
void DS1302_init();

//=========================================================================
//        ��ʱ��
//out:    nian,yue,ri,shi,fen,miao
//Author: ChenLing
//Date:   2012.12.17
//ע�⣬�������Ѿ����ǹ�DS1302��ȡ����BCD������⣬�Ѿ���BCD��ת������ʮ����������
//=========================================================================
void DS1302_GetTimeFromDS1302();

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Sec(unsigned char nonBcdData);

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Min(unsigned char nonBcdData);

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Hour(unsigned char nonBcdData);

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Day(unsigned char nonBcdData);

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Month(unsigned char nonBcdData);

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Year(unsigned char nonBcdData);

unsigned char DS1302_GetMonth();

unsigned char DS1302_GetDay();

unsigned char DS1302_GetHour();

unsigned char DS1302_GetMinute();

#endif