#ifndef __ILI9163LCD_H
#define __ILI9163LCD_H

#include "common.h"

/*************************�˿�/���Ŷ�������***********************/

//����/����ѡ��˿�(DC),Ҳ���Ǻ�ɫС��LCD�ϱ�����AO�ӿ� ��ͨGPIO����
#define      LCD_DC_PORT                 P11

//RESET���� ��ͨGPIO����
#define      LCD_RESET_PORT              P10

//CS���� ��ͨGPIO����
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

void ILI9163_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL);  		//��ʾ�����ַ�
void ILI9163_ShowString(u8 x,u8 y, u8 *chr,u8 Char_Size,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL);	//��ʾ�ַ���   ������
void ILI9163_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL);		//��ʾһ������num

#endif
