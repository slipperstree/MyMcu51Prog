/*************************************************************************************
�����ܡ�
    ����16x64 LED������������ʾ����ָ�����֡�
	ָ������1���޸ı���showDataSize1 / 2��showDataSize1 / 2��ֵ���ϵ�Ĭ��ѭ��������ʾ��
	ָ������2�����ڷ���������ʾ
    ���ں����ֿ�����޴�16x16�ı�׼���ֵ����ֿ���Ҫ260KB������Ƭ����ROM��Ų��������ֿ⡣
    ����ʾ���⺺�־ͱ�������ⲿ����оƬ����������õ���ר�ŵĵ����ֿ�оƬ������ʵҲ��ʹ��FlashоƬ��

��оƬѡ�͡�
    ��Ƭ��
        STC89C52 ����������������ĵ�Ƭ�����ɣ�
            ROM(Flash)  : 8K�ֽ� (�����ϣ�
            RAM(xdata)  : 256�ֽ� (�����ϣ�
            IO          : 12�� (�����ϣ����˿ڶ���˵��)

    �ⲿ�ֿ�оƬ
        GT20L16S1Y ����ͨ��16x16�ֿ�оƬ��
        <���ڹ����ѹ>
          ��оƬ��Ҫ3.3v����
          ����ʹ��1117��LDO��5V��Դת��3.3v(���߲��õķ���)
          5vֱ�ӹ�������û���Թ�������Ҳ����
          IO��ƽ���棬��Ȼ��ѹ��ͬ��ʵ����Ժ�5v��Ƭ��ֱ����û������
        <����/����ʹ���ⲿ�ֿ�оƬ>
          GT20L16S1YоƬ��SOT23��װ�ıȽ�С�����˰�����ʹ���������Ǻܷ��㣬�ر��Ƕ��ڳ�ѧ�߲�̫�Ѻá�
          ��������ⲿоƬҲ����ʹ�ù̶��ĵ�����ģ�������������ſ�������ziku_table���鶨���ע�ͣ�
          ��ģ������������ͬʱ�޸ĳ������ bufHZ Ϊ ziku_table���ɡ���ص��������ա�(��س���ҲҪ������������)

������
    ʹ��12M���ϵľ��塣����ʹ��24M���ϵľ��壬����Ƶ��Խ�ߣ���ʾ�ֲ��������˸�С�
    ���ʹ��24M���µľ��壬�����STC��оƬ�������6T(˫����)���ܡ�

���˿ڶ��塿
    ������ģ��
        LATCH  - P1.6  (������������)
        CLK    - P1.7  (��������ʱ��)
        R1     - P3.5  (�����������룬R1��ʾ������������źţ�R2�º죬G1���̣�G2���̣����ߵ�ģ���ǵ���ɫ����ֻ��32���е����������Խ�R1����)
        EN     - P3.6
        A      - P1.0
        B      - P1.1
        C      - P1.2
        D      - P1.3
        R2     - P3.4  # ����������ݽӿڣ�������δʹ��
        G1     - P1.4  # �����̵����ݽӿڣ�������δʹ��
        G2     - P1.5  # �����̵����ݽӿڣ�������δʹ��
    �ֿ�оƬ GT20L16S1Y
        CE     - P2^0
        SCLK   - P2^1
        SI     - P2^2
        SO     - P2^3

�����ڶ�ʱ�������ڵ�˵����
	��ʱ��0��δʹ�ã����ȼ����ڴ����жϣ����Բ�������ɨ�裬�����Ӱ�촮��ͨ�ţ�
	��ʱ��1����ΪӲ�����ڵĲ����ʷ�����
	��ʱ��2����ɨ���Լ�������ʾ���ƣ��ж����ȼ����ڴ��ڣ�����Ӱ�촮��ͨ��
*/

/*ͷ�ļ�*/
#include "./header/common.h"
#include "./header/GT20L16S1Y.h"

