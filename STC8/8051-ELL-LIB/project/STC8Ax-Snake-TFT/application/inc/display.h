#ifndef __TXS_SNAKE_DISPLAY_H_
#define __TXS_SNAKE_DISPLAY_H_

#include "common.h"

void DISP_Clear(void);     									//清屏				
void DISP_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);  		//显示单个字符
void DISP_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);		//显示一串数字num
void DISP_ShowString(u8 x,u8 y, u8 *chr,u8 Char_Size);	    //显示字符串   除汉字

unsigned char const* DISP_getPIC(uchar blockSts);
void DISP_updateGameBlock(uchar x, uchar y);

#endif
