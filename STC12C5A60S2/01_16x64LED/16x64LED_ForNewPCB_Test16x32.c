#include <intrins.h>
#include <STC89C52.h>
#define uint unsigned int
#define uchar unsigned char

// LED�� ��ɫ 16x32 ��ߴ� #########################################################################################
// �ӿ� ������08�ӿڣ���Ҫ��2x8��ֱ�����Լ����İ����ϣ�Ҫ�öŰ��ߵ������ߣ�
//     OE    �� �ߵ�ƽʹ�ܣ������Ͻ���138��E3��, E1,E2�Ѿ�ֱ�ӽӵ�ʹ���ˣ�
//     N     �� �ӵ� (7��������ȫ�ӣ��ڲ��Ѿ�����һ����)
//     A/B   �� ��ѡ�������Ͻ���138��A0,A1�ϣ�
//     C     �� ������138��A2ֱ�ӽӵ���??��
//     S     �� SCK ��������λʱ�ӣ������Ͻ���595��CLK�ϣ�
//     L     �� LAT ���棨�����Ͻ���595��LE�ϣ�
//     R     �� DATA IO ���ݿڣ������Ͻ���595��LE�ϣ�
//     G     �� ��ɫ���ã�������ʹ�ã�
//     D     �� δ֪��������ʹ�ã�

// ����Ӳ�����ӣ�138�����Ϊ�±���A2�̶��ӵأ�
// ------------------------------------
// A2 A1 A0 | Y0 Y1 Y2 Y3 Y4 Y5 Y6 Y7
// ------------------------------------
// L  L  L  | L  H  H  H  H  H  H  H
// L  L  H  | H  L  H  H  H  H  H  H
// L  H  L  | H  H  L  H  H  H  H  H
// L  H  H  | H  H  H  L  H  H  H  H
// ------------------------------------

// ================================================================================================
// Ӳ��ʾ��ͼ(�ӱ���PCB�ķ��򿴵ģ�����������LED�ķ���)
// HC595������(16�ֽڣ�128λ)
// HC138������(4��)
// ================================================================================================
//                    00 01 ... 07  .--32 33 ... 39  .--XX XX ... XX  .--XX XX ... XX
// �� 01 �У�    Y0 ->           |   |            |   |            |   |            |    
// �� 02 �У�    Y1 ->           |   |            |   |            |   |            |    
// �� 03 �У�    Y2 ->           |   |            |   |            |   |            |    
// �� 04 �У�    Y3 ->           |   |            |   |            |   |            |    
//                    .---------`   |  .---------`   |  .---------`   |  .---------`    
//                    |             |  |             |  |             |  |              
//                    08 09 ... 15  |  40 41 ... XX  |  XX XX ... XX  |  XX XX ... XX
// �� 05 �У�    Y0 ->           |   |            |   |            |   |            |    
// �� 06 �У�    Y1 ->           |   |            |   |            |   |            |    
// �� 07 �У�    Y2 ->           |   |            |   |            |   |            |    
// �� 08 �У�    Y3 ->           |   |            |   |            |   |            |    
//                    .---------`   |  .---------`   |  .---------`   |  .---------`    
//                    |             |  |             |  |             |  |              
//                    16 17 ... 23  |  XX XX ... XX  |  XX XX ... XX  |  XX XX ... XX
// �� 09 �У�    Y0 ->           |   |            |   |            |   |            |    
// �� 10 �У�    Y1 ->           |   |            |   |            |   |            |    
// �� 11 �У�    Y2 ->           |   |            |   |            |   |            |    
// �� 12 �У�    Y3 ->           |   |            |   |            |   |            |    
//                    .---------`   |  .---------`   |  .---------`   |  .---------`    
//                    |             |  |             |  |             |  |              
//                    24 25 ... 31--`  XX XX ... XX--`  XX XX ... XX--`  XX XX ... XX
// �� 13 �У�    Y0 -> 
// �� 14 �У�    Y1 -> 
// �� 15 �У�    Y2 -> 
// �� 16 �У�    Y3 -> 
// ================================================================================================
// Ӳ������ʾ��ͼ������ʾ���������ݵķ�����Ҫ��1/4��ɨ�裺
// ɨ��1/4
//   HC595 �������� 16���ֽ� ������ ����01�У���05�У���09�У���13�У�
//   HC138 ��ѡ��Y0���
// ɨ��2/4
//   HC595 �������� 16���ֽ� ������ ����02�У���06�У���10�У���14�У�
//   HC138 ��ѡ��Y1���
// ɨ��3/4
//   HC595 �������� 16���ֽ� ������ ����03�У���07�У���11�У���15�У�
//   HC138 ��ѡ��Y2���
// ɨ��4/4
//   HC595 �������� 16���ֽ� ������ ����04�У���08�У���12�У���16�У�
//   HC138 ��ѡ��Y3���
// λ��595���ݵ�ʱ��Ҫע�����ݵ��Ⱥ�˳���ȴ���������ʾ��������


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

uint ttTimes = 0;
void main()
{
	uchar dat=0;
	/*HC138�����Ч���ر���ʾ*/
	EN_port = 0;
	//testSetFullScreenByte(0xFE);
    while(1){
		ttTimes++;
		if (ttTimes >= 5000) {
			dat++;
			ttTimes = 0;
		}
		testSetFullScreenByte(dat);
	}
}

// test---------------------------------------------------------------
// ��������Ļ�����������ó�ͬһ������(�������0xFF����ȫ������)
// ���������ڲ���LED���ӣ���Ҫ����������whileѭ���вſ��ԣ�ģ����Ҫ��ͣɨ�������ʾ��
uchar nowBright=0;
uchar maxBright=250;
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
		
		// ÿɨ��һ������16���ֽڵ�����
		for(temp=0;temp<16;temp++){

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
		if(row == 4) row = 0;
	}
	
	if (nowBright < setBright) {
		/*HC138�����Ч������ʾ*/
		EN_port = 1;
	} else {
		/*HC138�����Ч���ر���ʾ*/
		EN_port = 0;
	}

}