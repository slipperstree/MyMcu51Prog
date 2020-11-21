/********************************************************/
/*�� �ߣ�Dyingstraw
/*�޸����ڣ�2015.5.2
/*��    ����V1.0
/*�����ܣ���51��Ƭ������ws2812 RGB�������ʾȫɫ��
/*��        ע��51��Ƭ������24M(STC15L104E����ͨ���������ͺ���Ҫ����ʵ����Ҫ������ʱ����)
/********************************************************/
 
//#include<reg52.h>
#include "STC15104E.h"
#include "intrins.h"
 
sbit WS2812 = P3^5;
 
#define numLEDs 10   //�Ƶĸ���
unsigned char buf_R[numLEDs] = {0};//��ɫ����
unsigned char buf_G[numLEDs] = {0};
unsigned char buf_B[numLEDs] = {0};
 
void RGB_Set_Up();  //��0��
void RGB_Set_Down(); //��1��
 
 
 
void HAL_Delay(unsigned int t)
{
		unsigned int x,y;
	  for(x=114;x>0;x--)
	  for(y=t;y>0;y--);
}
 
 
	 
//��λ��ʱ
//�����߼������ǵ��Եĵ���ʱ
void Delay50us()		//@22.1184MHz
{
	unsigned char i, j;
 
	_nop_();
	_nop_();
	i = 2;
	j = 150;
	do
	{
		while (--j);
	} while (--i);
}
 
 
//1�룬�ߵ�ƽ850ns �͵�ƽ400ns �������150ns
unsigned char speed_div_12 = 12;
void RGB_Set_Up()
{
		WS2812 = 1;
	  
		//�����߼������ǵ��Եĵ���ʱ
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
		//_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		
		WS2812 = 0;
}
 
//1�룬�ߵ�ƽ400ns �͵�ƽ850ns �������150ns
void RGB_Set_Down()
{
		WS2812 = 1;
		
		//�����߼������ǵ��Եĵ���ʱ
		//_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
	
		WS2812 = 0;
}
 
//����24λ����
void Send_2811_24bits(unsigned char G8,unsigned char R8,unsigned char B8)
{
	  
	  unsigned int n = 0;
	  //����G8λ
		for(n=0;n<8;n++)
		{
			G8<<=n;
			if(G8&0x80 == 0x80)
			{
				RGB_Set_Up();
			}
			else  
			{
			  RGB_Set_Down();
			}
		}
		//����R8λ
		for(n=0;n<8;n++)
		{
			R8<<=n;
			if(R8&0x80 == 0x80)
			{
				RGB_Set_Up();
			}
			else  
			{
				RGB_Set_Down();
			}
			
		}
		//����B8λ
	  for(n=0;n<8;n++)
		{
			B8<<=n;
			if(B8&0x80 == 0x80)
			{
				RGB_Set_Up();
			}
			else  
			{
			  RGB_Set_Down();
			}
		}
}
//��λ��
void RGB_Rst()
{
		WS2812 = 0;
		Delay50us();
}
//��24λ����GRB��תRGB
void Set_Colour(unsigned char r,unsigned char g,unsigned char b)
{
	  unsigned char i;
		for(i=0;i<numLEDs;i++)
	  {
				buf_R[i] = r; //����
			  buf_G[i] = g;
			  buf_B[i] = b;
		}
		for(i=0;i<numLEDs;i++)
		{
			Send_2811_24bits(buf_G[i],buf_R[i],buf_B[i]);//������ʾ
		}
}
//ĳһ������ʾ����ɫ
void SetPointColour(unsigned int num,unsigned char r,unsigned char g,unsigned char b)
{
	  unsigned char i;
		for(i=0;i<numLEDs;i++)
	  {
				buf_R[num] = r;//����
			  buf_G[num] = g;
			  buf_B[num] = b;
		}
		for(i=0;i<numLEDs;i++)
		{
			Send_2811_24bits(buf_G[i],buf_R[i],buf_B[i]);//������ʾ
		}
}
 
