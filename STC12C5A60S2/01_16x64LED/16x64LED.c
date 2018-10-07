
/*******************************************
�������ƣ�4������ģ����ʾ���֡��ǳ���л��
�ⲿ����˵����
��Ƭ��ѡ�ͣ����������ʹ��RAMΪ128���ϵĵ�Ƭ��������STC89C52 STC89C**,STC90C*��12T�ĵ�Ƭ��
            Ҳ����ʹ��STC12C5A*,STC11F*��1T�ĵ�Ƭ��������ʹ��AT89C51 AT89C52 AT89S51 
            AT89S52��RAMֻ��128�ĵ�Ƭ����

���������ʹ��12M���ϵľ��塣����ʹ��24M���ϵľ��壬����Ƶ��Խ�ߣ���ʾ�ֲ��������˸�С�
          ���ʹ��24M���µľ��壬�����STC��оƬ�������6T(˫����)���ܡ�
          
�˿ڶ��壺ģ���A����ӵ�Ƭ����P1.4��ģ���B����ӵ�Ƭ����P1.5��ģ���C����ӵ�Ƭ����P1.6
          ģ���D����ӵ�Ƭ����P1.7��ģ���LATCH����ӵ�Ƭ����P1.0
		  ģ���CLK����ӵ�Ƭ����P1.1��ģ���DI(������������DA_port)����ӵ�Ƭ����P1.2
		  ģ���G(������������EN_port)����ӵ�Ƭ����P1.3
*/

/*ͷ�ļ�*/
#include <STC12C5A60S2.h>      
#include <intrins.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char
#define NOP() _nop_()

/*�˿ڶ���*/

sbit EN_port = P1^3;
sbit DA_in_port = P1^2;
sbit CLK_port = P1^1;
sbit Latch_port = P1^0;

// for test led
sbit LED = P3^7;

// ���ڲ��� --------------------------------------------------------------------------------
typedef unsigned char BYTE;
typedef unsigned int WORD;

//#define FOSC 11059200L      //System frequency
#define FOSC 32000000L      //System frequency
#define BAUD 600           //UART baudrate
//#define BAUD 115200           //UART baudrate

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT NONE_PARITY   //Testing even parity

void SendData(BYTE dat);
void SendString(char *s);
void UartInit();

//sbit bit9 = P2^2;           //P2.2 show UART data bit9
bit busy;

uchar rec[10]={0,0,0,0,0,0,0,0,0,0};
uchar recIdx=0;
// ���ڲ��� --------------------------------------------------------------------------------

#define ABCD_port P1  //HC138 ABCD�˿ڶ���
//sbit D_port = P1^7;
//sbit C_port = P1^6;
//sbit B_port = P1^5;
//sbit A_port = P1^4;

// GT20L16S1Y �ӿ� ------------------------------------------------
sbit pCE        =P2^0;         //
sbit pSCLK      =P2^1;         //
sbit pSI        =P2^2;         //
sbit pSO        =P2^3;         //

unsigned long addr;
int idx = 0;
uchar data ic_data[32];

uchar* getICData_ASCII_8x16(uchar ch);
uchar* getICData_Hanzi_16x16(uchar* hz);
void readICDataToBuffer(uchar bufNum, uchar* str);

// GT20L16S1Y �ӿ� ------------------------------------------------

void HC595_Data_Send(uchar *p, uchar han, uchar zishu, uchar offset);
void setICDataToBuffer(uchar nowBufferNum, uchar *pICData, uchar size, uchar pos);

// ����16X64������Ϣ��128+32�ֽڣ���������֣���һ��������Ϊ����λ��ʾ�õĻ��壩
uchar xdata bufHZ[1][128+32] = {
	{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	},
	// {
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	// }
};

uchar code ic_data_A[] = {0x00,0xE0,0x9C,0x82,0x9C,0xE0,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x0F,0x00};

uchar nowOffset=0;
int nowPos=0;

