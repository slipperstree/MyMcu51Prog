
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-755-82944243 ----------------------------------------*/
/* --- Tel: 86-755-82948412 ----------------------------------------*/
/* --- Web: www.STCMCU.com -----------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


/*************	����˵��	**************

������ճ����������г�����������HT6121/6122�������IC�ı��롣

�����û�����User_code���岻ͬ��ң����������Ὣ�û���һ��Ӵ��������

ʹ��ģ�⴮�ڷ��ͼ����ʾ���룬��ʾ����ΪASCII������ġ�

�����ճ������״̬���ķ�ʽ��ռ��CPU��ʱ��ǳ��١�

HEX�ļ��ڱ�Ŀ¼��/list���档

******************************************/


/*************	�û�ϵͳ����	**************/
// ����׷��ע�ͣ�����
// ע�⣬���ʹ��STC15F104W���������Ȼ��12M������¼ʱ��Ҫָ���ڲ�RCƵ��Ϊ11.0592������ʱ�򲻶ԣ�����Ҳ������
//       ���ʹ��STC15L104Eϵ����RCƵ��ָ��Ϊ12Mû���⡣
// ��������ΪWϵ��ʵ���ٶȸ��죿������̫���������

#define MAIN_Fosc		12000000L	//������ʱ��, ģ�⴮�ںͺ�����ջ��Զ���Ӧ��5~36MHZ

#define D_TIMER0		125			//ѡ��ʱ��ʱ��, us, �������Ҫ����60us~250us֮��

#define	User_code		0xFF00		//�����������û���


/*************	���º궨���û������޸�	**************/
//#include	"reg51.H"
#include	"STC15F104W.h"
#define	uchar	unsigned char
#define uint	unsigned int

#define freq_base			(MAIN_Fosc / 1200)
#define Timer0_Reload		(65536 - (D_TIMER0 * freq_base / 10000))




/*************	���س�������	**************/



/*************	���ر�������	**************/
sbit	P_TXD1 = P3^1;		//����ģ�⴮�ڷ��ͽţ���ӡ��Ϣ��
sbit	P_IR_RX = P3^2;		//��������������˿�

sbit	P_LED = P3^4;		//����LED

bit		P_IR_RX_temp;		//Last sample
bit		B_IR_Sync;			//���յ�ͬ����־
uchar	IR_SampleCnt;		//��������
uchar	IR_BitCnt;			//����λ��
uchar	IR_UserH;			//�û���(��ַ)���ֽ�
uchar	IR_UserL;			//�û���(��ַ)���ֽ�
uchar	IR_data;			//����ԭ��
uchar	IR_DataShit;		//���ݷ���

bit		B_IrUserErr;		//User code error flag
bit		B_IR_Press;			//Key press flag,include repeat key.
uchar	IR_code;			//IR code	�������

// add by chenling 4 sleepTimer
uchar   sleepMin;
uchar   tt10ms;
uchar 	tt1s;
uchar 	tt1m;

/*************	���غ�������	**************/
void	Tx1Send(uchar dat);
uchar	HEX2ASCII(uchar dat);
void	InitTimer(void);
void	PrintString(unsigned char code *puts);

void	InitTimer2(void);
void	DoWork(uchar keyCode);
void	SetSleepTimer(uchar min);
void	ClrSleepTimer();


/*************  �ⲿ�����ͱ������� *****************/



/********************* ������ *************************/
void main(void)
{
	InitTimer();		//��ʼ��Timer
	
	InitTimer2();		//��ʼ��Timer2��δ��������ʱ�������²�����
	
	PrintString("****** STCϵ��MCU������ճ��� 2010-12-10 ******\r\n");	//�ϵ�󴮿ڷ���һ����ʾ��Ϣ

	while(1)
	{
		if(B_IR_Press)		//��IR������
		{
			// ���ͼ���
			PrintString("������: 0x");			//��ʾ�������
			Tx1Send(HEX2ASCII(IR_code >> 4));	//����߰��ֽ�
			Tx1Send(HEX2ASCII(IR_code));		//����Ͱ��ֽ�

			// �����û���
			Tx1Send(' ');					//���ո�
			Tx1Send(' ');					//���ո�
			PrintString("�û���: 0x");		//��ʾ�û���
			Tx1Send(HEX2ASCII(IR_UserH >> 4));	//�û�����ֽڵĸ߰��ֽ�
			Tx1Send(HEX2ASCII(IR_UserH));		//�û�����ֽڵĵͰ��ֽ�
			Tx1Send(HEX2ASCII(IR_UserL >> 4));	//�û�����ֽڵĸ߰��ֽ�
			Tx1Send(HEX2ASCII(IR_UserL));		//�û�����ֽڵĵͰ��ֽ�

			if(B_IrUserErr)						//�û������
			{
				// DO NOTHING
			} else {
				Tx1Send(0x0d);		//���س�
				Tx1Send(0x0a);		//���س�
				
				B_IR_Press = 0;		//���IR�����±�־
				
				// Add by chenling
				DoWork(IR_code);
			}
		}
	}
}


