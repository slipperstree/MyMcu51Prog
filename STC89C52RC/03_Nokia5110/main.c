/*
***************************************************************
									Nokia 5110驱动
*说明：本驱动适用于51单片机（5V或3.3V）,液晶的VCC接5V或3.3V
		 修改偏置电压可改变5110点阵显示的亮度
			 5110接5V时偏置电压改为（0xa7,0）,接3.3V时改为（0xc0,0）
*作者：JK
*创建日期：2013/1/26
*修改日期：
***************************************************************
*/
#include <reg52.h>
#include	"font.h"
#define uint unsigned int
#define uchar unsigned char
/*
**********************************************************
										IO口定义区
**********************************************************
*/
sbit	LCD_RST = P2^4;
sbit	LCD_CE = P2^0;
sbit	LCD_DC = P2^1;
sbit	LCD_DIN = P2^2;
sbit	LCD_CLK = P2^3;
/*
**********************************************************
									 函数声明区
**********************************************************
*/
void LCD_Init(void);
void LCD_WriteByte(unsigned char dat, unsigned char command);
void LCD_Set_XY(unsigned char X, unsigned char Y);
void LCD_Clear(void);
void LCD_WriteChar(unsigned char x,unsigned char y,unsigned char c);
void LCD_WriteString(unsigned char X,unsigned char Y,char *s);
void LCD_WriteNum(unsigned char X,unsigned char Y,unsigned int num);
void LCD_Write_12X16Font(unsigned char x,unsigned char y,unsigned char c[2]);
void LCD_DrawPicture(unsigned char X,unsigned char Y,unsigned char const *map,
									unsigned char Pix_x,unsigned char Pix_y);
/*
**********************************************************
*函数名：LCD_Init
*功能：初始化LCD
*参数：无

*返回值：无
*备 注：
**********************************************************
*/
void LCD_Init(void)
{
	unsigned char i;
	LCD_RST=0;					 //复位
	for(i=0;i<150;i++);
	LCD_RST=1;
	LCD_WriteByte(0x21, 0);	// 使用扩展命令设置LCD模式
	LCD_WriteByte(0xc0, 0);	// 设置偏置电压,修改可改变5110点阵显示的亮度
				/*5110接5V时偏置电压改为（0xa7,0）,接3.3V时改为（0xc0,0）*/
	LCD_WriteByte(0x20, 0);	// 使用基本命令。并设置V=0，水平寻址
	LCD_Clear();									// 清屏
	LCD_WriteByte(0x0c, 0);	// 设定显示模式，正常显示(普通显示)
	LCD_CE=0;
}