// int showDataSize=960*2;
uchar code showData[] = "������ɫ˼�������������󲻵á������Ů�����ɣ����������δʶ������������������һ��ѡ�ھ����ࡣ����һЦ��������������������ɫ��������ԡ����أ���Ȫˮ��ϴ��֬���̶�����������ʼ���³ж���ʱ�����޻��ս�ҡ��ܽ����ů�ȴ�������������ո��𣬴Ӵ˾������糯���л���������Ͼ�����Ӵ���ҹרҹ���󹬼�����ǧ�ˣ���ǧ�谮��һ������ױ�ɽ���ҹ����¥�����ʹ�����õ��ֽ�����������������Ż����������¸�ĸ�ģ�������������Ů���깬�ߴ������ƣ����ַ�Ʈ�����š�����������˿�񣬾��վ��������㡣����ܱ�Ķ��������������������������س����̳�����ǧ�����������С��仪ҡҡ�и�ֹ���������Ű���������������κΣ���ת��ü��ǰ��������ί�������գ����̽�ȸ��ɦͷ����������Ȳ��ã��ؿ�Ѫ����������ư�ɢ������������ջ�����ǽ��󡣶���ɽ�������У�����޹���ɫ������ˮ����ɽ�࣬ʥ������ĺĺ�顣�й���������ɫ��ҹ�����峦������������ת����Ԧ�����˳������ȥ���������������У��������տ�������������˾�մ�£�������������顣������Է�����ɣ�̫Һܽ��δ������ܽ����������ü���Դ���β��ᴹ�����������ҹ��������ͩҶ��ʱ���������ڶ���ݣ���Ҷ���׺첻ɨ����԰���Ӱ׷��£�������������ϡ�Ϧ��ө��˼��Ȼ���µ�����δ���ߡ��ٳ��ӹĳ���ҹ�������Ǻ������졣ԧ������˪���أ��������˭�빲�������������꣬���ǲ��������Ρ�������ʿ�趼�ͣ����Ծ����»��ǡ�Ϊ�о���չת˼����̷�ʿ�����١��ſ�Ԧ������磬���������֮�顣��������»�Ȫ������ãã�Բ��������ź�������ɽ��ɽ����������䡣¥���������������д�Լ�����ӡ�����һ����̫�棬ѩ����ò�β��ǡ���������ߵ���磬ת��С��˫�ɡ��ŵ���������ʹ���Ż������λ꾪�������������ǻأ��鲭�������ƿ������ް�ƫ��˯�������ڲ������������紵����Ʈҡ�٣��������������衣���ݼ�į�����ɣ��滨һ֦�����ꡣ��������л������һ����������ã����������������������������³�����ͷ������徴�����������������Ψ����������飬��Ͻ��μĽ�ȥ������һ�ɺ�һ�ȣ���뢻ƽ�Ϸ��䡣�������ƽ���ᣬ�����˼��������ٱ������ؼĴʣ�������������֪���������ճ����ҹ������˽��ʱ������Ը���������ڵ�ԸΪ����֦���쳤�ؾ���ʱ�����˺������޾��ڡ�";
int showDataSize=297*2;
//uchar code showData[] = "���Ǻ������Ķ�������ǵ��𣿺�������һ�������Ҹշ�ѿ�����߹���û�л�ͷ���Ҽǵã��ҿ����ˡ����Ǻ������Ķ�������ǵ��𣿼ǵ�����һ������ʢ���绨���ҳ��裬û�ж��ң����Ҽǵã����ҿ����ˡ����Ǻ������ļ�����ǵ��𣿺�������һ������Ϧ�����£����������Ҳ��Һ���˵�����㾭����ʱ���𸡶��ҵķ������Ǻ������ļ�����ǵ��𣿼ǵ�����һ����������ѩ�������߹���û�л�ͷ���Ҽǵã��ҿ����ˡ����Ǻ������ļ�����ǵ�����ʱ�㻹�Ǹ��������ڴ����£��Ҳ���������ֿ�����ǽ�ϣ��һ�����������������������Ǻ������ļ�����ǵ��𣿵��������������������ģ����������������˭���룬��������һ�о�����һ�������Ǻ������ļ�����";

// ȡģ�������Ҫָ��������+˳�򣨸�λ��ǰ��+����ʽ������ʽ
// ��ʹ���ֿ�оƬ��ʹ�ù̶�����ʱ�ſ���һ�Σ����޸ĳ������ bufHZ Ϊ ziku_table
// uchar code ziku_table[]={
// 0x00,0x30,0xC0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x60,0x10,/*"[",0*/

