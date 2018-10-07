#include<reg51.h>
#define uint unsigned int
#define uchar unsigned char
sbit CS=P3^4;
sbit CLK=P3^3;
sbit DIN=P3^5;
uchar code table[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7E,0x18,0x18,0x18,0x18,0x7E,0x00,
0x00,0x66,0x99,0x81,0x42,0x24,0x18,0x00,
0x00,0x42,0x42,0x42,0x42,0x24,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
void writebyte(uchar sj)
{        uchar i;
        CS=0;
        for(i=0;i<8;i++)
        {
                CLK=0;
                DIN=sj&0x80;
                sj=sj<<1;
                CLK=1;
        }
}

void write_MAX7219(uchar address,uchar date)
{
        CS=0;
        writebyte(address);
        writebyte(date);
        CS=1;
}
void delay(uint x)
{
        uint i,j;
        for(i=0;i<x;i++)
                for(j=0;j<112;j++);
}
void init()
{        
        write_MAX7219(0x09,0x00);                        //����Ĵ�������ַ:0x09,д0x00ʱ��ѡND������7~0����ܣ�
   write_MAX7219(0x0a,0x01);                        //���ȼĴ���
        write_MAX7219(0x0b,0x03);                        //ɨ��Ĵ�����0x0_��_������ʾ����ܵĸ�����
        write_MAX7219(0x0c,0x01);                        //ʡ��Ĵ�����д0x00�������ģʽ��д0x01��������ģʽ��
        write_MAX7219(0x0f,0x00);                        //���ԼĴ�����д0x01�������ģʽ��д0x00��������ģʽ��
}
void main()
{
        uchar i,a,j,f;
        uchar wo;
        init();
        while(1)
        {
                   
                        for(i=1;i<9;i++)
                        {
                           //write_MAX7219(,);
                           write_MAX7219(i,table[wo++]);
                                delay(50);
                        }
                j++;
                i=1;
                if(j>=32)
                {
               j=0;
                }
                wo=j;
        }
}