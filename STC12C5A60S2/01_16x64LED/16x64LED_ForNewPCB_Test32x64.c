#include <intrins.h>
#include <STC89C52.h>
#define uint unsigned int
#define uchar unsigned char

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
sbit D_port = P1^3; //-> HC138 E1 �͵�ƽʹ��
sbit C_port = P1^2;
sbit B_port = P1^1;
sbit A_port = P1^0;

void testSetFullScreenByte(uchar);

void delay_ms(uchar n)
{
	uchar x,y;
	for(x=n;x>0;x--)
	{
		for(y=5;y>0;y--);	
	}
}

int ttTimer2 = 0;
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

uchar rowIdx=0; // ��ǰɨ��ڼ���
void main()
{DATA_G1=0;
    //Timer2Init();
	//testSetFullScreenByte(0xFE);
    while(1){
		testSetFullScreenByte(0x55);
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
	
}

// test---------------------------------------------------------------
// ��������Ļ�����������ó�ͬһ������(�������0xFF����ȫ������)
// ���������ڲ���LED���ӣ���Ҫ����������whileѭ���вſ��ԣ�ģ����Ҫ��ͣɨ�������ʾ��
uchar nowBright=0;
uchar maxBright=100;
uchar setBright=1;
uchar row=0;

uchar flg=0;
void testSetFullScreenByte(uchar bData)
{
	uchar i=0;
	uchar temp=0;

	nowBright++;
	if (flg == 0 && nowBright >= maxBright) {
		//flg = 1;
		nowBright = 0;

		// ��ÿһλ����595����λ�Ĵ���
		// SM16106��LE�����棩����Ϊ�͵�ƽ����������
		Latch_port = 0;
		
		for(temp=0;temp<8;temp++){

			for(i=0;i<8;i++){
				CLK_port = 0;

				if(((bData<<i)&0x80)!=0) {
					DATA_R1 = 0;
				} else {
					DATA_R1 = 1;
				}

				// ��������������
				CLK_port = 1;
			}
		}

		/*SM16106��LE�����棩����Ϊ�ߵ�ƽ����������*/
		Latch_port = 1; 
		

		//����138��ָ���е�����
		 ABCD_port = (ABCD_port & 0xf0)|row;
		// D_port = 1;
		// A_port = 1;
		// B_port = 1;
		// C_port = 1;

		row++;
		if(row == 16) row = 0;
	}
	
	if (nowBright < setBright) {
		/*HC138�����Ч������ʾ*/
		EN_port = 0;
	} else {
		/*HC138�����Ч���ر���ʾ*/
		EN_port = 1;
	}

}