#define FOSC 32000000L      // ϵͳ����Ƶ��
#define SCROLL_SPEED 100    // ���ֹ����ٶȣ�10-200��ԽСԽ�죨��Timer2�Ķ�ʱ��ֵ�޸ĵĴ�һЩ��Ч�������ԣ�����̫��Ļ���������һ�л���Խ�����

// ���������� #########################################################################################
sbit EN_port 	= P3^6;
sbit DATA_R1    = P3^5;
sbit CLK_port 	= P1^7;
sbit Latch_port = P1^6;

// ��ɫ��ɫ��δʹ�ö���
sbit DATA_R2    = P3^4;
sbit DATA_G1    = P1^4;
sbit DATA_G2    = P1^5;

#define ABCD_port P1  //HC138 ABCD�˿ڶ���
//sbit D_port = P1^3;
//sbit C_port = P1^2;
//sbit B_port = P1^1;
//sbit A_port = P1^0;

// ��Buffer
// ����16X64������Ϣ��128�ֽڣ���4�����֣�
// ��������������������������������������������������������������������������������������������������������������������
// B0	B16		B32		B48		B64		B80		B96		B112
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
uchar xdata bufHZ[128] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

data uchar nowShiftOffset=0; 	// ��ǰ����ƫ����(ȫ�� 0-15/ ��� 0-7)
idata int nowShowTextPos=0;		// ��ǰ��ʾ�ڼ����֣��ϸ���˵�ǵڼ����ֽڣ�

// һ���Դ���HC595�õ�64+16λ��һ�������ݣ�������λ�õĵ�������ֵĻ�������
data uchar row_data_buf[10];
void HC595_Data_Send(uchar *p, uchar offset);

// ���ƺ��� ��������ICȡ���ұߵ�һ�����ݣ������buffer��ͬʱ���µ�ǰƫ��λ�ã�
void shiftLeft(void);
data int ttShiftLeft = 0;