/********************* ʮ������תASCII���� *************************/
uchar	HEX2ASCII(uchar dat)
{
	dat &= 0x0f;
	if(dat <= 9)	return (dat + '0');	//����0~9
	return (dat - 10 + 'A');			//��ĸA~F
}




//*******************************************************************
//*********************** IR Remote Module **************************

//*********************** IR Remote Module **************************
//this programme is used for Receive IR Remote (HT6121).

//data format: Synchro,AddressH,AddressL,data,/data, (total 32 bit).

//send a frame(85ms), pause 23ms, send synchro of another frame, pause 94ms

//data rate: 108ms/Frame


//Synchro:low=9ms,high=4.5/2.25ms,low=0.5626ms
//Bit0:high=0.5626ms,low=0.5626ms
//Bit1:high=1.6879ms,low=0.5626ms
//frame space = 23 ms or 96 ms

/******************** �������ʱ��궨��, �û���Ҫ�����޸�	*******************/

#if ((D_TIMER0 <= 250) && (D_TIMER0 >= 60))
	#define	D_IR_sample			D_TIMER0		//�������ʱ�䣬��60us~250us֮��
#endif

#define D_IR_SYNC_MAX		(15000/D_IR_sample)	//SYNC max time
#define D_IR_SYNC_MIN		(9700 /D_IR_sample)	//SYNC min time
#define D_IR_SYNC_DIVIDE	(12375/D_IR_sample)	//decide data 0 or 1
#define D_IR_DATA_MAX		(3000 /D_IR_sample)	//data max time
#define D_IR_DATA_MIN		(600  /D_IR_sample)	//data min time
#define D_IR_DATA_DIVIDE	(1687 /D_IR_sample)	//decide data 0 or 1
#define D_IR_BIT_NUMBER		32					//bit number

//*******************************************************************************************
//**************************** IR RECEIVE MODULE ********************************************

void IR_RX_HT6121(void)
{
	uchar	SampleTime;

	IR_SampleCnt++;							//Sample + 1

	F0 = P_IR_RX_temp;						//Save Last sample status
	P_IR_RX_temp = P_IR_RX;					//Read current status
	if(F0 && !P_IR_RX_temp)					//Last sample is high��and current sample is low, so is fall edge
	{
		SampleTime = IR_SampleCnt;			//get the sample time
		IR_SampleCnt = 0;					//Clear the sample counter

			 if(SampleTime > D_IR_SYNC_MAX)		B_IR_Sync = 0;	//large the Maxim SYNC time, then error
		else if(SampleTime >= D_IR_SYNC_MIN)					//SYNC
		{
			if(SampleTime >= D_IR_SYNC_DIVIDE)
			{
				B_IR_Sync = 1;					//has received SYNC
				IR_BitCnt = D_IR_BIT_NUMBER;	//Load bit number
			}
		}
		else if(B_IR_Sync)						//has received SYNC
		{
			if(SampleTime > D_IR_DATA_MAX)		B_IR_Sync=0;	//data samlpe time to large
			else
			{
				IR_DataShit >>= 1;					//data shift right 1 bit
				if(SampleTime >= D_IR_DATA_DIVIDE)	IR_DataShit |= 0x80;	//devide data 0 or 1
				if(--IR_BitCnt == 0)				//bit number is over?
				{
					B_IR_Sync = 0;					//Clear SYNC
					if(~IR_DataShit == IR_data)		//�ж�����������
					{
						if((IR_UserH == (User_code / 256)) &&
							IR_UserL == (User_code % 256))
								B_IrUserErr = 0;	//User code is righe
						else	B_IrUserErr = 1;	//user code is wrong
							
						IR_code      = IR_data;
						B_IR_Press   = 1;			//������Ч
					}
				}
				else if((IR_BitCnt & 7)== 0)		//one byte receive
				{
					IR_UserL = IR_UserH;			//Save the User code high byte
					IR_UserH = IR_data;				//Save the User code low byte
					IR_data  = IR_DataShit;			//Save the IR data byte
				}
			}
		}
	}
}

