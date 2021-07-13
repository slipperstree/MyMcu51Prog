#include "ILI9163LCD.h"
#include "font.h"
#include "common.h"

void ILI9163_delay(unsigned int Count)
{
    unsigned char i,j;
    while (Count--)//Count形参控制延时次数
    {
        for(i=0;i<25;i++)
            for(j=0;j<20;j++);
    }
}

// SPI发送一个字节
uint8_t MY_SPI_SendByte(uint8_t dat)
{
    SPDAT = dat;
    while (!(SPI_GET_FLAG()));
    //TODO: wait a while... for test
    // _nop_();_nop_();_nop_();
    // ILI9163_delay(1);  
    SPI_CLEAR_FLAG();
    return SPDAT;
}

uint8_t MY_SPI_SendByte_nowait(uint8_t dat)
{
    // SPDAT = dat; 
    // SPI_CLEAR_FLAG();
    // return SPDAT;
    SPDAT = dat;
    while (!(SPI_GET_FLAG()));
    //TODO: wait a while... for test
    // ILI9163_delay(1);
    // _nop_();_nop_();_nop_();
    SPI_CLEAR_FLAG();
    return SPDAT;
}

void ILI9163_W_Command(unsigned char cmd){
    LCD_CLR_DC();
    MY_SPI_SendByte(cmd);
}

void ILI9163_W_Data(unsigned char dat){
    LCD_SET_DC();
    MY_SPI_SendByte(dat);
}

void ILI9163_Reset(void){
    LCD_SET_RESET();
    ILI9163_delay(5);
    LCD_CLR_RESET();
    ILI9163_delay(5);
    LCD_SET_RESET();
    ILI9163_delay(5);
}

void ILI9163_Init(void){

    LCD_CLR_CS();

    ILI9163_Reset(); // HardReset
    ILI9163_delay(200);

    ILI9163_W_Command(0x01); // Software reset
    ILI9163_delay(200);

    ILI9163_W_Command(0x11); // Exit Sleep
    ILI9163_delay(200);

    ILI9163_W_Command(0x26); // Set Default Gamma
    ILI9163_W_Data(0x04);
    ILI9163_delay(10);

    ILI9163_W_Command(0x3a); // Set Color Format
    ILI9163_W_Data(0x05);
    ILI9163_delay(10);

    ILI9163_W_Command(0x36); // RGB ，显示方向设置
    //ILI9163_W_Data(0xA8);
    ILI9163_W_Data(0x60);
    ILI9163_delay(10);

    ILI9163_W_Command(0x29); //  Display On
    ILI9163_delay(10);

    
    //---------

    

    // ILI9163_Reset(); // HardReset

    // ILI9163_W_Command(0x01); // Software reset

    // ILI9163_W_Command(0x11); // Exit Sleep
    // ILI9163_delay(200);

    // ILI9163_W_Command(0xB1);// Set Frame Rate
    //     ILI9163_W_Data(0x0e);
    //     ILI9163_W_Data(0x10);

    // ILI9163_W_Command(0xC0); // Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    //     //ILI9163_W_Data(0x08); // 4.4V
    //     ILI9163_W_Data(0x1F); // 3.0V ：经测试，LED，VCC接3.3V时两者设置都可以
    //     ILI9163_W_Data(0x00);

    // ILI9163_W_Command(0xC1); // Set BT[2:0] for AVDD & VCL & VGH & VGL
    //     ILI9163_W_Data(0x05);

    // ILI9163_W_Command(0xC5); // Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    //     //ILI9163_W_Data(0x38); //VComH = 3.9v
    //     ILI9163_W_Data(0x24); //VComH = 3.4v ：经测试，LED，VCC接3.3V时两者设置都可以
        
    //     ILI9163_W_Data(0x40); //VComL = -0.9v
    //     //ILI9163_W_Data(0x64); //VComL = 0.0v ：经测试，LED，VCC接3.3V时两者设置都可以

    // ILI9163_W_Command(0xf2); // Enable Gamma bit
    // ILI9163_W_Data(0x01);
    // ILI9163_W_Command(0xE0); 
    // ILI9163_W_Data(0x3f);// p1
    // ILI9163_W_Data(0x22);// p2
    // ILI9163_W_Data(0x20);// p3
    // ILI9163_W_Data(0x30);// p4
    // ILI9163_W_Data(0x29);// p5
    // ILI9163_W_Data(0x0c);// p6
    // ILI9163_W_Data(0x4e);// p7
    // ILI9163_W_Data(0xb7);// p8
    // ILI9163_W_Data(0x3c);// p9
    // ILI9163_W_Data(0x19);// p10
    // ILI9163_W_Data(0x22);// p11
    // ILI9163_W_Data(0x1e);// p12
    // ILI9163_W_Data(0x02);// p13
    // ILI9163_W_Data(0x01);// p14
    // ILI9163_W_Data(0x00);// p15
    // ILI9163_W_Command(0xE1); 
    // ILI9163_W_Data(0x00);// p1
    // ILI9163_W_Data(0x1b);// p2
    // ILI9163_W_Data(0x1f);// p3
    // ILI9163_W_Data(0x0f);// p4
    // ILI9163_W_Data(0x16);// p5
    // ILI9163_W_Data(0x13);// p6
    // ILI9163_W_Data(0x31);// p7
    // ILI9163_W_Data(0x84);// p8
    // ILI9163_W_Data(0x43);// p9
    // ILI9163_W_Data(0x06);// p10
    // ILI9163_W_Data(0x1d);// p11
    // ILI9163_W_Data(0x21);// p12
    // ILI9163_W_Data(0x3d);// p13
    // ILI9163_W_Data(0x3e);// p14
    // ILI9163_W_Data(0x3f);// p15
    // ILI9163_W_Command(0xB4); 
    // ILI9163_W_Data(0x00);
    // ILI9163_W_Command(0x26); // Set Default Gamma
    // ILI9163_W_Data(0x04);
    // ILI9163_W_Command(0x3a); // Set Color Format
    // ILI9163_W_Data(0x05);
    // ILI9163_W_Command(0x36); // RGB ，显示方向设置
    // ILI9163_W_Data(0xA8);
    // ILI9163_W_Command(0x29); //  Display On
}

