#ifndef __ILI9163LCD_H
#define __ILI9163LCD_H

#include "common.h"

/*************************端口/引脚定义区域***********************/

//命令/数据选择端口(DC),也就是红色小板LCD上标明的AO接口 普通GPIO即可
#define      LCD_DC_PORT                 P11

//RESET引脚 普通GPIO即可
#define      LCD_RESET_PORT              P10

//CS引脚 普通GPIO即可
#define      LCD_CS_PORT                 P12

#define LCD_SET_CS()	LCD_CS_PORT=1;ILI9163_delay(10);
#define LCD_CLR_CS()	LCD_CS_PORT=0;ILI9163_delay(10);
#define LCD_SET_DC()	LCD_DC_PORT=1;ILI9163_delay(10);
#define LCD_CLR_DC()	LCD_DC_PORT=0;ILI9163_delay(10);
#define LCD_SET_RESET()	LCD_RESET_PORT=1;ILI9163_delay(10);
#define LCD_CLR_RESET()	LCD_RESET_PORT=0;ILI9163_delay(10);

void ILI9163_Init(void);
void ILI9163_delay(unsigned int Count);
void ILI9163_FILL_BACKGROUND_SIZE(u16 W, u16 H, unsigned char colorH, unsigned char colorL);
void ILI9163_FILL_BACKGROUND_128x128(unsigned char colorH, unsigned char colorL);
void ILI9163_FILL_BACKGROUND_240x320(unsigned char colorH, unsigned char colorL);
void ILI9163_FILL_IMG_128x128(unsigned char const* img);
void ILI9163_FILL_Rectange(u16 startX, u16 startY, u16 W16, u16 H16, unsigned char colorH, unsigned char colorL);

void ILI9163_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL);  		//显示单个字符
void ILI9163_ShowString(u8 x,u8 y, u8 *chr,u8 Char_Size,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL);	//显示字符串   除汉字
void ILI9163_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL);		//显示一串数字num

#endif
