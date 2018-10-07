/*============================================================
��ʱ����
==============================================================
�������˵��:
	���� delay(�ӳٺ�����)
[ʹ��11.0592M����]
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
sbit LCD_RW=P3^1; // ��������ϸ�1602�����RW����ֱ�ӽӵ�GND�ˣ�ֻ��д�����ܶ�������������������һ����ʹ�õľ���
sbit LCD_RS=P3^5;
sbit LCD_E=P3^4;

/*============================================================
1602Һ����ʾ��������
============================================================== 
��������ʾ����
	#define  LCD_Data P1
	sbit LCD_RW=P2^0;
	sbit LCD_RS=P2^1;
	sbit LCD_E=P2^2;
�������˵��:
	1) ʹ��ǰ���ȵ��� LCD_Init() ��LCD���г�ʼ��
	2) ����ַ�ʹ��
		LCD_PutStr(�ַ���,���λ��)
	   �������ʹ��
		LCD_PutNum(����,���λ��)
	[ע:]
	1) ���λ��Ϊ0~31�������,Ϊ-1ʱ��ʾ������
	2) ��������ֵΪһ������,��ʾ��ǰ�������λ��
	3) ����������֧������ת���ַ�:
		\n	����
		\b	�˸�
		\\	��� \
		\'	��� '
		\"	��� "
����ʾ��:
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
//д����
void WriteDataLCD(unsigned char WDLCD)
{
	LCD_Data=WDLCD;
	LCD_RS=1;
	LCD_E=0;
	delay_1602(10);
	LCD_E=1;
	delay_1602(10);
	LCD_E=0;
	
	// ReadStatusLCD(); //���æ
	// LCD_Data = WDLCD;
	// LCD_RS = 1;
	// LCD_RW = 0;
	// LCD_E = 0; //�������ٶ�̫�߿���������С����ʱ
	// LCD_E = 0; //��ʱ
	// delay(10);
	// LCD_E = 1;
}
//дָ��
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysCΪ0ʱ����æ���
{
	LCD_Data=WCLCD;
	LCD_RS=0;
	LCD_E=0;
	delay_1602(10);
	LCD_E=1;
	delay_1602(10);
	LCD_E=0;
	
	//if (BuysC) ReadStatusLCD(); //������Ҫ���æ
	// LCD_Data = WCLCD;
	// LCD_RS = 0;
	// LCD_RW = 0; 
	// LCD_E = 0;
	// LCD_E = 0;
	// delay(10);
	// LCD_E = 1; 
}
//��״̬)
unsigned char ReadStatusLCD(void)
{
	LCD_Data = 0xFF;
	LCD_E = 0;
	LCD_RS = 0;
	LCD_RW = 1;
	LCD_E = 1;
	while (LCD_Data & 0x80 == 0x80); //���æ�ź�
	return(LCD_Data);
}
//��ָ��λ����ʾһ���ַ�
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	if(Y<2){
		Y &= 0x1;
		X &= 0xF; //����X���ܴ���15��Y���ܴ���1
		if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
		X |= 0x80; //���ָ����
		WriteCommandLCD(X, 0); //���ﲻ���æ�źţ����͵�ַ��
		WriteDataLCD(DData);
	}
}
//LCD��ʼ��
void LCD_Init(void)
{
	unsigned char pos;

	LCD_Data = 0;
	WriteCommandLCD(0x38,0); //������ʾģʽ���ã������æ�ź�
	delay(5); 
	WriteCommandLCD(0x38,0);
	delay(5); 
	WriteCommandLCD(0x38,0);
	delay(5); 
	WriteCommandLCD(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	WriteCommandLCD(0x08,1); //�ر���ʾ
	WriteCommandLCD(0x01,1); //��ʾ����
	WriteCommandLCD(0x06,1); // ��ʾ����ƶ�����
	WriteCommandLCD(0x0C,1); // ��ʾ�����������
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
			case '\n':	//�����\n������
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
			case '\b':	//�����\b�����˸�
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