data uchar nowText=0;
data int showDataSize=0;
idata uchar* textForShow;
// ��ʾ�������ã�ע�ⳤ�Ⱥ���������Ҫƥ��
int idata showDataSize2=972*2;
uchar code textForShow2[] = "�����޸衷���ơ��׾��ס�������ɫ˼�������������󲻵á������Ů�����ɣ����������δʶ������������������һ��ѡ�ھ����ࡣ����һЦ��������������������ɫ��������ԡ����أ���Ȫˮ��ϴ��֬���̶�����������ʼ���³ж���ʱ�����޻��ս�ҡ��ܽ����ů�ȴ�������������ո��𣬴Ӵ˾������糯���л���������Ͼ�����Ӵ���ҹרҹ���󹬼�����ǧ�ˣ���ǧ�谮��һ������ױ�ɽ���ҹ����¥�����ʹ�����õ��ֽ�����������������Ż����������¸�ĸ�ģ�������������Ů���깬�ߴ������ƣ����ַ�Ʈ�����š�����������˿�񣬾��վ��������㡣����ܱ�Ķ��������������������������س����̳�����ǧ�����������С��仪ҡҡ�и�ֹ���������Ű���������������κΣ���ת��ü��ǰ��������ί�������գ����̽�ȸ��ɦͷ����������Ȳ��ã��ؿ�Ѫ����������ư�ɢ������������ջ�����ǽ��󡣶���ɽ�������У�����޹���ɫ������ˮ����ɽ�࣬ʥ������ĺĺ�顣�й���������ɫ��ҹ�����峦������������ת����Ԧ�����˳������ȥ���������������У��������տ�������������˾�մ�£�������������顣������Է�����ɣ�̫Һܽ��δ������ܽ����������ü���Դ���β��ᴹ�����������ҹ��������ͩҶ��ʱ���������ڶ���ݣ���Ҷ���׺첻ɨ����԰���Ӱ׷��£�������������ϡ�Ϧ��ө��˼��Ȼ���µ�����δ���ߡ��ٳ��ӹĳ���ҹ�������Ǻ������졣ԧ������˪���أ��������˭�빲�������������꣬���ǲ��������Ρ�������ʿ�趼�ͣ����Ծ����»��ǡ�Ϊ�о���չת˼����̷�ʿ�����١��ſ�Ԧ������磬���������֮�顣��������»�Ȫ������ãã�Բ��������ź�������ɽ��ɽ����������䡣¥���������������д�Լ�����ӡ�����һ����̫�棬ѩ����ò�β��ǡ���������ߵ���磬ת��С��˫�ɡ��ŵ���������ʹ���Ż������λ꾪�������������ǻأ��鲭�������ƿ������ް�ƫ��˯�������ڲ������������紵����Ʈҡ�٣��������������衣���ݼ�į�����ɣ��滨һ֦�����ꡣ��������л������һ����������ã����������������������������³�����ͷ������徴�����������������Ψ����������飬��Ͻ��μĽ�ȥ������һ�ɺ�һ�ȣ���뢻ƽ�Ϸ��䡣�������ƽ���ᣬ�����˼��������ٱ������ؼĴʣ�������������֪���������ճ����ҹ������˽��ʱ������Ը���������ڵ�ԸΪ����֦���쳤�ؾ���ʱ�����˺������޾��ڡ�";
int idata showDataSize1=722*2;
uchar code textForShow1[] = "�������С����ơ��׾��ס������ͷҹ�Ϳͣ���Ҷݶ����ɪɪ������������ڴ����پ������޹��ҡ����ɻ��ҽ��𣬱�ʱãã�����¡�����ˮ������������������Ͳ�����Ѱ�����ʵ���˭��������ͣ����١��ƴ�������������ƻص��ؿ��硣ǧ����ʼ�������̱����ð����档ת�Ღ����������δ�����������顣������������˼������ƽ������־����ü������������˵�����������¡���£����Ĩ��������Ϊ�����ѡ������ۡ������������缱�꣬С��������˽��������д��ӵ�������С�������̡����ݺ�ﻨ�׻�������Ȫ�������ѡ���Ȫ��ɬ��������������ͨ����Ъ�������ĳ��������ʱ����ʤ��������ƿէ��ˮ���ţ�����ͻ����ǹ���������ղ����Ļ�������һ�����Ѳ����������������ԣ�Ψ���������°ס������Ų������У��������������ݡ����Ա��Ǿ���Ů������Ϻ�����ס��ʮ��ѧ�����óɣ������̷���һ�������������Ʋŷ���ױ��ÿ������ʡ�������������ͷ��һ����篲�֪������ͷ���������飬Ѫɫ��ȹ�����ۡ����껶Ц�����꣬���´�����жȡ����ߴӾ���������ĺȥ������ɫ�ʡ���ǰ���䰰��ϡ���ϴ�������˸���������������룬ǰ�¸������ȥ��ȥ�������ؿմ����ƴ�������ˮ����ҹ����������£�����ױ������ɡ�����������̾Ϣ�����Ŵ���������ͬ�����������ˣ����α�����ʶ���Ҵ�ȥ��ǵ۾����ؾ��Բ�����ǡ������Ƨ�����֣����겻��˿������ס���Խ��ص�ʪ����«������լ������䵩ĺ�ź���ž���ѪԳ������������������ҹ������ȡ�ƻ����㡣����ɽ�����ѣ�Ż�Ƴ�����Ϊ������ҹ�ž�������������ֶ�������Ī�Ǹ�����һ����Ϊ�������������С������Ҵ�����������ȴ��������ת�������಻����ǰ�����������Ž���������������˭��ࣿ����˾������ʪ��";

//int showDataSize=21;
//uchar code textForShow1[] = "����1ȫ2��3��ǻ�aBc��";
//int showDataSize=32;
//uchar code textForShow2[] = "!"#$%&'()=~|?^-@`[]{};:+*,.<>/_?";

// ������PWM������������ -----------------------------------------------------------------------------------
// ԭ���ǣ�PWM����û�г��� pwmBright ����ֵʱ��Ŀ���������ͨ���Ҳ���ǵ�����
// ���������� pwmBright ʱ��ϵ�ر�Ŀ�������
// �������ﵽPWM�ܿ�� PWM_WIDTH_ALL ʱ����ǰ��Ŀ�������һ��PWM Cycle�������л�����һֻĿ������ܣ��ܶ���ʼ
// ����������������ڵ�λʱ����ͨ���ʱ����� pwmBright/PWM_WIDTH_ALL �� �Ӷ���������
// Ҳ���ǵ�pwmBrightΪ1ʱ�����ΪPWM_WIDTH_ALLʱ������

