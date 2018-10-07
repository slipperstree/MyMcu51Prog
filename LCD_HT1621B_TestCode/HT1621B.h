//-----------------------------------------------------------------------------
// HT1621B.h
//-----------------------------------------------------------------------------


#ifndef  _HT1621B_H_
#define  _HT1621B_H_

#define uchar unsigned char
#define uint unsigned int

#define ComMode 0x52  //4COM,1/3bias 1000 010 1001 0
#define RCosc 0x30    //内部RC振荡器(上电默认)1000 0011 0000
#define LCD_on 0x06   //打开LCD显示 偏压发生器1000 0000 0 11 0
#define LCD_off 0x04  //关闭LCD显示(上电默认)
#define Sys_en 0x02   //系统振荡器开 1000 0000 0010
#define Ctrl_cmd 0x80 //写控制命令 1000 
#define Data_cmd 0xa0 //写数据命令 1010 

//定义HT1621端口
#define SEG_NUM 12	  //段码数字个数,即Seg数量/2

sbit CS1=P3^2;
sbit WR1=P3^3;
sbit DATA1=P3^4;

//函数声明
void SendBit_1621(uchar sdata,uchar cnt) ;
void SendCmd_1621(uchar command);
void Write_1621(uchar addr,uchar sdata,uchar len);
void HT1621_all_off(void);
void HT1621_all_on(void);
void Init_1621(void);
void LCDoff(void);
void LCDon(void);

#endif  /* _HT1621B_H_ */