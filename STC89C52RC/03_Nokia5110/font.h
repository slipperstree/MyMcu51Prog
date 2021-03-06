/*
********************************************************** 
*说明：该font.h 文件中包含的是5110显示所用的ASCII、部分汉字、
	   及图片字模。取模软件使用“PCtoLCD2002”，取模方式选项：
	   阴码、逆向、列行式、十六进制数。汉字取模：宽12 、高 12
**********************************************************
*/
// 6 x 8 font
// 1 pixel space at left and bottom
// index = ASCII - 32				
code unsigned char font6x12[][12] =
{
/*--  文字:     --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  !  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00},

/*--  文字:  "  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x0C,0x02,0x0C,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  #  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x90,0xD0,0xBC,0xD0,0xBC,0x90,0x00,0x03,0x00,0x03,0x00,0x00},

/*--  文字:  $  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x18,0x24,0xFE,0x44,0x8C,0x00,0x03,0x02,0x07,0x02,0x01,0x00},

/*--  文字:  %  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x18,0x24,0xD8,0xB0,0x4C,0x80,0x00,0x03,0x00,0x01,0x02,0x01},

/*--  文字:  &  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xC0,0x38,0xE4,0x38,0xE0,0x00,0x01,0x02,0x02,0x01,0x02,0x02},

/*--  文字:  '  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x08,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  (  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x00,0xF8,0x04,0x02,0x00,0x00,0x00,0x01,0x02,0x04},

/*--  文字:  )  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x02,0x04,0xF8,0x00,0x00,0x00,0x04,0x02,0x01,0x00,0x00},

/*--  文字:  *  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x90,0x60,0xF8,0x60,0x90,0x00,0x00,0x00,0x01,0x00,0x00,0x00},

/*--  文字:  +  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0x20,0xFC,0x20,0x20,0x00,0x00,0x00,0x01,0x00,0x00,0x00},

/*--  文字:  ,  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x00,0x00,0x00},

/*--  文字:  -  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  .  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00},

/*--  文字:  /  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x80,0x60,0x1C,0x02,0x00,0x04,0x03,0x00,0x00,0x00,0x00},

/*--  文字:  0  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF8,0x04,0x04,0x04,0xF8,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  1  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x08,0xFC,0x00,0x00,0x00,0x00,0x02,0x03,0x02,0x00,0x00},

/*--  文字:  2  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x18,0x84,0x44,0x24,0x18,0x00,0x03,0x02,0x02,0x02,0x02,0x00},

/*--  文字:  3  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x08,0x04,0x24,0x24,0xD8,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  4  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x40,0xB0,0x88,0xFC,0x80,0x00,0x00,0x00,0x00,0x03,0x02,0x00},

/*--  文字:  5  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x3C,0x24,0x24,0x24,0xC4,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  6  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF8,0x24,0x24,0x2C,0xC0,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  7  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x0C,0x04,0xE4,0x1C,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00},

/*--  文字:  8  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xD8,0x24,0x24,0x24,0xD8,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  9  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x38,0x44,0x44,0x44,0xF8,0x00,0x00,0x03,0x02,0x02,0x01,0x00},

/*--  文字:  :  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00},

/*--  文字:  ;  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00},

/*--  文字:  <  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x20,0x50,0x88,0x04,0x02,0x00,0x00,0x00,0x00,0x01,0x02},

/*--  文字:  =  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x90,0x90,0x90,0x90,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  >  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x02,0x04,0x88,0x50,0x20,0x00,0x02,0x01,0x00,0x00,0x00},

/*--  文字:  ?  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x18,0x04,0xC4,0x24,0x18,0x00,0x00,0x00,0x02,0x00,0x00,0x00},

/*--  文字:  @  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF8,0x04,0xE4,0x94,0xF8,0x00,0x01,0x02,0x02,0x02,0x02,0x00},

/*--  文字:  A  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0xE0,0x9C,0xF0,0x80,0x00,0x02,0x03,0x00,0x00,0x03,0x02},

/*--  文字:  B  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x24,0x24,0xD8,0x00,0x02,0x03,0x02,0x02,0x01,0x00},

/*--  文字:  C  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF8,0x04,0x04,0x04,0x0C,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  D  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x04,0x04,0xF8,0x00,0x02,0x03,0x02,0x02,0x01,0x00},

/*--  文字:  E  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x24,0x74,0x0C,0x00,0x02,0x03,0x02,0x02,0x03,0x00},

/*--  文字:  F  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x24,0x74,0x0C,0x00,0x02,0x03,0x02,0x00,0x00,0x00},

/*--  文字:  G  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF0,0x08,0x04,0x44,0xCC,0x40,0x00,0x01,0x02,0x02,0x01,0x00},

/*--  文字:  H  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x20,0x20,0xFC,0x04,0x02,0x03,0x00,0x00,0x03,0x02},

/*--  文字:  I  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0x04,0xFC,0x04,0x04,0x00,0x02,0x02,0x03,0x02,0x02,0x00},

/*--  文字:  J  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x04,0x04,0xFC,0x04,0x04,0x06,0x04,0x04,0x03,0x00,0x00},

/*--  文字:  K  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x24,0xD0,0x0C,0x04,0x02,0x03,0x02,0x00,0x03,0x02},

/*--  文字:  L  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x04,0x00,0x00,0x00,0x02,0x03,0x02,0x02,0x02,0x03},

/*--  文字:  M  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xFC,0x3C,0xC0,0x3C,0xFC,0x00,0x03,0x00,0x03,0x00,0x03,0x00},

/*--  文字:  N  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x30,0xC4,0xFC,0x04,0x02,0x03,0x02,0x00,0x03,0x00},

/*--  文字:  O  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF8,0x04,0x04,0x04,0xF8,0x00,0x01,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  P  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x24,0x24,0x18,0x00,0x02,0x03,0x02,0x00,0x00,0x00},

/*--  文字:  Q  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xF8,0x84,0x84,0x04,0xF8,0x00,0x01,0x02,0x02,0x07,0x05,0x00},

/*--  文字:  R  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x24,0x64,0x98,0x00,0x02,0x03,0x02,0x00,0x03,0x02},

/*--  文字:  S  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x18,0x24,0x24,0x44,0x8C,0x00,0x03,0x02,0x02,0x02,0x01,0x00},

/*--  文字:  T  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x0C,0x04,0xFC,0x04,0x0C,0x00,0x00,0x02,0x03,0x02,0x00,0x00},

/*--  文字:  U  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x00,0x00,0xFC,0x04,0x00,0x01,0x02,0x02,0x01,0x00},

/*--  文字:  V  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0x7C,0x80,0xE0,0x1C,0x04,0x00,0x00,0x03,0x00,0x00,0x00},

/*--  文字:  W  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x1C,0xE0,0x3C,0xE0,0x1C,0x00,0x00,0x03,0x00,0x03,0x00,0x00},

/*--  文字:  X  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0x9C,0x60,0x9C,0x04,0x00,0x02,0x03,0x00,0x03,0x02,0x00},

/*--  文字:  Y  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0x1C,0xE0,0x1C,0x04,0x00,0x00,0x02,0x03,0x02,0x00,0x00},

/*--  文字:  Z  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x0C,0x84,0x64,0x1C,0x04,0x00,0x02,0x03,0x02,0x02,0x03,0x00},

/*--  文字:  [  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0xFE,0x02,0x02,0x00,0x00,0x00,0x07,0x04,0x04,0x00},

/*--  文字:  |  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00},

/*--  文字:  ]  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x02,0x02,0xFE,0x00,0x00,0x00,0x04,0x04,0x07,0x00,0x00},

/*--  文字:  ^  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x04,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  _  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08},

/*--  文字:  '  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x08,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  a  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x40,0xA0,0xA0,0xC0,0x00,0x00,0x01,0x02,0x02,0x03,0x02},

/*--  文字:  b  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x20,0x20,0xC0,0x00,0x00,0x03,0x02,0x02,0x01,0x00},

/*--  文字:  c  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0xC0,0x20,0x20,0x60,0x00,0x00,0x01,0x02,0x02,0x02,0x00},

/*--  文字:  d  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0xC0,0x20,0x24,0xFC,0x00,0x00,0x01,0x02,0x02,0x03,0x02},

/*--  文字:  e  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0xC0,0xA0,0xA0,0xC0,0x00,0x00,0x01,0x02,0x02,0x02,0x00},

/*--  文字:  f  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x20,0xF8,0x24,0x24,0x04,0x00,0x02,0x03,0x02,0x02,0x00},

/*--  文字:  g  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x40,0xA0,0xA0,0x60,0x20,0x00,0x07,0x0A,0x0A,0x0A,0x04},

/*--  文字:  h  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x20,0x20,0xC0,0x00,0x02,0x03,0x02,0x00,0x03,0x02},

/*--  文字:  i  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x20,0xE4,0x00,0x00,0x00,0x00,0x02,0x03,0x02,0x00,0x00},

/*--  文字:  j  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x00,0x20,0xE4,0x00,0x00,0x08,0x08,0x08,0x07,0x00,0x00},

/*--  文字:  k  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0xFC,0x80,0xE0,0x20,0x20,0x02,0x03,0x02,0x00,0x03,0x02},

/*--  文字:  l  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x04,0x04,0xFC,0x00,0x00,0x00,0x02,0x02,0x03,0x02,0x02,0x00},

/*--  文字:  m  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0xE0,0x20,0xE0,0x20,0xC0,0x00,0x03,0x00,0x03,0x00,0x03,0x00},

/*--  文字:  n  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0xE0,0x20,0x20,0xC0,0x00,0x02,0x03,0x02,0x00,0x03,0x02},

/*--  文字:  o  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0xC0,0x20,0x20,0xC0,0x00,0x00,0x01,0x02,0x02,0x01,0x00},

/*--  文字:  p  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0xE0,0x20,0x20,0xC0,0x00,0x08,0x0F,0x0A,0x02,0x01,0x00},

/*--  文字:  q  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0xC0,0x20,0x20,0xE0,0x00,0x00,0x01,0x02,0x0A,0x0F,0x08},

/*--  文字:  r  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0xE0,0x40,0x20,0x20,0x00,0x02,0x03,0x02,0x00,0x00,0x00},

/*--  文字:  s  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x60,0xA0,0xA0,0x20,0x00,0x00,0x02,0x02,0x02,0x03,0x00},

/*--  文字:  t  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x20,0xF8,0x20,0x00,0x00,0x00,0x00,0x01,0x02,0x02,0x00},

/*--  文字:  u  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0xE0,0x00,0x20,0xE0,0x00,0x00,0x01,0x02,0x02,0x03,0x02},

/*--  文字:  v  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0xE0,0x20,0x80,0x60,0x20,0x00,0x00,0x03,0x01,0x00,0x00},

/*--  文字:  w  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x60,0x80,0xE0,0x80,0x60,0x00,0x00,0x03,0x00,0x03,0x00,0x00},

/*--  文字:  x  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0x60,0x80,0x60,0x20,0x00,0x02,0x03,0x00,0x03,0x02,0x00},

/*--  文字:  y  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x20,0xE0,0x20,0x80,0x60,0x20,0x08,0x08,0x07,0x01,0x00,0x00},

/*--  文字:  z  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=6x16  --*/
{0x00,0x20,0xA0,0x60,0x20,0x00,0x00,0x02,0x03,0x02,0x02,0x00},


};