// 0x00,0x78,0x48,0x57,0x50,0x61,0x51,0x4A,0x4B,0x48,0x69,0x51,0x42,0x44,0x40,0x40,
// 0x40,0x40,0x40,0xFE,0x80,0x20,0x20,0x20,0xFC,0x20,0x28,0x24,0x22,0x22,0xA0,0x40,/*"��",1*/

// 0x00,0xFF,0x01,0x01,0x01,0x7F,0x41,0x41,0x49,0x45,0x41,0x49,0x45,0x41,0x41,0x40,
// 0x00,0xFE,0x00,0x00,0x00,0xFC,0x04,0x04,0x44,0x24,0x04,0x44,0x24,0x04,0x14,0x08,/*"��",2*/

// 0x00,0xFF,0x02,0x7A,0x4A,0x7A,0x00,0xFF,0x02,0x7A,0x4A,0x4A,0x7A,0x02,0x0A,0x04,
// 0x20,0x20,0x20,0x7E,0x42,0x84,0x10,0x10,0x10,0x10,0x28,0x28,0x28,0x44,0x44,0x82,/*"��",3*/

// 0x00,0xC0,0x30,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x20,0xC0,/*"]",4*/
// };

// һ���Դ���HC595�õ�64+16λ��һ�������ݣ�������λ�õĵ�������ֵĻ�������
uchar data disp_buff[10];

