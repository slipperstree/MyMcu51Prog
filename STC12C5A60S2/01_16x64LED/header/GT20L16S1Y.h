#ifndef __GT20L16S1Y_H_
#define __GT20L16S1Y_H_

// 从芯片取当前文字当前偏移量下的完整点阵
void readICDataToBuffer( unsigned char* str);

// 从芯片取点阵的子函数
unsigned char* getICData_ASCII_8x16(unsigned char ch);
unsigned char* getICData_Hanzi_16x16(unsigned char* hz);

// 从芯片取指定文字指定列的上下两个字节（用于位移）
void getICData_Col( unsigned char* str, unsigned char colIdx,  unsigned char* colData);

#endif
