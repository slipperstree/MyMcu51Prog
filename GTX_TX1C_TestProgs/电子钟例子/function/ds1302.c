#include "../header/ds1302.h"
#include "../header/common.h"

static unsigned char nian,yue,ri,shi,fen,miao;

//==============================================================================
//	DS1302初使化，增加了初使化标识
//	先读DS1302的标识，如果读出标识正确，则说明1302已经初使化过，不再初使化
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
//        读时间
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

// 以下函数考虑了ds1302写入时需要写入BCD码，自动转换成BCD码再写入
// DS1302芯片写入时间的格式为BDC码，不能直接写原来的数字
// 要将数字的十位放在高4位，个位放在低4位
// 比如19这个数，要送十六进制数0x19，不能直接送十进制数19
// 方法是：
//      将19整除10的商，也就是十位数的1，左移4位移动到高4位上去(0x10)
//      再计算19除以10的余数，也就是个位数的9(0x09)
//      最后将两个数相或就可以了 0x10|0x09=0x19
// 目前这个时钟的硬件没有显示星期的功能，所以星期部分的数据丢弃不用

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Sec(uchar nonBcdData) {
	miao = nonBcdData;
}

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Min(uchar nonBcdData) {
	fen = nonBcdData;
}

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Hour(uchar nonBcdData) {
	shi = nonBcdData;
}

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Day(uchar nonBcdData) {
	ri = nonBcdData;
}

/*
 传入十进制数据，自动转成BCD并写入DS1302
 */
void DS1302_WriteTime_Month(uchar nonBcdData) {
	yue = nonBcdData;
}

/*
 传入十进制数据，自动转成BCD并写入DS1302
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