int tt = 0;
int tt1 = 0;
void Timer0Init(void)		//20����@30.000MHz
{
	EA=1;
    ET0=1;
	
	//AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0xB0;		//���ö�ʱ��ֵ
	TH0 = 0x3C;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

uchar rowIdx=0; // �ڼ���
uchar nowBufferNum=0;
void display(){
	uchar i;
	
	// ѭ���Ӵ�buffer��ȡ��ǰ�д����ҵ�ȫ��8���ֽ���Ϊһ�������ݱ�������ʾ�õ�buff��
	for (i=0;i<10;i++){
		// �����Ǵ�buffer��ʾ��ͼ��ÿ����Ҫȡһ�е�����
		// ��������������������������������������������������������������������������������������������������������������������
		// B0	B16		B32		B48		B64		B80		B96		B112   --> һ�������ݴ���595���
		// ��������������������������������������������������������������������������������������������������������������������
		// B1	B17		.		.		.       .       .       .
		// ��������������������������������������������������������������������������������������������������������������������
		// B2	B18		.		.		.       .       .       .
		// ��������������������������������������������������������������������������������������������������������������������
		// .	.		.       .       .       .       .       .
		// .	.		.       .       .       .       .       .
		// .	.		.       .       .       .       .       .
		// ��������������������������������������������������������������������������������������������������������������������
		// B15	B31		B47		B63		B79		B95		B111	B127
		// ��������������������������������������������������������������������������������������������������������������������
		disp_buff[i]=bufHZ[nowBufferNum][i*16 + rowIdx];
	}
	
	// ������ȡ����һ�������ݴ��������74HC595���У�������ǰ�е�����
	HC595_Data_Send(disp_buff,rowIdx,4,nowOffset);
	
	// ��ǰ������1��������һ�е�ɨ��
	rowIdx++;
	
	// ɨ����16�У��ص���һ�м�����ɨ��
	if(rowIdx==16) rowIdx=0;
}

void main()
{
	UartInit();
	
	Timer0Init();
	
	// For test --------------------------------------------------------------------------------
	//readICDataToBuffer(nowBufferNum, getICData_Hanzi_16x16("ȫ"), 32, 0);
	readICDataToBuffer(nowBufferNum, showData);
	//readICDataToBuffer(nowBufferNum,"�����ʻ�");
	
	//setICDataToBuffer(nowBufferNum, getICData_ASCII_8x16('A'), 16, 0);
	//setICDataToBuffer(nowBufferNum, ic_data_A, 16, 0);
	// For test --------------------------------------------------------------------------------

	while (1){
		display();
		
		if(recIdx >= 8){
			readICDataToBuffer(nowBufferNum,rec);
			recIdx=0;
		}
	}
}

// �����յ���IC�������ݴ��뻺����������ʾ
// pICData	- ���յ��ĵ�������
// size		- ��������Ĵ�С��ͨ������Ϊ32�ֽڣ����ASCII��Ϊ16�ֽڡ�
// pos		- ָ���õ���������ʾ�Ŀ�ʼλ��(0-7)����8λ���У�Ϊһ����λ���Ḳ��ԭ�������ݡ�
void setICDataToBuffer(uchar bufNum, uchar *pICData, uchar size, uchar pos)
{
	// ############# ���ֿ�оƬGT20L16S1Yȡ���ĵ�����Ϣ�����ú���ģʽ��������ĸA�� ############# 
	// ���ֿ�оƬGT20L16S1Yȡ���ĵ�����Ϣ�����ú���ģʽ��Ҳ����������Ϊ��λȡ��������
	// ---- �����оƬȡ������ĸA�ĵ������� ���ú��ŵ� ---- 
	// B0	B1	B2	B3	B4	B5	B6	B7
	// 0	0	0	0	0	0	0	0
	// 0	0	0	��	0	0	0	0
	// 0	0	��	0	��	0	0	0
	// 0	0	��	0	��	0	0	0
	// 0	0	��	0	��	0	0	0
	// 0	��	0	0	0	��	0	0
	// 0	��	0	0	0	��	0	0
	// 0	��	��	��	��	��	0	0
	// B8	B9	B10	B11	B12	B13	B14	B15
	// ��	0	0	0	0	0	��	0
	// ��	0	0	0	0	0	��	0
	// ��	0	0	0	0	0	��	0
	// ��	0	0	0	0	0	��	0
	// 0	0	0	0	0	0	0	0
	// 0	0	0	0	0	0	0	0
	// 0	0	0	0	0	0	0	0
	// 0	0	0	0	0	0	0	0
	// ����ͷ������������������ʽ����������Ҳ����ÿ��ɨ��ʱ��Ҫ����һ���еĵ�����Ϣ
	// ������Ҫ��ÿ��ȡ���ĵ�����Ϣת�浽�������ţ�Ҳ���Ǻ���ȡģ����еġ�����ʽ������ʽ��buf��ȥ��
	// ��ʾʱ��buf��ȡ������һ���е�8���ֽڵ����ݴ��д���595����ɨ����ʾ
	// ����ŵ��������������ڵ���ʾλ���й�ϵ���������Ǵ��㽫�������A�ĵ�����ʾ����Ļ�������
	// ��ô��ת���Ժ�Ӧ��������������ӣ��Ҳ������ʡ�ԣ�
	// Ҳ����˵Ҫ��ȡ����B0-B7�����λ��ϳ�buf���һ���ֽ�B0���ַ�AΪ������0x00��
	// ��ȡ����B0-B7�Ĵθ�λ��ϳ�buf����ֽ�B1���ַ�AΪ������0x10��
	// B0	0	0	0	0	0	0	0	0
	// B1	0	0	0	��	0	0	0	0
	// B2	0	0	��	0	��	0	0	0
	// B3	0	0	��	0	��	0	0	0
	// B4	0	0	��	0	��	0	0	0
	// B5	0	��	0	0	0	��	0	0
	// B6	0	��	0	0	0	��	0	0
	// B7	0	��	��	��	��	��	0	0
	// B8	��	0	0	0	0	0	��	0
	// B9	��	0	0	0	0	0	��	0
	// B10	��	0	0	0	0	0	��	0
	// B11	��	0	0	0	0	0	��	0
	// B12	0	0	0	0	0	0	0	0
	// B13	0	0	0	0	0	0	0	0
	// B14	0	0	0	0	0	0	0	0
	// B15	0	0	0	0	0	0	0	0

	uchar bufByteIdx = 0;
	uchar icByteIdx = 0;
	
	uchar* pTmpByteBufHZ;
	
	//clear target data
	//memset(&bufHZ[nowBufferNum][pos*16], size, 0x00);
	
	switch(size){
		case 32:
			// ����
			// ���ֵ�IC��������16��X16��: 
			// B0-B7 ΪPOS+0�������8�����ݣ�B16-B23ΪPOS+0�������8������
			// B8-B15ΪPOS+1�������8�����ݣ�B24-B31ΪPOS+1�������8������
			// ���Դ�buff��B0-B7   �����ݴ�IC���ݵ�B0-B7   ��ȡ���������л�����
			//     ��buff��B8-B15  �����ݴ�IC���ݵ�B16-B23 ��ȡ���������л�����
			//     ��buff��B16-B23 �����ݴ�IC���ݵ�B8-B15  ��ȡ���������л�����
			//     ��buff��B24-B31 �����ݴ�IC���ݵ�B24-B31 ��ȡ���������л�����
			for (bufByteIdx=0; bufByteIdx<size; bufByteIdx++){
				pTmpByteBufHZ = &(bufHZ[bufNum][pos*16 + bufByteIdx]);
				display();//test
				// �����Ŀ���ֽ�
				pTmpByteBufHZ[0] = 0x00;
				
				if(bufByteIdx < 8){
					for (icByteIdx=0; icByteIdx<8; icByteIdx++){
						// ÿ�θ����λ��ֵ������һλ�����õ�8λ����
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							// ���ic�ĸ�λ������1����������ֽ����λ��ֵ1
							pTmpByteBufHZ[0] |= 0x01;
						} else {
							// ����ֵ0
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
				
				if(bufByteIdx >= 8 && bufByteIdx < 16 ){
					for (icByteIdx=16; icByteIdx<24; icByteIdx++){
						pTmpByteBufHZ[0] <<= 1;
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							pTmpByteBufHZ[0] |= 0x01;
						} else {
							// ����ֵ0
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
				
				if(bufByteIdx >= 16 && bufByteIdx < 24 ){
					for (icByteIdx=8; icByteIdx<16; icByteIdx++){
						pTmpByteBufHZ[0] <<= 1;
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							pTmpByteBufHZ[0] |= 0x01;
						} else {
							// ����ֵ0
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
				
				if(bufByteIdx >= 24 && bufByteIdx < 32 ){
					for (icByteIdx=24; icByteIdx<32; icByteIdx++){
						pTmpByteBufHZ[0] <<= 1;
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							pTmpByteBufHZ[0] |= 0x01;
						} else {
							// ����ֵ0
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
			}
			break;
		case 16:
			// ���ACSII����
			// ����ַ���IC��������8��X16��: B0-B7Ϊ�������8�����ݣ�B8-B15Ϊ�������8�����ݣ���������ַ�A�����ӣ�
			// ���Դ�buff��B0-B7 �����ݴ�IC���ݵ�B0-B7 ��ȡ���������л�����
			//   ����buff��B8-B15�����ݴ�IC���ݵ�B8-B15��ȡ���������л�����
			for (bufByteIdx=0; bufByteIdx<size; bufByteIdx++){
				//display();//test
					
				pTmpByteBufHZ = &(bufHZ[bufNum][pos*16 + bufByteIdx]);
				
				// �����Ŀ���ֽ�
				pTmpByteBufHZ[0] = 0x00;
				
				if(bufByteIdx < 8){
					for (icByteIdx=0; icByteIdx<8; icByteIdx++){
						// ÿ�θ����λ��ֵ������һλ�����õ�8λ����
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							// ���ic�ĸ�λ������1����������ֽ����λ��ֵ1
							pTmpByteBufHZ[0] |= 0x01; // 0000 0001
						} else {
							// ����ֵ0��Ĭ����0����ʲô������
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
				
				if(bufByteIdx >= 8 && bufByteIdx < 16 ){
					for (icByteIdx=8; icByteIdx<16; icByteIdx++){
						// ÿ�θ����λ��ֵ������һλ�����õ�8λ����
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							// ���ic�ĸ�λ������1����������ֽ����λ��ֵ1
							pTmpByteBufHZ[0] |= 0x01; // 0000 0001
						} else {
							// ����ֵ0��Ĭ����0����ʲô������
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
			}
			break;
		default:
			break;
	}
	
}

// ��ʱֻ֧����ǰ���8���ֽڵ���ʾ��8����ǻ�4��ȫ�ǣ�
void readICDataToBuffer(uchar bufNum, uchar* str){
	uchar pos;
	
	// GB2312-80����ı��뷶Χ�Ǹ�λ0xa1��0xfe����λ�� 0xa1-0xfe ��
	// ���к��ַ�ΧΪ 0xb0a1 �� 0xf7fe�����ֻ�Ǽ򵥵��жϺ��֣���ֻҪ�鿴���ֽ��Ƿ���ڵ���0xa1�Ϳ�����
	for (pos=0; pos<10; pos++){
		if (str[pos] >= 0x20 && str[pos] <= 0x7E) {
			// �ж�Ϊ���ASCII�룬����ASCII���ȡ������������ڵ�ǰ����ʾλ����
			setICDataToBuffer(bufNum, getICData_ASCII_8x16(str[pos]), 16, pos); 
		} else {
			// �ж�Ϊȫ�Ǻ��֣���ʵ���кܶ��Ŀ��ܱ������ĺ��֣���������Ĭ��������ַ�����GB2312���룩
			// ���ú��ֵ�ȡ������������ڵ�ǰ����ʾλ����
			setICDataToBuffer(bufNum, getICData_Hanzi_16x16(&str[pos]), 32, pos);
			pos++;//���ں���ռ�����ֽڣ������ֶ���ѭ������������һ���ֽ�
		}
	}
}

// ȡ�õ�������
// addr: ����õĵ������ݵ���ʼ��ַ
// size������ʼ��ַ����ȡ�����ֽڵĵ������ݣ�Ŀǰֻ֧��16��ASCII�룩��32��GB2312���֣�����
uchar* getICData(unsigned long addr, uchar size) {
	
	//init
	pCE=1;
	pSI=0;
	pSCLK=0;
	
	//memset(ic_data, 32, 0x00);
	
	// ���Ȱ�Ƭѡ�źţ�CS#����Ϊ��
	pCE=0;
	
	// �����ŵ���ͨ�����������������ţ�SI����λ���� 1 ���ֽڵ������֣�03 h��
	// ÿһλ�ڴ���ʱ�ӣ�SCLK�������ر����档
	for (idx = 0; idx<8; idx++){
		pSCLK=0;
		pSI=0x03<<idx & 0x80;
		pSCLK=1;
	}
	
	// Ȼ�����ͨ�����������������ţ�SI����λ���� 3 ���ֽڵĵ�ַ
	// ÿһλ�ڴ���ʱ�ӣ�SCLK�������ر����档
	for (idx = 0; idx<24; idx++){
		pSCLK=0;
		pSI=addr<<idx & 0x800000;
		
		// For test break point
		if(idx==23){
			idx = 23;
		}
		
		pSCLK=1;
	}
	
	// ������ϸ���Է��֣�оƬ�������ݵ�ʱ���߸��ٷ��ĵ���������
	// �ٷ��ĵ�������ʱ�����Ǵ�������������һλ�Ժ�ĵ�һ��SCLK���½��ػ��MO���ص�һ���ֽڵ����λ��
	// ʵ���ϣ��ڴ�����������һ�������ص�˲�䣬MO���Ѿ���ʼ���ش������ˣ�Ȼ������žͻ���ÿ���½��ش��غ��������
	// ��֪��Ϊʲô���������ϵ�оƬȷʵ��ˣ�����һ��ʼ����ȱ�ٵ�һλ����
	// �����������ѭ��֮ǰ��ȡ��һλ������
	
	// ��֣���stc12c5a60s2ʱ��ʱ����û�������ˣ��㲻������������������ʱ�����ˡ�����
	// ��������������䣬����Ϊ������ʾ������λ��һλ������
	//ic_data[0] = ic_data[0] | pSO;
	//ic_data[0] <<= 1;
	
	// Ȼ��оƬ�Ὣ�õ�ַ���ֽ�����ͨ����������������ţ�SO����λ�����
	// ÿһλ�ڴ���ʱ�ӣ�SCLK���½��ر��Ƴ���
	// ���ڵ�һλ�����Ѿ�ȡ���ˣ����������ѭ����1������0��ʼ
	for (idx = 0; idx<8*size ; idx++){
		pSCLK=0;
		
		// ÿ�β����½��أ�IC���������һλ��������
		// ȡ����һλ�������ݴ���ڵ�ǰ�ֽڵ����λ
		// ע�⣬λ��ֵʱ������ֱ���û����㣡����Ϊ���������޷���ԭ����1�ĳ�0����
		//       ���������Ļ�����һ�л�����޹���ĵ㣬��Щ�㶼�Ǵ����鷶Χ����λ�ƽ�������û�б���Ϊ0�ĵ㡣��pSO=0��
		if (pSO == 0) {
			ic_data[idx/8] &= 0xFE; // 1111 1110 
		} else {
			ic_data[idx/8] |= 0x01; // 0000 0001
		}
		
		if(((idx+1)%8) > 0){
			// ���������Կճ����λ�Ա����������һλ����
			ic_data[idx/8] <<= 1;
		} else {
			// ÿ���ֽ�ֻ��Ҫλ��7�μ��ɣ����һλ��Ҳ�������������λ�����������Ժ���������
			// ����ᶪʧ���λ�����ݣ�����������ƫ��
			//ic_data[idx/8] >>= 1;
		}
		
		pSCLK=1;
		
		display();//test
	}
	
	// ��ȡ�ֽ����ݺ����Ƭѡ�źţ�CS#����Ϊ�ߣ��������β�����
	pCE=1;
	return ic_data;
}

uchar* getICData_ASCII_8x16(uchar ch) {
	// ASCII�ַ����ڵ�ַ�ļ��㹫ʽ
	addr = (ch-0x20)*16+0x3b7c0;
	return getICData(addr, 16);
}

uchar* getICData_Hanzi_16x16(uchar* hz) {
	unsigned long hzGBCodeH8, hzGBCodeL8;
	
	// ���ں����е�ַ�ļ��㣬���Ա���ʹ�ÿ����ɴ�����long����Ϊ�м��������������ַ����ʱ�����
	// ֱ�ӽ�uchar�͸�ֵ��long�͵Ļ���û�и�ֵ���ĸ�λ�п����ҵ������Ը�0xFF����һ�Σ�ȷ��ֻ����uchar���ֵ�����
	hzGBCodeH8 = hz[0] & 0xFF;
	hzGBCodeL8 = hz[1] & 0xFF;
	
	// GB2312�������ڵ�ַ�ļ��㹫ʽ
	if(hzGBCodeH8 ==0xA9 && hzGBCodeL8 >=0xA1)
		addr = (282 + (hzGBCodeL8 - 0xA1 ))*32;
	else if(hzGBCodeH8 >=0xA1 && hzGBCodeH8 <= 0xA3 && hzGBCodeL8 >=0xA1)
		addr =( (hzGBCodeH8 - 0xA1) * 94 + (hzGBCodeL8 - 0xA1))*32;
	else if(hzGBCodeH8 >=0xB0 && hzGBCodeH8 <= 0xF7 && hzGBCodeL8 >=0xA1) {
		// addr = ((hzGBCodeH8 - 0xB0) * 94 + (hzGBCodeL8 - 0xA1)+ 846)*32;
		addr = (hzGBCodeH8 - 0xB0) * 94;
		addr += (hzGBCodeL8 - 0xA1) + 846;
		addr *= 32;
	}
	
	return getICData(addr, 32);
}

/*--------------------------------------------------------------
    ����LED�ڼ�����ʾ����
   ����*pΪ����ĵ������ݣ�hanΪ�ڼ�����ʾ��zishuΪ��ʾ���ٸ���
---------------------------------------------------------------*/
void HC595_Data_Send(uchar *p, uchar han, uchar zishu, uchar offset)
{
	uchar i=0;
	uchar temp=0;
	uchar* data_buff;
	Latch_port = 0;  /*HC595�������,�������ݴ�������У������ݱ仯�Ӷ���ʾ��˸*/
	CLK_port = 0;
	
	// ǰ���4������8���ֽ����ͳ�ȥ
	for(temp=0;temp<8;temp++){
		data_buff = &p[temp];
		for(i=0;i<8;i++){
			if(((data_buff[0]<<i)&0x80)!=0) DA_in_port = 0;
			else DA_in_port = 1;
			CLK_port = 1;
			CLK_port = 0;
		}
	}
	
	// Ϊ��ʵ������Ч���������͵�������ֻ�������ǰƫ�Ƶ�λ��
	for(i=0;i<offset;i++){
		// ����λ�����ֱ�ʹ�õ�������ֵĸ߰�λ��ڰ�λ
		data_buff = &p[temp + (i+8)/8-1]; //TODO
		if(((data_buff[0]<<(i%8))&0x80)!=0) DA_in_port = 0;
		else DA_in_port = 1;
		CLK_port = 1;
		CLK_port = 0;
	}

	EN_port = 1; /*������ʾ��ԭ��ΪʹHC138���ȫΪ1���Ӷ������ܽ�ֹ��������ʾ*/

	ABCD_port = (ABCD_port & 0x0f)|(han<<4);

	Latch_port = 1; /*����HC595���������Q1-Q8�˿�*/
	EN_port = 0; /*HC138�����Ч������ʾ*/
	Latch_port = 0;	/*����HC595�������*/

}

uchar isLeft=0;
void timer0() interrupt 1
{
	// 20ms
	tt++;
	TL0 = 0xB0;		//���ö�ʱ��ֵ
	TH0 = 0xFC;		//���ö�ʱ��ֵ
	
	//if(tt >= 5){
	if(tt >= 50){
		// 1s
		tt = 0;
		
		// if(isLeft==0){
			// nowOffset++;
			// if(nowOffset==16){
				// isLeft=1;
			// }
		// } else {
			// nowOffset--;
			// if(nowOffset==0){
				// isLeft=0;
			// }
		// }
		
		if(nowOffset==15){
			
			EN_port = 1; //off screen
			
			nowOffset=0;
			
			// �л���ʾ��̨�Ѿ�׼�������ݵ�buffer
			if (nowBufferNum == 0) {
				//nowBufferNum = 1;
				nowBufferNum = 0; //89C52���ڴ治���󣬲�����˫���棬STC1260S2����
				readICDataToBuffer(0, &showData[nowPos+=2]);
			} else {
				nowBufferNum = 0;
				readICDataToBuffer(1, &showData[nowPos+=2]);
			}
			
			//Ϊ�˱�����µĺ�������ռ�ô���ʱ�䵼����ʾʱ�䲻����������Ļ����˸������
			//��ȡic����ʱҲ��Ҫ����display����
			if(nowPos>=(showDataSize-8)) nowPos=0;
			
			EN_port = 0; //on screen
		} else {
			nowOffset++;
		}
	}
}












void UartInit(){
	#if (PARITYBIT == NONE_PARITY)
		SCON = 0x50;            //8-bit variable UART
	#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
		SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
	#elif (PARITYBIT == SPACE_PARITY)
		SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
	#endif

	TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
	TH1 = TL1 = -(FOSC/12/32/BAUD); //Set auto-reload vaule
	TR1 = 1;                //Timer1 start run
	ES = 1;                 //Enable UART interrupt
	EA = 1;                 //Open master interrupt switch

	SendString("STC12C5A60S2\r\nUart Test !\r\n");
	//SendData("");
}

/*----------------------------
UART interrupt service routine
----------------------------*/

void Uart_Isr() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
        //P0 = SBUF;          //P0 show UART data
        //bit9 = RB8;         //P2.2 show parity bit
		
		
		
		if(SBUF == 0x55){
			LED^=1;
			recIdx=0;
		} else {
			rec[recIdx] = SBUF;
			recIdx++;
			
			// if(recIdx==8){
				// TR0=0;
				// nowOffset = 0;
				// recIdx=0;
				
				// LED^=1;
			// }
		}
		
		//SendData("OK����");
		//setICDataToBuffer(nowBufferNum, getICData_ASCII_8x16(SBUF), 16, 3);
		//setICDataToBuffer(nowBufferNum, ic_data_A, 16, 1);
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        busy = 0;           //Clear transmit busy flag
    }
}

/*----------------------------
Send a byte data to UART
Input: dat (data to be sent)
Output:None
----------------------------*/
int ttt=0;
void SendData(BYTE dat)
{
    while (busy){
		ttt++;
		if(ttt>1000){
			ttt = 0;
			LED^=1;
		}
	}           //Wait for the completion of the previous data is sent
    ACC = dat;              //Calculate the even parity bit P (PSW.0)
    if (P)                  //Set the parity bit according to P
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;            //Set parity bit to 0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;            //Set parity bit to 1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;            //Set parity bit to 0
#endif
    }
    busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void SendString(char *s)
{
    while (*s)              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr
    }
}