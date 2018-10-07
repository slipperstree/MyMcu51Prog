/*********************************************************************************************/
/* ���Ե�Ƭ�� STC15F100ϵ�е�Ƭ�� ���Զ���Һ����[P3.0-P3.5�����������ߵ͵�ƽ]              */
/*********************************************************************************************/

#include<STC12x20xx.h>


#define uchar unsigned char
#define uint unsigned int

sbit PIN1_COM4 = P1^7;
sbit PIN2_COM3 = P1^6;
sbit PIN3_COM2 = P1^5;
sbit PIN4_COM1 = P1^4;

sbit PIN5 = P1^3;  // 1E, 1G, 1F, 4G
sbit PIN6 = P1^2;  // 1D, 1C, 1B, 1A
sbit PIN7 = P1^1;  // 2E, 2G, 2F, 4D
sbit PIN8 = P1^0;  // 2D, 2C, 2B, 2A
sbit PIN9 = P3^7;  // 3E, 3G, 3F, 4A
sbit PIN10 = P3^5; // 3D, 3C, 3B, 3A

// map[�м���COM��ÿ��com��Ӧ���Σ� ÿ�εĶ���һ��Ϊ2λ����]
// ����888C���ֶ�������4��com
// ÿ��com����6���Σ�����4G��1A��4D��2A��4A��3A��ÿ����0x1A������ʽ��ʾ��1A��ʾ��һλ���ֵ�A�Ρ�����G����9��ʾ��16������û��G����
uchar code map_com[4][6] = { 
	{0x49, 0x1A, 0x4D, 0x2A, 0x4A, 0x3A},
	{0x1F, 0x1B, 0x2F, 0x2B, 0x3F, 0x3B},
	{0x19, 0x1C, 0x29, 0x2C, 0x39, 0x3C},
	{0x1E, 0x1D, 0x2E, 0x2D, 0x3E, 0x3D}
};

void setPinValueByPinNum(uchar nPinNum, uchar value);
void setValueByCom(uchar nCom, uchar nDig, uchar num);
void setValue(uchar nCom);
void Timer0Init();
void delay(uint t);
void delay(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
	for(j=0;j<250;j++);
}

uchar nowCom=1; // ��ǰˢ�µ��ڼ���COM(1-4)
int ttMs=0;
int ttSec=0;

int numForShow = 0;

uchar idxTmp;

main()
{
	// �ϵ�Ĭ����׼˫���
	// 7-0λ����Ϊ1�Ļ���P1M1�Ķ�Ӧ��λ����������������00����׼˫��ڣ������10��������̬��Ҳ���Ǹ���̬
	P1M0 = 0x00;
	P1M1 = 0x00;

	P33 = 0;
	P34 = 0;

	Timer0Init();

	while(1){
		
	}
}

void Timer0Init()		//100΢��@5.019MHz