typedef struct
{
  unsigned char Index[2];
  unsigned char Msk[24];  
}TYPE_FONT12X16;

 
code TYPE_FONT12X16 font12x16[]=
{    

/*--  文字:  我  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=12x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=12x16  --*/
"我",0x10,0x12,0x12,0xFF,0x91,0x10,0xFF,0x10,0x90,0x52,0x14,0x00,0x02,0x02,0x09,0x0F,
0x00,0x04,0x02,0x03,0x04,0x08,0x0E,0x00,

/*--  文字:  是  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=12x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=12x16  --*/
"是",0x40,0x40,0x5F,0x55,0x55,0xD5,0x55,0x55,0x5F,0x40,0x40,0x00,0x08,0x04,0x03,0x04,
0x08,0x0F,0x09,0x09,0x09,0x09,0x08,0x00,

/*--  文字:  中  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=12x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=12x16  --*/
"中",0x00,0xF8,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0xF8,0x00,0x00,0x00,0x01,0x00,0x00,
0x00,0x0F,0x00,0x00,0x00,0x01,0x00,0x00,

/*--  文字:  国  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=12x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=12x16  --*/
"国",0xFF,0x01,0x25,0x25,0x25,0xFD,0x25,0x65,0xA5,0x01,0xFF,0x00,0x0F,0x05,0x05,0x05,
0x05,0x05,0x05,0x05,0x05,0x05,0x0F,0x00,

/*--  文字:  人  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=12x12   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=12x16  --*/
"银",0x98,0xF7,0x94,0x94,0x00,0xFF,0x49,0xC9,0x49,0x7F,0x80,0x00,0x00,0x0F,0x04,0x02,0x00,
0x0F,0x04,0x01,0x02,0x05,0x08,0x00,/*"",0*/



};


//数据水平，字节垂直，字节内像素数据反序
code unsigned char gImage_1[] = { 0x00,0x00,0xE0,0xF0,0xF8,0xF8,0xF8,0xF0,0xE0,0xF0,0xF8,
0xF8,0xF8,0xF0,0xE0,0x00,0x00,0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x3F,0x1F,0x0F,0x07,
0x03,0x01,0x00};





