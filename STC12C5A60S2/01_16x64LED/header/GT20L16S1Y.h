#ifndef __GT20L16S1Y_H_
#define __GT20L16S1Y_H_

// ��оƬȡ��ǰ���ֵ�ǰƫ�����µ���������
void readICDataToBuffer( unsigned char* str);

// ��оƬȡ������Ӻ���
unsigned char* getICData_ASCII_8x16(unsigned char ch);
unsigned char* getICData_Hanzi_16x16(unsigned char* hz);

// ��оƬȡָ������ָ���е����������ֽڣ�����λ�ƣ�
void getICData_Col( unsigned char* str, unsigned char colIdx,  unsigned char* colData);

#endif
