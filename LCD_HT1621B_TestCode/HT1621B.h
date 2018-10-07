//-----------------------------------------------------------------------------
// HT1621B.h
//-----------------------------------------------------------------------------


#ifndef  _HT1621B_H_
#define  _HT1621B_H_

#define uchar unsigned char
#define uint unsigned int

#define ComMode 0x52  //4COM,1/3bias 1000 010 1001 0
#define RCosc 0x30    //�ڲ�RC����(�ϵ�Ĭ��)1000 0011 0000
#define LCD_on 0x06   //��LCD��ʾ ƫѹ������1000 0000 0 11 0
#define LCD_off 0x04  //�ر�LCD��ʾ(�ϵ�Ĭ��)
#define Sys_en 0x02   //ϵͳ������ 1000 0000 0010
#define Ctrl_cmd 0x80 //д�������� 1000 
#define Data_cmd 0xa0 //д�������� 1010 

//����HT1621�˿�
#define SEG_NUM 12	  //�������ָ���,��Seg����/2

sbit CS1=P3^2;
sbit WR1=P3^3;
sbit DATA1=P3^4;

//��������
void SendBit_1621(uchar sdata,uchar cnt) ;
void SendCmd_1621(uchar command);
void Write_1621(uchar addr,uchar sdata,uchar len);
void HT1621_all_off(void);
void HT1621_all_on(void);
void Init_1621(void);
void LCDoff(void);
void LCDon(void);

#endif  /* _HT1621B_H_ */