static void setAddress(u16 x, u16 y, u16 W16, u16 H16){
    u8 byteData=0;

    LCD_CLR_CS();

    ILI9163_W_Command(0x2A); // Set Column Address
    byteData = (x>>8) & 0xff;           ILI9163_W_Data(byteData); // XStart H
    byteData = x & 0xff;                ILI9163_W_Data(byteData); // XStart L
    byteData = ((x + W16-1)>>8) & 0xff;   ILI9163_W_Data(byteData); // XEnd H
    byteData = (x + W16-1) & 0xff;        ILI9163_W_Data(byteData); // XEnd L
    ILI9163_W_Command(0x2B); // Set Page Address
    byteData = (y>>8) & 0xff;           ILI9163_W_Data(byteData); // YStart H
    byteData = y & 0xff;                ILI9163_W_Data(byteData); // YStart L
    byteData = ((y + H16-1)>>8) & 0xff;   ILI9163_W_Data(byteData); // YEnd H
    byteData = ((y + H16-1)) & 0xff;      ILI9163_W_Data(byteData); // YEnd L

    LCD_SET_CS();
}

void ILI9163_FILL_BACKGROUND_128x128(unsigned char colorH, unsigned char colorL){
    unsigned char i,j=0;

    LCD_CLR_CS();

    // 128 x 128
    ILI9163_W_Command(0x2A); // Set Column Address
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x7F);
    ILI9163_W_Command(0x2B); // Set Page Address
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x7F);

    ILI9163_W_Command(0x2C); // 开始写入显存

    LCD_SET_DC(); // data
    for (i = 0; i < 128; ++i){
        for (j = 0; j < 128; ++j){
            MY_SPI_SendByte(colorH);
            //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
            //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
            //TODO: wait a while... for test
            //ILI9163_delay(1);
            MY_SPI_SendByte(colorL);
            //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
            //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
            //TODO: wait a while... for test
            //ILI9163_delay(1);
            //MY_SPI_SendByte_no_wait(colorH);
            //MY_SPI_SendByte_no_wait(colorL);
      }
    }

    LCD_SET_CS();
    //LCD_CLR_CS();
}