// PWM����ܳ������ֵԽ��PWM�ľ��Ⱦ�Խ�ߣ������õ�������С��λ��Խ��ϸ
// ��ֵԽ������ܵ�ˢ��Ƶ��ҲԽ�ͣ�̫�����ֵ�ᵼ���������ʾ��˸
#define PWM_WIDTH_ALL 20

// // ����ģʽ
// static enum EnumBreathMode breathMode = DISPLAY_BREATH_MODE_OFF;
// static enum EnumSpeed breathSpeed = DISPLAY_SPEED_LV_6;
// #define BRTH_BRIT_ARR_SIZE 32
// static uchar code breathBrightArray[] = {
//     1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1
// };
// static_idata_uchar nowBreathBright = 0;

// PWM�������ã������Գ��� PWM_WIDTH_ALL�������޸�
idata uchar pwmBright = 1;

// PWM�����ã������޸�
idata uchar ttPWM = 0;

// ������PWM������������ -----------------------------------------------------------------------------------


typedef unsigned char BYTE;
typedef unsigned int WORD;

#define BAUD 9600           //UART baudrate

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

bit busy;

// ���ڽ������ֻ�����
idata uchar serialRcvBuf[50]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
idata uchar serialRcvIdx=0;

// �������� ######################################################################################
//test
void testSetFullScreenByte(uchar);
//test

// оƬȡ��������ת�����ʺ�ģ������ݸ�ʽ�������ڴ�buffer��
void setICDataToBuffer(unsigned char *pICData, unsigned char size, unsigned char pos);

// ��Ȼ������Timer0����main����û�����ã�Ŀǰ���ò������Ժ������Ҫ��������
idata int ttTimer0 = 0;
void Timer0Init(void)		//30us@32.000MHz@6T
{
	EA=0;

	TMOD &= 0xF0;	//���ö�ʱ��ģʽ
	TMOD |= 0x02;	//���ö�ʱ��ģʽ��8λ�Զ����أ�
	TL0 = 0xb0;		//���ö�ʱ��ֵ(30us@6T@@32.000MHz)
	TH0 = 0xb0;		//���ö�ʱ��ֵ(30us@6T@32.000MHz)
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
    ET0 = 1;

	EA=1;
}

// ��Ȼ������Timer0����main����û�����ã�Ŀǰ���ò������Ժ������Ҫ��������
void Timer2Init(void)
{
	// 89ϵ�ж�ʱ��2ֻ��16λ�Զ�����ģʽ
	T2MOD = 0;				//��ʼ��ģʽ�Ĵ���
	T2CON = 0;				//��ʼ�����ƼĴ���
	RCAP2L = TL2 = 0x2B;	//���ö�ʱ��ֵ������ֵ(1ms@32M@6T)
	RCAP2H = TH2 = 0xFB;	//����ֵ��EB�ȽϺ��ʻ������ȶ�����FB������Ѹ�١����ö�ʱ��ֵ������ֵ(1ms@32M@6T)
	TR2 = 1;				//��ʱ��2��ʼ��ʱ
    ET2 = 1;                //enable timer2 interrupt
    EA = 1;                 //open global interrupt switch
}