{
	EA=1;		// �����ж��ܿ��ش�
    ET0=1;		// ��ʱ��0��Ч

	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x02;		//���ö�ʱ��ģʽ
	TL0 = 0xD6;		//���ö�ʱ��ֵ
	TH0 = 0xD6;		//���ö�ʱ����ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

void timer0() interrupt 1
{
	TR0 = 0;

	ttMs++;
	ttSec++;

	//P1M0 = 0xF0;
	// ��ͣ�÷�com��seg�ĵ�ƽ
	PIN4_COM1 = ~PIN4_COM1;
	PIN3_COM2 = ~PIN3_COM2;
	PIN2_COM3 = ~PIN2_COM3;
	PIN1_COM4 = ~PIN1_COM4;

	PIN5  = ~PIN5;
	PIN6  = ~PIN6;
	PIN7  = ~PIN7;
	PIN8  = ~PIN8;
	PIN9  = ~PIN9;
	PIN10  = ~PIN10;


	// �л�COM
	if (ttMs >= 30) // 1mS
	{
		ttMs=0;

		nowCom++;
		if (nowCom > 4)
		{
			nowCom = 1;
		}

		// reset all pins
		for (idxTmp = 1; idxTmp <= 10; ++idxTmp)
		{
			setPinValueByPinNum(idxTmp, 0);
		}

		P1M0 = 0xF0;// �Ƚ�4��COMȫ�����ø���̬

		//setValue(nowCom);// �����µ�����
		setValueByCom(nowCom, 1, numForShow % 1000 / 100);
		setValueByCom(nowCom, 2, numForShow % 100 / 10);
		setValueByCom(nowCom, 3, numForShow % 10);
		setValueByCom(nowCom, 4, 10);// wendu mark C

		// ������һ��COM
		switch(nowCom){
			case 1:
				// ����COM1Ϊ׼˫��ڣ�����3��Ϊ����̬(����̬COM��صĶβ�����)
				P1M0 = 0xE0; // 1110 0000 (��4λΪcom4-com1����4λ��seg���ã�һֱ����׼˫���)
				break;
			case 2:
				// ����COM2Ϊ׼˫��ڣ�����3��Ϊ����̬
				P1M0 = 0xD0; // 1101 0000
				break;
			case 3:
				// ����COM3Ϊ׼˫��ڣ�����3��Ϊ����̬
				P1M0 = 0xB0; // 1011 0000
				break;
			case 4:
				// ����COM4Ϊ׼˫��ڣ�����3��Ϊ����̬
				P1M0 = 0x70; // 0111 0000
				break;
		}
	}

	// ��ʾ���ּ�һ
	if (ttSec >= 10000) // 1S
	{
		ttSec = 0;
		numForShow++;
		//P34 = ~P34;
	}

	TR0 = 1;
}


// ָ����com(1-4)��ָ��λ(1-4)��ָ�����ݣ����֣�
void setValueByCom(uchar nCom, uchar nDig, uchar num ){
	uchar i = 0;
	uchar mapComVal;
	uchar pinNum;

	// ָ��COM�õ͵�ƽ
	setPinValueByPinNum(nCom, 0);

	for (pinNum = 5; pinNum <= 10; ++pinNum)
	{
		mapComVal = map_com[nCom-1][pinNum-5];
		switch(num){
			case 0:
				if (//ABCDEF
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 1:
				if (//BC
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 2:
				if (//ABDEG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 3:
				if (//ABCDG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 4:
				if (//BCFG(9 is G)
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 5:
				if (//ACDFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 6:
				if (//ACDEFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 7:
				if (//ABC
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 8:
				if (//ABCDEFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0E) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 9:
				if (//ABCDFG(9 is G)
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0B) == mapComVal ||
					((nDig<<4) | 0x0C) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal ||
					((nDig<<4) | 0x0F) == mapComVal ||
					((nDig<<4) | 0x09) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
			case 10:	// ��4λ������������϶�
				if (//AD
					((nDig<<4) | 0x0A) == mapComVal ||
					((nDig<<4) | 0x0D) == mapComVal
					) {
					setPinValueByPinNum(pinNum, 1);
				}
				break;
		}
	}
}

void setPinValueByPinNum(uchar nPinNum, uchar value) {
	switch(nPinNum){
		case 1:
			PIN1_COM4 = value;
			break;
		case 2:
			PIN2_COM3 = value;
			break;
		case 3:
			PIN3_COM2 = value;
			break;
		case 4:
			PIN4_COM1 = value;
			break;
		case 5:
			PIN5 = value;
			break;
		case 6:
			PIN6 = value;
			break;
		case 7:
			PIN7 = value;
			break;
		case 8:
			PIN8 = value;
			break;
		case 9:
			PIN9 = value;
			break;
		case 10:
			PIN10 = value;
			break;
	}
}

// dummy show 123C
void setValue(uchar nCom){
	switch(nCom){
		case 1:
			PIN4_COM1 = 0;

			PIN5 = 0;
			PIN6 = 0;
			PIN7 = 1;
			PIN8 = 1;
			PIN9 = 1;
			PIN10 = 1;
			break;
		case 2:
			PIN3_COM2 = 0;
			
			PIN5 = 0;
			PIN6 = 1;
			PIN7 = 0;
			PIN8 = 1;
			PIN9 = 0;
			PIN10 = 1;
			break;
		case 3:
			PIN2_COM3 = 0;
			
			PIN5 = 0;
			PIN6 = 1;
			PIN7 = 1;
			PIN8 = 0;
			PIN9 = 1;
			PIN10 = 1;
			break;
		case 4:
			PIN1_COM4 = 0;
			
			PIN5 = 0;
			PIN6 = 0;
			PIN7 = 1;
			PIN8 = 1;
			PIN9 = 0;
			PIN10 = 1;
			break;
		}
}