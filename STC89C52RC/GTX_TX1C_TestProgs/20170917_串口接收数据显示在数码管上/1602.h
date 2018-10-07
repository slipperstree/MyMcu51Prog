/*============================================================
延时函数
==============================================================
程序调用说明:
	调用 delay(延迟毫秒数)
[使用11.0592M晶振]
============================================================*/
void delay(unsigned int ms)					
{
	unsigned int Temp;
	while(ms--){Temp=82;while(Temp--);}
}

void delay_1602(unsigned int x)
{
	unsigned int a,b;
	for(a=x;a>0;a--)
		for(b=10;b>0;b--); 
}

#define  LCD_Data P0
sbit LCD_RW=P3^1; // 天祥板子上给1602接入的RW引脚直接接到GND了，只能写，不能读。所以这个引脚随便设一个不使用的就行
sbit LCD_RS=P3^5;
sbit LCD_E=P3^4;

/*============================================================
1602液晶显示驱动程序
============================================================== 
参数定义示例：
	#define  LCD_Data P1
	sbit LCD_RW=P2^0;
	sbit LCD_RS=P2^1;
	sbit LCD_E=P2^2;
程序调用说明:
	1) 使用前需先调用 LCD_Init() 对LCD进行初始化
	2) 输出字符使用
		LCD_PutStr(字符串,输出位置)
	   输出数字使用
		LCD_PutNum(数字,输出位置)
	[注:]
	1) 输出位置为0~31间的整数,为-1时表示先清屏
	2) 函数返回值为一整形数,表示当前光标所在位置
	3) 此两个函数支持以下转义字符:
		\n	换行
		\b	退格
		\\	输出 \
		\'	输出 '
		\"	输出 "
调用示例:
	void main(void){
		int p;
		delay(400);
		LCD_Init();
		p=LCD_PutStr("=  Hi! It\'s me =\n",-1);
		p=LCD_PutStr("  Robot ver ",p);
		p=LCD_PutNum(1,p);
		while(1);
	}
=============================================================*/
void WriteCommandLCD(unsigned char WCLCD,BuysC);
unsigned char ReadDataLCD(void);
unsigned char ReadStatusLCD(void);
void LCD_Init(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
int LCD_PutStr(unsigned char *DData,int pos);
int LCD_PutNum(long num,int pos);
//写数据
void WriteDataLCD(unsigned char WDLCD)
{
	LCD_Data=WDLCD;
	LCD_RS=1;
	LCD_E=0;
	delay_1602(10);
	LCD_E=1;
	delay_1602(10);
	LCD_E=0;
	
	// ReadStatusLCD(); //检测忙
	// LCD_Data = WDLCD;
	// LCD_RS = 1;
	// LCD_RW = 0;
	// LCD_E = 0; //若晶振速度太高可以在这后加小的延时
	// LCD_E = 0; //延时
	// delay(10);
	// LCD_E = 1;
}
//写指令
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysC为0时忽略忙检测
{
	LCD_Data=WCLCD;
	LCD_RS=0;
	LCD_E=0;
	delay_1602(10);
	LCD_E=1;
	delay_1602(10);
	LCD_E=0;
	
	//if (BuysC) ReadStatusLCD(); //根据需要检测忙
	// LCD_Data = WCLCD;
	// LCD_RS = 0;
	// LCD_RW = 0; 
	// LCD_E = 0;
	// LCD_E = 0;
	// delay(10);
	// LCD_E = 1; 
}
//读状态)
unsigned char ReadStatusLCD(void)
{
	LCD_Data = 0xFF;
	LCD_E = 0;
	LCD_RS = 0;
	LCD_RW = 1;
	LCD_E = 1;
	while (LCD_Data & 0x80 == 0x80); //检测忙信号
	return(LCD_Data);
}
//按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	if(Y<2){
		Y &= 0x1;
		X &= 0xF; //限制X不能大于15，Y不能大于1
		if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
		X |= 0x80; //算出指令码
		WriteCommandLCD(X, 0); //这里不检测忙信号，发送地址码
		WriteDataLCD(DData);
	}
}
//LCD初始化
void LCD_Init(void)
{
	unsigned char pos;

	LCD_Data = 0;
	WriteCommandLCD(0x38,0); //三次显示模式设置，不检测忙信号
	delay(5); 
	WriteCommandLCD(0x38,0);
	delay(5); 
	WriteCommandLCD(0x38,0);
	delay(5); 
	WriteCommandLCD(0x38,1); //显示模式设置,开始要求每次检测忙信号
	WriteCommandLCD(0x08,1); //关闭显示
	WriteCommandLCD(0x01,1); //显示清屏
	WriteCommandLCD(0x06,1); // 显示光标移动设置
	WriteCommandLCD(0x0C,1); // 显示开及光标设置
	pos=LCD_PutStr("+ SunWise Robot\n",-1);
	LCD_PutStr("     verson 1.1",pos);
	delay(2000);
}
int LCD_PutStr(unsigned char *DData,int pos){
	unsigned char i;
	if(pos==-1){
		WriteCommandLCD(0x01,1);
		pos=0;
	}
	while((*DData)!='\0'){
		switch(*DData){
			case '\n':	//如果是\n，则换行
			{
				if(pos<17){
					for(i=pos;i<16;i++)DisplayOneChar(i%16, i/16, ' ');
					pos=16;
				}else{
					for(i=pos;i<32;i++)DisplayOneChar(i%16, i/16, ' ');
					pos=32;
				}
				break;
			}
			case '\b':	//如果是\b，则退格
			{
				if(pos>0)pos--;
				DisplayOneChar(pos%16, pos/16, ' ');
				break;
			}
			default:
			{
				if((*DData)<0x20){*DData=' ';}
				DisplayOneChar(pos%16, pos/16, *DData);
				pos++;
				break;
			}

		}
		DData++;
	}
	return(pos);
}
int LCD_PutNum(long num,int pos)
{
	long tmp=0;
	unsigned char numbits=0;
	if(pos==-1){
		WriteCommandLCD(0x01,1);
		pos=0;
	}
	if(num==0){
		DisplayOneChar(pos%16, pos/16, '0');
		pos++;
	}else{
		if(num<0){
			DisplayOneChar(pos%16, pos/16, '-');
			num*=(-1);
			pos++;
		}
		while(num){
			tmp=tmp*10+(num%10);
			num=num/10;
			numbits++;
		}
		while(tmp){
			DisplayOneChar(pos%16, pos/16, (tmp%10)+48);
			tmp=tmp/10;
			pos++;
			numbits--;
		}
		while(numbits--){
			DisplayOneChar(pos%16, pos/16, '0');
			pos++;
		}
	}
	return(pos);
}