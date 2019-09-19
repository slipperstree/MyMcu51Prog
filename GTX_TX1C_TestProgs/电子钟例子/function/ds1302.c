#include "../header/ds1302.h"
#include "../header/common.h"

static unsigned char nian,yue,ri,shi,fen,miao;

//==============================================================================
//	DS1302��ʹ���������˳�ʹ����ʶ
//	�ȶ�DS1302�ı�ʶ�����������ʶ��ȷ����˵��1302�Ѿ���ʹ���������ٳ�ʹ��
//==============================================================================
void DS1302_init()
{
	nian = 19;
	yue = 9;
	ri = 18;
	
	shi = 23;
	fen = 59;
	miao = 50;
}

//=========================================================================
//        ��ʱ��
//=========================================================================
void DS1302_GetTimeFromDS1302()
{
	miao++;
	if (miao==60){
		miao=0;fen++;
		if (fen==60){
			fen=0;shi++;
			if (shi==24){
				shi=0;ri++;
				if (ri==32){
					ri=1;yue++;
					if (yue==13){
						yue=1;nian++;
	}}}}}
}

// ���º���������ds1302д��ʱ��Ҫд��BCD�룬�Զ�ת����BCD����д��
// DS1302оƬд��ʱ��ĸ�ʽΪBDC�룬����ֱ��дԭ��������
// Ҫ�����ֵ�ʮλ���ڸ�4λ����λ���ڵ�4λ
// ����19�������Ҫ��ʮ��������0x19������ֱ����ʮ������19
// �����ǣ�
//      ��19����10���̣�Ҳ����ʮλ����1������4λ�ƶ�����4λ��ȥ(0x10)
//      �ټ���19����10��������Ҳ���Ǹ�λ����9(0x09)
//      ������������Ϳ����� 0x10|0x09=0x19
// Ŀǰ���ʱ�ӵ�Ӳ��û����ʾ���ڵĹ��ܣ��������ڲ��ֵ����ݶ�������

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Sec(uchar nonBcdData) {
	miao = nonBcdData;
}

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Min(uchar nonBcdData) {
	fen = nonBcdData;
}

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Hour(uchar nonBcdData) {
	shi = nonBcdData;
}

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Day(uchar nonBcdData) {
	ri = nonBcdData;
}

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Month(uchar nonBcdData) {
	yue = nonBcdData;
}

/*
 ����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
 */
void DS1302_WriteTime_Year(uchar nonBcdData) {
	nian = nonBcdData;
}

uchar DS1302_GetYear(){
	return nian;
}

uchar DS1302_GetMonth(){
	return yue;
}

uchar DS1302_GetDay(){
	return ri;
}

uchar DS1302_GetHour(){
	return shi;
}

uchar DS1302_GetMinute(){
	return fen;
}

uchar DS1302_GetSecond(){
	return miao;
}