/*
**********************************************************
*函数名：LCD_WriteByte
*功能：模拟SPI接口时序写数据/命令LCD
*参数：data		：写入的数据；
			command ：写数据（1）/命令选择（0）；
*返回值：无
*备 注：
**********************************************************
*/
void LCD_WriteByte(unsigned char dat, unsigned char command)
{
	unsigned char i;
	LCD_CE=0;					//5110片选有效，允许输入数据
	if (command == 0)				//写命令
		LCD_DC=0;
	else
		LCD_DC=1;							//写数据
	for(i=0;i<8;i++)				//传送8bit数据
	{
		if(dat&0x80)
			LCD_DIN=1;
		else
			LCD_DIN=0;
		LCD_CLK=0;
		dat = dat << 1;
		LCD_CLK=1;
	}
	LCD_CE=1;					 //禁止5110
}
/*
**********************************************************
*函数名：LCD_Set_XY
*功能： 设置LCD坐标
*参数： X:0-83 Y:0-5
*返回值：无
*备 注：
**********************************************************
*/
void LCD_Set_XY(unsigned char X, unsigned char Y)
{
	LCD_WriteByte(0x40 | Y, 0);// column	列
	LCD_WriteByte(0x80 | X, 0);// row		 行
}
/*
**********************************************************
*函数名：LCD_Clear
*功能： LCD清屏函数
*参数：无
*返回值：无
*备 注：
**********************************************************
*/
void LCD_Clear(void)
{
	unsigned char t;
	unsigned char k;
	LCD_Set_XY(0, 0);//设置RAM起始地址
	for(t=0;t<6;t++)
	{
		for(k=0;k<84;k++)
		{
			LCD_WriteByte(0x00,1);
		}
	}
}
/*
**********************************************************
*函数名：LCD_WriteChar
*功能： LCD写一个6X8的字符
*参数：c
*返回值：无
备 注：
**********************************************************
*/
void LCD_WriteChar(unsigned char x,unsigned char y,unsigned char c)
{
	unsigned char i;
	c -= 32;											 //数组的行号
	LCD_Set_XY(x,y);
	for(i=0; i<12; i++)
	{
		LCD_WriteByte(font6x12[c][i], 1);
		if(i==5)
		{
			y++;
			LCD_Set_XY(x,y);
		}
	}
}
/*
**********************************************************
*函数名：LCD_WriteString
*功能： LCD写6X8的字符串
*参数：X , Y , S
*返回值：无
备 注：
**********************************************************
*/
void LCD_WriteString(unsigned char X,unsigned char Y,char *s)
{
	while(*s)
	{
		 LCD_WriteChar(X,Y,*s);
		 s++;
		 X += 6;
	}
}
/*
***************************************************************
*函数名：LCD_WriteNum(unsigned int num)
*功能：写入数字
*参数：num
*返回值：无
****************************************************************
*/
void LCD_WriteNum(unsigned char X,unsigned char Y,unsigned int num)
{
	unsigned char str[8],i=0,len=0;
	unsigned int temp;
	temp = num;
	while(temp)
	{
		temp /=10;
		len++;
	}
	if(!num)
	{
		 len++;
		 str[0]=0x30;
	}
	str[len] = 0;
	while(num)
	{
		str[len-i-1] = num%10 + 0x30;
		num /=10;
		i++;
	}
	LCD_WriteString(X,Y,(char *)str);
}
/*
**********************************************************
*函数名：LCD_Write_16X16Font
*功能： 写一个16X16的汉字
*参数：x , y , c[2]	x:0-83 y:0-5
*返回值：无
*备 注：
**********************************************************
*/
void LCD_Write_12X16Font(unsigned char x,unsigned char y,unsigned char c[2])
{
	unsigned char i,k;
	LCD_Set_XY(x,y);
	for(k=0; k<25; k++)		//K的值表示汉字库最多存放的字的数量（可改大）
	{
		if((font12x16[k].Index[0]==c[0])&&(font12x16[k].Index[1]==c[1]))
		{
			for(i=0; i<24; i++)
			{
				LCD_WriteByte(font12x16[k].Msk[i], 1);
				if(i==11)
				{
					 y++;
					 LCD_Set_XY(x,y);
				}
			}
		}
	}
}
/*
**********************************************************
*函数名：LCD_DrawPicture
*功能： 绘图
*参数：		X、Y		：位图绘制的起始X、Y坐标；
					*map		：位图点阵数据；
					Pix_x	 ：位图像素（长） <=84
					Pix_y	 ：位图像素（宽） <=48
*返回值：无
*备 注：
**********************************************************
*/
void LCD_DrawPicture(unsigned char X,unsigned char Y,unsigned char const *map,
									unsigned char Pix_x,unsigned char Pix_y)
{
	unsigned int i,n;
	unsigned char row;
	//计算位图所占行数
	if (Pix_y%8==0)	 //如果为位图所占行数为整数
		row=Pix_y/8;
	else
		row=Pix_y/8+1;	//如果为位图所占行数不是整数

	LCD_Set_XY(X,Y);
	for (n=0;n<row;n++)	//换行
	{
		for(i=0;i<Pix_x;i++)
		{
			LCD_Set_XY(X+i,Y+n);
			LCD_WriteByte(map[i+n*Pix_x], 1);
		}
	}
}
/*
**********************************************************
				-------main函数--------
*函数名：主函数
*备 注：
**********************************************************
*/
void main()
{
	LCD_Init();
	while(1)
	{
		//LCD_WriteNum(0,0,24510); // 写入数字 int型的

		//LCD_WriteNum(18,0,0); // 写入数字 int型的
		//LCD_WriteNum(24,0,5);
		//LCD_WriteChar(48,0,'I');// 写一个6X8的字符
		//LCD_WriteChar(54,0,'J');
		//LCD_WriteChar(60,0,'K');

		LCD_WriteString(0,0,"Hello,Chenling");		//写6X8的字符串

		LCD_DrawPicture(32,2,gImage_1,16,16); //绘图
		LCD_Write_12X16Font(12,4,"我");	 //写一个16X16的汉字
		LCD_Write_12X16Font(24,4,"是");
		LCD_Write_12X16Font(36,4,"中");
		LCD_Write_12X16Font(48,4,"国");
		LCD_Write_12X16Font(60,4,"银");
	}
}