/**************** ���յ�������Ķ������� ******************************/
void DoWork(uchar keyCode){
	switch(keyCode){
		case 0x45: // CH-
		case 0x46: // CH
		case 0x47: // CH+
			ClrSleepTimer();
			P_LED = 1; // light off
			break;
		case 0x44: // PREV
		case 0x40: // NEXT
		case 0x43: // PLAY/PAUSE
			ClrSleepTimer();
			P_LED = 0; // light on
			break;
		case 0x07: // VOL-
		case 0x15: // VOL+
		case 0x09: // EQ
			break;
		case 0x16: // 0
			ClrSleepTimer();
			break;
		case 0x19: // 100+
			SetSleepTimer(100);
			break;
		case 0x0D: // 200+
			SetSleepTimer(200);
			break;
		case 0x0C: // 1
			SetSleepTimer(10);
			break;
		case 0x18: // 2
			SetSleepTimer(20);
			break;
		case 0x5E: // 3
			SetSleepTimer(30);
			break;
		case 0x08: // 4
			SetSleepTimer(40);
			break;
		case 0x1C: // 5
			SetSleepTimer(50);
			break;
		case 0x5A: // 6
			SetSleepTimer(60);
			break;
		case 0x42: // 7
			SetSleepTimer(70);
			break;
		case 0x52: // 8
			SetSleepTimer(80);
			break;
		case 0x4A: // 9
			SetSleepTimer(90);
			break;
		default:
			P_LED ^= 1;
			break;
	}
}

#define FOSC 11059200L
#define T10MS (65536-FOSC/1000)      //1Tģʽ
//#define T10MS (65536-FOSC/12/1000) //12Tģʽ
/**************** Timer2��ʼ������ ******************************/
void InitTimer2(void)
{
	//	AUXR |= 0x04;                   //��ʱ��2Ϊ1Tģʽ
    AUXR &= ~0x04;                  //��ʱ��2Ϊ12Tģʽ
    T2L = T10MS;                     //��ʼ����ʱֵ
    T2H = T10MS >> 8;
    //AUXR |= 0x10;                   //��ʱ��2��ʼ��ʱ
    IE2 |= 0x04;                    //����ʱ��2�ж�

	EA  = 1;
}

/**************** �趨˯�ߵ���ʱ ******************************/
void SetSleepTimer(uchar min){
	
	// ����ǰû�е���������ӵ���ʱ����
	if(P_LED == 1){
		return;
	}
	
	ClrSleepTimer();
	sleepMin = min;
	
    AUXR |= 0x10;                   //��ʱ��2��ʼ��ʱ
}

/**************** ���˯�ߵ���ʱ ******************************/
void ClrSleepTimer(){
	AUXR &= 0xEF;    //��ʱ��2ֹͣ��ʱ
	tt1m = 0;
	tt1s = 0;
	tt10ms = 0;
}

//-----------------------------------------------
//T2�жϷ������
void t2int() interrupt 12           //�ж����
{
    tt10ms++;
	
	if (tt10ms >= 100) {
		tt10ms = 0;
		tt1s++;
		P33 ^= 1;
		if (tt1s >= 60) {
			tt1s = 0;
			tt1m++;
		}
	}
	
	if (tt1m >= sleepMin) {
		// ��ʱʱ�䵽���ص�
		P_LED = 1;
		ClrSleepTimer();
	}
      
//  IE2 &= ~0x04;                   //����Ҫ�ֶ�����жϱ�־,���ȹر��ж�,��ʱϵͳ���Զ�����ڲ����жϱ�־
//  IE2 |= 0x04;                    //Ȼ���ٿ��жϼ���
}


/**************** Timer0��ʼ������ ******************************/
void InitTimer(void)
{
	TMOD = 0;		//for STC15Fxxxϵ��	Timer0 as 16bit reload timer.
	TH0 = Timer0_Reload / 256;
	TL0 = Timer0_Reload % 256;
	ET0 = 1;
	TR0 = 1;

	EA  = 1;
}


/********************** Timer0�жϺ���************************/
void timer0 (void) interrupt 1
{
	IR_RX_HT6121();
}


/********************** ģ�⴮����غ���************************/

void	BitTime(void)	//λʱ�亯��
{
	uint i;
	i = ((MAIN_Fosc / 100) * 104) / 140000 - 1;		//������ʱ��������λʱ��
	while(--i);
}

//ģ�⴮�ڷ���
void	Tx1Send(uchar dat)		//9600��N��8��1		����һ���ֽ�
{
	uchar	i;
	EA = 0;
	P_TXD1 = 0;
	BitTime();
	for(i=0; i<8; i++)
	{
		if(dat & 1)		P_TXD1 = 1;
		else			P_TXD1 = 0;
		dat >>= 1;
		BitTime();
	}
	P_TXD1 = 1;
	EA = 1;
	BitTime();
	BitTime();
}

void PrintString(unsigned char code *puts)		//����һ���ַ���
{
    for (; *puts != 0;	puts++)  Tx1Send(*puts); 	//����ֹͣ��0����
}