// ��ɨ�躯����ÿ����һ��ɨ��һ�У�
// Ϊʲô16�е�ɨ�費���ں�������һ��������������Ϊϣ����ɨ��֮�����п�϶��������Ĺ���
// ���ڶ�ʱ��0���жϺ����ж�ʱ���̣ܶ�ִ��
// �ٶ�Խ�컭��Խ����˸����ÿ�β���ʱ�䲻���Գ�����ʱ���
// ��ȡ�ֿ⣬������ʾ���ݵĴ�������ڶ�ʱ�������ɣ�����ﲻ����ô�죬����Ҫ��һ��16�����ݸպ�ɨ����ʱִ�У���������ȥ������ȶ���ÿ����ͬһ���ط�������������������˸����
idata uchar rowIdx=0; // ��ǰɨ��ڼ���
void display(){
	idata uchar i;

	ttPWM++;

	// ÿ���һ��PWM��������Ⱦ���ʾ��һ��
	if (ttPWM >= PWM_WIDTH_ALL){
		ttPWM = 0;
		rowIdx++;
		if (rowIdx>=16)
		{
			rowIdx=0;
		}
		// ѭ���Ӵ�buffer��ȡ��ǰ�д����ҵ�ȫ��8���ֽ���Ϊһ�������ݱ�������ʾ�õ�buff��
		for (i=0;i<10;i++){
			row_data_buf[i]=bufHZ[i*16 + rowIdx];
		}
		// ������ȡ����һ�������ݴ��������74HC595���У�������ǰ�е�����
		HC595_Data_Send(row_data_buf,nowShiftOffset);

		// HC138����ѡ��˿�ָ����һ��
		ABCD_port = (ABCD_port & 0xf0)|rowIdx;
	} 

	// PWM���ȿ���
	if (ttPWM <= pwmBright)
	{
		/*HC138�����Ч������ʾ*/
		EN_port = 0;
	} else {
		/*HC138�����Ч���ر���ʾ*/
		EN_port = 1;
	}
}

// һ�����ڽ��ܵ����֣���ֹͣ������ʾĬ�����֣�ת����ʾ��������ģʽ
// ����ģʽ��ʱ��֧�ֹ���
bit isShowSerialData = 0;
void main()
{
	nowText = 0;
	showDataSize = showDataSize1;
	textForShow = textForShow1;

	// ÿ�δ����Ҷˣ�nowShowTextPos+8�ĺ��֣���ʼ���ƽ���
	nowShowTextPos = -8;

	UartInit(); //ʹ��Timer1��Ϊ�����ʷ�����
//	Timer0Init();
    //Timer2Init();
	
	// For test --------------------------------------------------------------------------------
	//readICDataToBuffer( getICData_Hanzi_16x16("ȫ"), 32, 0);
//	readICDataToBuffer(textForShow);
	//readICDataToBuffer("�����ʻ�");
	
	//setICDataToBuffer(getICData_ASCII_8x16('A'), 16, 0);
	//setICDataToBuffer(ic_data_A, 16, 0);
	// For test --------------------------------------------------------------------------------
    while(1){
		//display();
		//testSetFullScreenByte(0xff);

		//���ڽ��յ��ı���ʾ�ڻ����� ����Ҫ�޸�
		//ÿ�յ�8���ֽڣ�һ�����֣���ˢ�»���
		if(serialRcvIdx >= 8){
			//TR2=0; //��ͣɨ�費Ȼ����Դ��ͻ������ú���
			// �л�������������ʾģʽ��ֹͣ����
			isShowSerialData=1;
			// ȡ�������ֵĵ���
			//readICDataToBuffer("�����ʻ�");
			readICDataToBuffer(serialRcvBuf);

			serialRcvIdx=0;
			//TR2=1; //�ָ�ɨ��
		}

		// ��ɨ��----------------
		// ÿɨ��һ�Σ���ǰ�е�pwm cycle�ͻ�ˢһ֡��һֱ��PWM MAX WIDTH �ͻ����ˢ��һ��
		display();

		// ����----------------
		// �����ǰ����ʾ��������ģʽ�򲻽�������
		if (isShowSerialData != 1) {
			
			ttShiftLeft++;
			// ����ɨ��һ֡����ʱ�����ҹ���ʱ��������
			// ��������
			// �ж�rowIdx=15�������ǣ����ƴ���ֻ�����ڻ���������һ֡ɨ�����Ժ�ſ�����
			// �������Ա��⻭���λ
			// Ҫ��֤ÿ����ɨ��֮����ǵ�ʱ���㹻ִ�������Ʋ���
			if ( rowIdx == 15 && ttShiftLeft >= SCROLL_SPEED) {
				ttShiftLeft = 0;
				// ����1λ
				delay_ms(pwmBright);
				EN_port = 1; //off screen
				shiftLeft();
				EN_port = 0; //on screen
			}
		}
	}
}