void ILI9163_FILL_IMG_128x128(unsigned char const* img){

    u16 idx=0;

    LCD_CLR_CS();

    // 128 x 128
    ILI9163_W_Command(0x2A); // Set Column Address
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x7F);
    ILI9163_W_Command(0x2B); // Set Page Address
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x00);
    ILI9163_W_Data(0x7F);

    ILI9163_W_Command(0x2C); // 开始写入显存

    LCD_SET_DC(); // data

    for (idx = 0; idx < 32768; ++idx)
    {
        MY_SPI_SendByte_nowait(img[idx]);
        //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
        //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
        //ILI9163_delay(5);
    }

    LCD_SET_CS();
    //LCD_CLR_CS();
}

void ILI9163_FILL_BACKGROUND_240x320(unsigned char colorH, unsigned char colorL){
    ILI9163_FILL_Rectange(0, 0, 240, 320, colorH, colorL);
}

void ILI9163_FILL_BACKGROUND_SIZE(u16 W, u16 H, unsigned char colorH, unsigned char colorL){
    ILI9163_FILL_Rectange(0, 0, W, H, colorH, colorL);
}

void ILI9163_FILL_Rectange(u16 x, u16 y, u16 W16, u16 H16,
                            unsigned char colorH, unsigned char colorL){
    u16 i,j=0;

    setAddress(x, y, W16, H16);

    LCD_CLR_CS();

    ILI9163_W_Command(0x2C); // 开始写入显存

    LCD_SET_DC(); // data
    i=W16*H16;
    while(i--){
        MY_SPI_SendByte_nowait(colorH);
        //TODO: wait a while... for test
        //ILI9163_delay(1);
        MY_SPI_SendByte_nowait(colorL);
        //TODO: wait a while... for test
        //ILI9163_delay(1);
    }

    LCD_SET_CS();
    //LCD_CLR_CS();
}

//在指定位置显示一个字符,包括部分字符
//char:数据				 
//size:选择字体 16/8
void ILI9163_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size, 
                        u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL)
{      	
		unsigned char c=0,i=0,j=0;

		c=chr-' ';//得到偏移后的值
		if(Char_Size == 16)
		{
			#ifdef SUPPORT_CHAR_SIZE_16
				OLED_Set_Pos(x,y);	
				for(i=0;i<8;i++)
				OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
				OLED_Set_Pos(x,y+1);
				for(i=0;i<8;i++)
				OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			#endif
		}
		else
		{
			#ifdef SUPPORT_CHAR_SIZE_8
                setAddress(x, y, 8, 6);

                LCD_CLR_CS();
                ILI9163_W_Command(0x2C); // 开始写入显存
                LCD_SET_DC(); // data
                for(i=0;i<6;i++){
                    for(j=0;j<8;j++){
                        // 按位取点阵内容。1：绘制字体色，0：绘制背景色
                        if (((F6x8[c][i]<<j) & 0x80) == 0x80 )
                        {
                            MY_SPI_SendByte_nowait(colorFontH);
                            MY_SPI_SendByte_nowait(colorFontL);
                        }else{
                            MY_SPI_SendByte_nowait(colorBgH);
                            MY_SPI_SendByte_nowait(colorBgL);
                        }
                    }
                }

                LCD_SET_CS();
                //LCD_CLR_CS();
			#endif
		}
}

//显示一个字符号串
void ILI9163_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size, 
                        u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL)
{
	unsigned char j=0;

	while (chr[j]!='\0')
	{	
        ILI9163_ShowChar(x,y,chr[j],Char_Size,colorBgH,colorBgL,colorFontH,colorFontL);
		// 每输出一个字符x向后位移8
        x+=8;
        // 自动换行
		if(x>120){x=0;y+=8;}
			j++;
	}
}

//m^n函数
static u32 my_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//num:数值(0~4294967295);
void ILI9163_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2,u8 colorBgH, u8 colorBgL, u8 colorFontH, u8 colorFontL){
    u8 t,temp;
	u8 enshow=0;

	for(t=0;t<len;t++)
	{
		temp=(num/my_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				ILI9163_ShowChar(x,y+(size2)*t,' ',size2,colorBgH,colorBgL,colorFontH,colorFontL);
				continue;
			}else enshow=1; 
		 	 
		}
	 	ILI9163_ShowChar(x,y+(size2)*t,temp+'0',size2,colorBgH,colorBgL,colorFontH,colorFontL); 
	}
}