//��ɫ����24λ����ַ�
void SetPixelColor(unsigned char num,unsigned long c)
{
	  unsigned char i;
		for(i=0;i<numLEDs;i++)
	  {
				buf_R[num] = (unsigned char)(c>>16);
			  buf_G[num] = (unsigned char)(c>>8);
			  buf_B[num] = (unsigned char)(c);
		}
		for(i=0;i<numLEDs;i++)
		{
			Send_2811_24bits(buf_G[i],buf_R[i],buf_B[i]);
		}
}
 
//��λ
void PixelUpdate()
{
	RGB_Rst();
}
//��ɫ
unsigned long Color(unsigned char r, unsigned char g, unsigned char b)
{
  return ((unsigned long)r << 16) | ((unsigned long)g <<  8) | b;
}
 
//��ɫ�㷨
unsigned long Wheel(unsigned char WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) 
	{
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
 
//�ʺ�
void rainbow(unsigned int wait)
{
  unsigned int i, j;
 
  for(j=0; j<256; j++) 
	{
    for(i=0; i<numLEDs; i++)
		{
      SetPixelColor(i, Wheel((i+j) & 255));
    }
		PixelUpdate();
    HAL_Delay(wait);
  }
}
 
//��΢��ͬ���ǣ���ʹ�òʺ���ȷֲ�
void rainbowCycle(unsigned int wait) 
{
  unsigned int i, j;
 
  for(j=0;j<256*5;j++) 
	{ // 5 cycles of all colors on wheel  ������������ɫ��5��ѭ��
    for(i=0;i<numLEDs;i++) 
	 {
     SetPixelColor(i, Wheel(((i * 256 / numLEDs) + j) & 255));
    }
	  PixelUpdate();
    HAL_Delay (wait);
  }
}
 
//Theatre-style crawling lights.������
void theaterChase(unsigned long c, unsigned int wait) 
{
	int j,q;
	unsigned int i;
  for (j=0; j<10; j++) 
	{  //do 10 cycles of chasing  ��10��ѭ��
    for (q=0; q < 3; q++) 
		{
      for (i=0; i<numLEDs; i=i+3)
			{
        SetPixelColor(i+q, c);    //turn every third pixel on  ��ÿһ������������
      }
			PixelUpdate();
      HAL_Delay(wait);
 
      for (i=0; i<numLEDs; i=i+3) 
			{
       SetPixelColor(i+q, 0);        //turn every third pixel off   ��ÿһ�����������عص�
      }
			PixelUpdate();
    }
  }
}
 
//Theatre-style crawling lights with rainbow effect
//���вʺ�Ч����Ϸ��ʽ���е�
void theaterChaseRainbow(unsigned int wait) 
{
	int j,q;
	unsigned int i;
  for (j=0; j < 256; j++) 
	{     // cycle all 256 colors in the wheel ��������ѭ������256ɫ
    for (q=0; q < 3; q++)
		{
      for (i=0; i < numLEDs; i=i+3) 
			{
        SetPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel off ��ÿһ������������
      }
      PixelUpdate();
 
      HAL_Delay(wait);
 
      for (i=0; i < numLEDs; i=i+3)
			{
        SetPixelColor(i+q, 0);        //turn every third pixel off  ��ÿһ�����������عص�
      }
    }
  }
}
 
// Fill the dots one after the other with a color
//��һ����ɫ�����ЩԲ��
void colorWipe(unsigned long c, unsigned int wait) 
{
	unsigned int i=0;
  for( i=0; i<numLEDs; i++) 
	{
    SetPixelColor(i, c);
    PixelUpdate();
    HAL_Delay(wait);
  }
}
 
void main()
{	
	//CLK_DIV = 1; // RC(12M)/2
	  
		while(1)
		{
			// RGB_Set_Down();
			
			// Delay50us();
			
			// RGB_Set_Up();
			
			// Delay50us();
			
			//rainbow(45);
			//rainbowCycle(40);
			//theaterChase(Color(0,0,100),80); // Blue
			//theaterChase(Color(0,255,0),80); // Blue
			//theaterChase(Color(255,0,0),80); // Blue
			//theaterChaseRainbow(40);
			//colorWipe(255,255);
		}
}
