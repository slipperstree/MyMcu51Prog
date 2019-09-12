/*
***************************************************************
									Nokia 5110����
*˵����������������51��Ƭ����5V��3.3V��,Һ����VCC��5V��3.3V
		 �޸�ƫ�õ�ѹ�ɸı�5110������ʾ������
			 5110��5Vʱƫ�õ�ѹ��Ϊ��0xa7,0��,��3.3Vʱ��Ϊ��0xc0,0��
*���ߣ�JK
*�������ڣ�2013/1/26
*�޸����ڣ�
***************************************************************
*/
#include <reg52.h>
#include	"font.h"
#define uint unsigned int
#define uchar unsigned char
/*
**********************************************************
										IO�ڶ�����
**********************************************************
*/
sbit	LCD_RST = P2^4;
sbit	LCD_CE = P2^0;
sbit	LCD_DC = P2^1;
sbit	LCD_DIN = P2^2;
sbit	LCD_CLK = P2^3;
/*
**********************************************************
									 ����������
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
*��������LCD_Init
*���ܣ���ʼ��LCD
*��������

*����ֵ����
*�� ע��
**********************************************************
*/
void LCD_Init(void)
{
	unsigned char i;
	LCD_RST=0;					 //��λ
	for(i=0;i<150;i++);
	LCD_RST=1;
	LCD_WriteByte(0x21, 0);	// ʹ����չ��������LCDģʽ
	LCD_WriteByte(0xc0, 0);	// ����ƫ�õ�ѹ,�޸Ŀɸı�5110������ʾ������
				/*5110��5Vʱƫ�õ�ѹ��Ϊ��0xa7,0��,��3.3Vʱ��Ϊ��0xc0,0��*/
	LCD_WriteByte(0x20, 0);	// ʹ�û������������V=0��ˮƽѰַ
	LCD_Clear();									// ����
	LCD_WriteByte(0x0c, 0);	// �趨��ʾģʽ��������ʾ(��ͨ��ʾ)
	LCD_CE=0;
}

/*
**********************************************************
*��������LCD_WriteByte
*���ܣ�ģ��SPI�ӿ�ʱ��д����/����LCD
*������data		��д������ݣ�
			command ��д���ݣ�1��/����ѡ��0����
*����ֵ����
*�� ע��
**********************************************************
*/
void LCD_WriteByte(unsigned char dat, unsigned char command)
{
	unsigned char i;
	LCD_CE=0;					//5110Ƭѡ��Ч��������������
	if (command == 0)				//д����
		LCD_DC=0;
	else
		LCD_DC=1;							//д����
	for(i=0;i<8;i++)				//����8bit����
	{
		if(dat&0x80)
			LCD_DIN=1;
		else
			LCD_DIN=0;
		LCD_CLK=0;
		dat = dat << 1;
		LCD_CLK=1;
	}
	LCD_CE=1;					 //��ֹ5110
}
/*
**********************************************************
*��������LCD_Set_XY
*���ܣ� ����LCD����
*������ X:0-83 Y:0-5
*����ֵ����
*�� ע��
**********************************************************
*/
void LCD_Set_XY(unsigned char X, unsigned char Y)
{
	LCD_WriteByte(0x40 | Y, 0);// column	��
	LCD_WriteByte(0x80 | X, 0);// row		 ��
}
/*
**********************************************************
*��������LCD_Clear
*���ܣ� LCD��������
*��������
*����ֵ����
*�� ע��
**********************************************************
*/
void LCD_Clear(void)
{
	unsigned char t;
	unsigned char k;
	LCD_Set_XY(0, 0);//����RAM��ʼ��ַ
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
*��������LCD_WriteChar
*���ܣ� LCDдһ��6X8���ַ�
*������c
*����ֵ����
�� ע��
**********************************************************
*/
void LCD_WriteChar(unsigned char x,unsigned char y,unsigned char c)
{
	unsigned char i;
	c -= 32;											 //������к�
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
*��������LCD_WriteString
*���ܣ� LCDд6X8���ַ���
*������X , Y , S
*����ֵ����
�� ע��
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
*��������LCD_WriteNum(unsigned int num)
*���ܣ�д������
*������num
*����ֵ����
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
*��������LCD_Write_16X16Font
*���ܣ� дһ��16X16�ĺ���
*������x , y , c[2]	x:0-83 y:0-5
*����ֵ����
*�� ע��
**********************************************************
*/
void LCD_Write_12X16Font(unsigned char x,unsigned char y,unsigned char c[2])
{
	unsigned char i,k;
	LCD_Set_XY(x,y);
	for(k=0; k<25; k++)		//K��ֵ��ʾ���ֿ�����ŵ��ֵ��������ɸĴ�
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
*��������LCD_DrawPicture
*���ܣ� ��ͼ
*������		X��Y		��λͼ���Ƶ���ʼX��Y���ꣻ
					*map		��λͼ�������ݣ�
					Pix_x	 ��λͼ���أ����� <=84
					Pix_y	 ��λͼ���أ��� <=48
*����ֵ����
*�� ע��
**********************************************************
*/
void LCD_DrawPicture(unsigned char X,unsigned char Y,unsigned char const *map,
									unsigned char Pix_x,unsigned char Pix_y)
{
	unsigned int i,n;
	unsigned char row;
	//����λͼ��ռ����
	if (Pix_y%8==0)	 //���Ϊλͼ��ռ����Ϊ����
		row=Pix_y/8;
	else
		row=Pix_y/8+1;	//���Ϊλͼ��ռ������������

	LCD_Set_XY(X,Y);
	for (n=0;n<row;n++)	//����
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
				-------main����--------
*��������������
*�� ע��
**********************************************************
*/
void main()
{
	LCD_Init();
	while(1)
	{
		//LCD_WriteNum(0,0,24510); // д������ int�͵�

		//LCD_WriteNum(18,0,0); // д������ int�͵�
		//LCD_WriteNum(24,0,5);
		//LCD_WriteChar(48,0,'I');// дһ��6X8���ַ�
		//LCD_WriteChar(54,0,'J');
		//LCD_WriteChar(60,0,'K');

		LCD_WriteString(0,0,"Hello,Chenling");		//д6X8���ַ���

		LCD_DrawPicture(32,2,gImage_1,16,16); //��ͼ
		LCD_Write_12X16Font(12,4,"��");	 //дһ��16X16�ĺ���
		LCD_Write_12X16Font(24,4,"��");
		LCD_Write_12X16Font(36,4,"��");
		LCD_Write_12X16Font(48,4,"��");
		LCD_Write_12X16Font(60,4,"��");
	}
}