/*--------------------------------------------------------------
    ����LED�ڼ�����ʾ����
   ����*pΪ����ĵ������ݣ�rowΪ�ڼ�����ʾ��zishuΪ��ʾ���ٸ���
---------------------------------------------------------------*/
void HC595_Data_Send(uchar *p, uchar offset)
{
	idata uchar i=0;
	idata uchar temp=0;
	idata uchar* data_buff;

	/*��ÿһλ����595����λ�Ĵ���*/
	for(temp=0;temp<8;temp++){
		data_buff = &p[temp];
		for(i=0;i<8;i++){
			if(((data_buff[0]<<i)&0x80)!=0) DATA_R1 = 0;
			else DATA_R1 = 1;
			CLK_port = 1;
			CLK_port = 0;
		}
	}

	/*HC595�����½��أ��������*/
	Latch_port = 1; /*����HC595���������Q1-Q8�˿�*/
	Latch_port = 0;	/*����HC595�������*/
}

// ��ǰδʹ�ø��жϺ���
void Timer0() interrupt 1
{
	// 30usһ��
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
	TH1 = TL1 = -(FOSC/6/32/BAUD); //Set auto-reload vaule(FOSC/6�����6��ʾ����ʱָ����6Tģʽ�������Ĭ��12T��ĳ�12)
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

SBUF=SBUF;
		if(SBUF == 0x55){
			serialRcvIdx=0;
		} else {
			serialRcvBuf[serialRcvIdx] = SBUF;
			serialRcvIdx++;
		}
		TR0=1;
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
void SendData(BYTE dat)
{
    while (busy){
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


void Timer2() interrupt 5
{
	// 1msһ�� @ 32M @ 6T
	// �Ӿ��ϸо�����˸Ҫ������2ms����ɨ��һ��
	// 16�о���30ms��һ����Ҳ����30֡������������˸�ˡ�
	// ��Ҳ����̫�죬��Ϊ̫��Ļ����̫�̲���ȡ���ݸ������ݵ�ʱ�䣬Ҳ�ᵼ����˸

	// ��ʱ��2��TF2��־λ�����ֶ�����
	TF2 = 0;

	// ��ɨ��----------------
	// ÿɨ��һ�Σ�ȫ�ֱ���rowIdx�Լ�һ��һֱ��15�ٻص�0
	display();

	// �����ǰ����ʾ��������ģʽ�򲻽�������
	if (isShowSerialData == 1) {
		return;
	}

	// ����----------------
	ttShiftLeft++;
	// ����ɨ��һ֡����ʱ�����ҹ���ʱ��������
	// ��������
	// �ж�rowIdx=0�������ǣ����ƴ���ֻ�����ڻ���������һ֡ɨ�����Ժ�ſ�����
	// �������Ա��⻭����˸����Ϊ���ƴ�������Ҫ��ȡ�ֿ�оƬ�Ⱥ�ʱ�Ĳ���
	// ��������ϻ���ɨ�裬��������᲻������˸
	// ���������ÿ����ɨ��֮����ǵ�ʱ���㹻ִ�������Ʋ���
	// Ҳ�������ƻ��ѵ�ʱ��Ҫ�������ڵ�ǰ��ʱ��0���жϼ��
	if ( rowIdx == 0 && ttShiftLeft >= SCROLL_SPEED) {
		ttShiftLeft = 0;
		// ����1λ
		//EN_port = 1; //off screen
		shiftLeft();
		//EN_port = 0; //on screen
	}
}

// ��������һλ
void shiftLeft() {

	uchar idx = 0;

	// ��ICȡ���ҲཫҪ����������ݣ����¸�һ���ֽڣ�
	// ��һ���ֽ�����������Ascii�뻹�Ǻ��֣��Ա���λoffset������
	uchar shiftInColData[3] = {0x00,0x00,0x00};
	getICData_Col(&textForShow[nowShowTextPos+8], nowShiftOffset, shiftInColData);

	// ��Bufferȫ������һλ
	for (idx=0; idx<128; idx++){
		// ������1λ
		bufHZ[idx]<<=1;
		// ���λ�������Ҳ����ݵ����λ���
		bufHZ[idx] |= ((bufHZ[idx+16]>>7)&0x01);
	}

	// ȡ�����������������ֽ�()�����ݵ�ÿһλд���buffer���Ҳ�
	// Ҳ����д��B112-B127��16���ֽڵ����λ
	// <��Buff���Ҳ�����>
	// B112   ...bit0 <- shiftInColData[1]�� bit0
	// ...
	// B119   ...bit0 <- shiftInColData[1]�� bit7
	// B120   ...bit0 <- shiftInColData[2]�� bit0
	// ...
	// B127   ...bit0 <- shiftInColData[2]�� bit7
	for(idx = 0; idx < 8; idx++)
	{
		bufHZ[idx+112] &= 0xFE; // 112-119 �������λ
		bufHZ[idx+120] &= 0xFE; // 120-127 �������λ
		
		if ((shiftInColData[1]<<(7-idx) & 0x80) == 0x80 )  {
			bufHZ[idx+112] |= 0x01; // 112-119 ���λ��1
		}

		if ((shiftInColData[2]<<(7-idx) & 0x80) == 0x80 )  {
			bufHZ[idx+120] |= 0x01; // 120-127 ���λ��1
		}
	}

	// ƫ������һ��֪ͨ��ʾ�Ӻ���ˢ�»���
	if(shiftInColData[0] == 0){
		// ��ǰ�ƽ������ַ���Ascii��8λ���
		if(nowShiftOffset==7){
			nowShiftOffset=0;
			nowShowTextPos+=1;  // ȫ���ƶ���ɺ󣬵�ǰ��ʾ����λ������ƶ�1���ֽڣ�һ��Ascii��1���ֽڣ�
			if(nowShowTextPos>=(showDataSize-8))
			{
				nowShowTextPos=-8;

				// �л��������ֵ���ʾ
				if(nowText==0){
					nowText = 1;
					showDataSize = showDataSize2;
					textForShow = textForShow2;
				}else{
					nowText = 0;
					showDataSize = showDataSize1;
					textForShow = textForShow1;
				}
				
			}
		} else {
			nowShiftOffset++;
		}
	} else {
		// ��ǰ�ƽ������ַ��Ǻ���16λ���
		if(nowShiftOffset==15){
			nowShiftOffset=0;
			nowShowTextPos+=2;  // ȫ���ƶ���ɺ󣬵�ǰ��ʾ����λ������ƶ�2���ֽڣ�һ�����ּ�2���ֽڣ�
			if(nowShowTextPos>=(showDataSize-8))
			{
				nowShowTextPos=-8;

				// �л��������ֵ���ʾ
				if(nowText==0){
					nowText = 1;
					showDataSize = showDataSize2;
					textForShow = textForShow2;
				}else{
					nowText = 0;
					showDataSize = showDataSize1;
					textForShow = textForShow1;
				}
				
			}
		} else {
			nowShiftOffset++;
		}
	}
	
}

// �����յ���IC�������ݴ��뻺����������ʾ
// pICData	- ���յ��ĵ�������
// size		- ��������Ĵ�С��ͨ������Ϊ32�ֽڣ����ASCII��Ϊ16�ֽڡ�
// pos		- ָ���õ���������ʾ�Ŀ�ʼλ��(0-7)����8λ���У�Ϊһ����λ���Ḳ��ԭ�������ݡ�
void setICDataToBuffer(unsigned char *pICData, unsigned char size, unsigned char pos)
{
	// ############# ���ֿ�оƬGT20L16S1Yȡ���ĵ�����Ϣ�����ú���ģʽ��������ĸA�� ############# 
	// ���ֿ�оƬGT20L16S1Yȡ���ĵ�����Ϣ�����ú���ģʽ��Ҳ����������Ϊ��λȡ��������
	// ---- �����оƬȡ������ĸA�ĵ������� ���ú��ŵģ������Ǹ�λ�����棬��λ�����棩 ---- 
	//       B0	B1	B2	B3	B4	B5	B6	B7
	// bit0  0	0	0	0	0	0	0	0
	// bit1  0	0	0	��	0	0	0	0
	// bit2  0	0	��	0	��	0	0	0
	// bit3  0	0	��	0	��	0	0	0
	// bit4  0	0	��	0	��	0	0	0
	// bit5  0	��	0	0	0	��	0	0
	// bit6  0	��	0	0	0	��	0	0
	// bit7  0	��	��	��	��	��	0	0
	//       B8	B9	B10	B11	B12	B13	B14	B15
	// bit0  ��	0	0	0	0	0	��	0
	// bit1  ��	0	0	0	0	0	��	0
	// bit2  ��	0	0	0	0	0	��	0
	// bit3  ��	0	0	0	0	0	��	0
	// bit4  0	0	0	0	0	0	0	0
	// bit5  0	0	0	0	0	0	0	0
	// bit6  0	0	0	0	0	0	0	0
	// bit7  0	0	0	0	0	0	0	0
	// ����ͷ������������������ʽ����������Ҳ����ÿ��ɨ��ʱ��Ҫ����һ���еĵ�����Ϣ
	// ������Ҫ��ÿ��ȡ���ĵ�����Ϣת�浽�������ţ�Ҳ���Ǻ���ȡģ����еġ�����ʽ������ʽ��buf��ȥ��
	// ��ʾʱ��buf��ȡ������һ���е�8���ֽڵ����ݴ��д���595����ɨ����ʾ
	// ����ŵ��������������ڵ���ʾλ���й�ϵ���������Ǵ��㽫�������A�ĵ�����ʾ����Ļ�������
	// ��ô��ת���Ժ�Ӧ��������������ӣ��Ҳ������ʡ�ԣ�
	// Ҳ����˵Ҫ��ȡ����B0-B7�����λ��ϳ�buf���һ���ֽ�B0���ַ�AΪ������0x00��
	// ��ȡ����B0-B7�Ĵθ�λ��ϳ�buf����ֽ�B1���ַ�AΪ������0x10��
	//     bit7 ---------------------- bit0
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
	//memset(&bufHZ[pos*16], size, 0x00);
	
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
				pTmpByteBufHZ = &(bufHZ[pos*16 + bufByteIdx]);
				//display();//test
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
					
				pTmpByteBufHZ = &(bufHZ[pos*16 + bufByteIdx]);
				
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

// test---------------------------------------------------------------
// ��������Ļ�����������ó�ͬһ������(�������0xFF����ȫ������)
// ���������ڲ���LED���ӣ���Ҫ����������whileѭ���вſ��ԣ�ģ����Ҫ��ͣɨ�������ʾ��
void testSetFullScreenByte(uchar bData)
{
	uchar row=0;
	uchar i=0;
	uchar temp=0;
	Latch_port = 0;  /*HC595�������,�������ݴ�������У������ݱ仯�Ӷ���ʾ��˸*/
	CLK_port = 0;
	
	for(row = 0; row < 16; row++)
	{
		/*������ʾ��ԭ��ΪʹHC138���ȫΪ1���Ӷ������ܽ�ֹ��������ʾ*/
		EN_port = 1;

		// ��ÿһλ����595����λ�Ĵ���
		// SM16106��LE�����棩����Ϊ�͵�ƽ����������
		Latch_port = 0;

		for(temp=0;temp<8;temp++){
			for(i=0;i<8;i++){
				CLK_port = 0;

				if(((bData<<i)&0x80)!=0) DATA_R1 = 0;
				else DATA_R1 = 1;
				
				CLK_port = 1;
			}
		}

		/*��������*/
		Latch_port = 1;

		//����138��ָ���е�����
		ABCD_port = (ABCD_port & 0xf0)|row;

		/*HC138�����Ч������ʾ*/
		EN_port = 0;
		
	}
}