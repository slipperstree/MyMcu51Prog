#ifndef __ILI9163LCD_H
#define __ILI9163LCD_H

#include "stm32f10x.h"

/*************************端口/引脚定义区域***********************/
//命令/数据选择端口(DC),也就是红色小板LCD上标明的AO接口 普通GPIO即可
#define      LCD_DC_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      LCD_DC_CLK                  RCC_APB2Periph_GPIOA    
#define      LCD_DC_PORT                 GPIOA
#define      LCD_DC_PIN                  GPIO_Pin_2

//RESET引脚 普通GPIO即可
#define      LCD_RESET_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      LCD_RESET_CLK                  RCC_APB2Periph_GPIOA    
#define      LCD_RESET_PORT                 GPIOA
#define      LCD_RESET_PIN                  GPIO_Pin_3

//CS引脚 普通GPIO即可
#define      LCD_CS_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      LCD_CS_CLK                  RCC_APB2Periph_GPIOA    
#define      LCD_CS_PORT                 GPIOA
#define      LCD_CS_PIN                  GPIO_Pin_4

#define LCD_SET_CS()	GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN)
#define LCD_CLR_CS()	GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN)
#define LCD_SET_DC()	GPIO_SetBits(LCD_DC_PORT, LCD_DC_PIN)
#define LCD_CLR_DC()	GPIO_ResetBits(LCD_DC_PORT, LCD_DC_PIN)
#define LCD_SET_RESET()	GPIO_SetBits(LCD_RESET_PORT, LCD_RESET_PIN)
#define LCD_CLR_RESET()	GPIO_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN)

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
