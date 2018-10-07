#include <STC12C5A60S2.h>      
#include <intrins.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char
#define NOP() _nop_()

sbit EN_port = P1^3;
sbit DA_in_port = P1^2;
sbit CLK_port = P1^1;
sbit Latch_port = P1^0;

// for test led
sbit LED = P3^7;

// Urat --------------------------------------------------------------------------------
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

sbit bit9 = P2^2;           //P2.2 show UART data bit9
bit busy;

uchar rec[10]={0,0,0,0,0,0,0,0,0,0};
uchar recIdx=0;
// Urat --------------------------------------------------------------------------------

#define ABCD_port P1  //HC138 ABCD Port
//sbit D_port = P1^7;
//sbit C_port = P1^6;
//sbit B_port = P1^5;
//sbit A_port = P1^4;

// GT20L16J1Y Interface ------------------------------------------------
sbit pCE        =P0^0;         //
sbit pSCLK      =P0^1;         //
sbit pSI        =P0^2;         //
sbit pSO        =P0^3;         //

unsigned long addr;
int idx = 0;
uchar data ic_data[32];

uchar* getICData_ASCII_8x16(uchar ch);
uchar* getICData_Hanzi_16x16(uchar* hz);
void readICDataToBuffer(uchar bufNum, uchar* str);

// GT20L16J1Y Interface ------------------------------------------------

void HC595_Data_Send(uchar *p, uchar han, uchar zishu, uchar offset);
void setICDataToBuffer(uchar nowBufferNum, uchar *pICData, uchar size, uchar pos);

uchar xdata bufHZ[2][128+32] = {
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
	}
};

uchar code ic_data_A[] = {0x00,0xE0,0x9C,0x82,0x9C,0xE0,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x0F,0x00};

uchar nowOffset=0;
int nowPos=0;

// int showDataSize=960*2;
// uchar code showData[] = "";
int showDataSize=485*2;
uchar code showData[] = "子供達の間でうわざになっている化物の子供のお話をします。◇化物の子供は、暖かさや寒さ、おいしさやまずさ、痛さや痛くないが分かりませんでした。◇だから、思い出すのも苦労するような昔、子供達は一緒に遊んだ化物の子供をおかしいと思い、化物の子供と呼ぶようになったのです。◇だっておかしいですよね。子供達はお菓子を食べておいしいと感じるのに、化物の子供は何も感じないのですから。◇だけど、化物の子供から遠ざかる子供達とは違って、近づいていく子供がいたのです。◇化物の子供と同じ年の男の子は、一緒に遊ぶようになりました。◇体がだるくなるような暑さの日に太陽の日差しを避けようとしなくても、顔が真っ青になるくらいまずい食べ物を食べて顔色一つ変えなくても、遊んでいたボールで頭を怪我して平然と立っていても、男の子は化物の子供と遊ぶのが楽しかったので、いつも一緒にいました。◇だって、たとえ感じなくても◇木陰で汗の流れる体を休ませている時は、いつも気づかって一杯の水筒の水を差し出してくれます。作ってきた手作りお菓子を食べさせたときは、嬉しそうに笑ってくれます。★★★終わり★★★　　　";

uchar data disp_buff[10];

int tt = 0;
int tt1 = 0;
void Timer0Init(void)		
{
	EA=1;
    ET0=1;
	
	//AUXR &= 0x7F;
	TMOD &= 0xF0;	
	TMOD |= 0x01;	
	TL0 = 0xB0;		
	TH0 = 0x3C;		
	TF0 = 0;		
	TR0 = 1;		
}

uchar rowIdx=0; 
uchar nowBufferNum=0;
void display(){
	uchar i;
	
	for (i=0;i<10;i++){
		disp_buff[i]=bufHZ[nowBufferNum][i*16 + rowIdx];
	}
	
	HC595_Data_Send(disp_buff,rowIdx,4,nowOffset);
	
	rowIdx++;
	
	if(rowIdx==16) rowIdx=0;
}

void main()
{
	UartInit();
	
	Timer0Init();
	
	// For test --------------------------------------------------------------------------------
	readICDataToBuffer(nowBufferNum, showData);
	// For test --------------------------------------------------------------------------------
	
	while (1){
		display();
		
		if(recIdx >= 8){
			readICDataToBuffer(nowBufferNum,rec);
			recIdx=0;
		}
	}
}

void setICDataToBuffer(uchar bufNum, uchar *pICData, uchar size, uchar pos)
{
	uchar bufByteIdx = 0;
	uchar icByteIdx = 0;
	
	uchar* pTmpByteBufHZ;
	
	//clear target data
	//memset(&bufHZ[nowBufferNum][pos*16], size, 0x00);
	
	switch(size){
		case 32:
			for (bufByteIdx=0; bufByteIdx<size; bufByteIdx++){
				pTmpByteBufHZ = &(bufHZ[bufNum][pos*16 + bufByteIdx]);
				display();//test
				pTmpByteBufHZ[0] = 0x00;
				
				if(bufByteIdx < 8){
					for (icByteIdx=0; icByteIdx<8; icByteIdx++){
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							pTmpByteBufHZ[0] |= 0x01;
						} else {
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
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
			}
			break;
		case 16:
			for (bufByteIdx=0; bufByteIdx<size; bufByteIdx++){
				//display();//test
					
				pTmpByteBufHZ = &(bufHZ[bufNum][pos*16 + bufByteIdx]);
				
				pTmpByteBufHZ[0] = 0x00;
				
				if(bufByteIdx < 8){
					for (icByteIdx=0; icByteIdx<8; icByteIdx++){
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							pTmpByteBufHZ[0] |= 0x01; // 0000 0001
						} else {
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
				
				if(bufByteIdx >= 8 && bufByteIdx < 16 ){
					for (icByteIdx=8; icByteIdx<16; icByteIdx++){
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							pTmpByteBufHZ[0] |= 0x01; // 0000 0001
						} else {
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

void readICDataToBuffer(uchar bufNum, uchar* str){
	uchar pos;
	
	for (pos=0; pos<10; pos++){
		if (str[pos] >= 0x20 && str[pos] <= 0x7E) {
			setICDataToBuffer(bufNum, getICData_ASCII_8x16(str[pos]), 16, pos); 
		} else {
			setICDataToBuffer(bufNum, getICData_Hanzi_16x16(&str[pos]), 32, pos);
			pos++;
		}
	}
}

uchar* getICData(unsigned long addr, uchar size) {
	
	//init
	pCE=1;
	pSI=0;
	pSCLK=0;
	
	pCE=0;
	
	for (idx = 0; idx<8; idx++){
		pSCLK=0;
		pSI=0x03<<idx & 0x80;
		pSCLK=1;
	}
	
	for (idx = 0; idx<24; idx++){
		pSCLK=0;
		pSI=addr<<idx & 0x800000;
		
		// For test break point
		if(idx==23){
			idx = 23;
		}
		
		pSCLK=1;
	}
	
		for (idx = 0; idx<8*size ; idx++){
		pSCLK=0;
		
		if (pSO == 0) {
			ic_data[idx/8] &= 0xFE; // 1111 1110 
		} else {
			ic_data[idx/8] |= 0x01; // 0000 0001
		}
		
		if(((idx+1)%8) > 0){
			ic_data[idx/8] <<= 1;
		} else {
		}
		
		pSCLK=1;
		
		display();//test
	}
	
	pCE=1;
	return ic_data;
}

uchar* getICData_ASCII_8x16(uchar ch) {
	addr = (ch-0x20)*16+0x3b7c0;
	return getICData(addr, 16);
}

uchar* getICData_Hanzi_16x16(uchar* hz) {
	unsigned long hzSJISCodeH8, hzSJISCodeL8;
	
	hzSJISCodeH8 = hz[0] & 0xFF;
	hzSJISCodeL8 = hz[1] & 0xFF;
	
	// シフトJISコードをJISコードに変換
    hzSJISCodeH8 -= (hzSJISCodeH8 <= 0x9F) ? 0x71 : 0xB1;
    hzSJISCodeH8 = hzSJISCodeH8 * 2 + 1;
    if ( hzSJISCodeL8 > 0x7F )     hzSJISCodeL8--;
    if ( hzSJISCodeL8 >= 0x9E )    hzSJISCodeL8 -= 0x7D, hzSJISCodeH8++;
    else                    hzSJISCodeL8 -= 0x1F;

    // JISコードを区点コードに変換
    hzSJISCodeH8 -= 0x20;
    hzSJISCodeL8 -= 0x20;
	
	if(hzSJISCodeH8 >=1 && hzSJISCodeH8 <= 15 && hzSJISCodeL8 >=1 && hzSJISCodeL8 <= 94)
		addr =( (hzSJISCodeH8 - 1) * 94 + (hzSJISCodeL8 - 01))*32;
	else if(hzSJISCodeH8 >=16 && hzSJISCodeH8 <= 47 && hzSJISCodeL8 >=1 && hzSJISCodeL8 <= 94)
		addr =( (hzSJISCodeH8 - 16) * 94 + (hzSJISCodeL8 - 1))*32+43584;
	else if(hzSJISCodeH8 >=48 && hzSJISCodeH8 <=84 && hzSJISCodeL8 >=1 && hzSJISCodeL8 <= 94)
		addr = ((hzSJISCodeH8 - 48) * 94 + (hzSJISCodeL8 - 1))*32+ 138463;
	else if(hzSJISCodeH8 ==85 && hzSJISCodeL8 >=0x01 && hzSJISCodeL8 <= 94)
		addr = ((hzSJISCodeH8 - 85) * 94 + (hzSJISCodeL8 - 1))*32+ 246944;
	else if(hzSJISCodeH8 >=88 && hzSJISCodeH8 <=89 && hzSJISCodeL8 >=1 && hzSJISCodeL8 <= 94)
		addr = ((hzSJISCodeH8 - 88) * 94 + (hzSJISCodeL8 - 1))*32+ 249952;


	
	// if(hzSJISCodeH8 ==0xA9 && hzSJISCodeL8 >=0xA1)
		// addr = (282 + (hzSJISCodeL8 - 0xA1 ))*32;
	// else if(hzSJISCodeH8 >=0xA1 && hzSJISCodeH8 <= 0xA3 && hzSJISCodeL8 >=0xA1)
		// addr =( (hzSJISCodeH8 - 0xA1) * 94 + (hzSJISCodeL8 - 0xA1))*32;
	// else if(hzSJISCodeH8 >=0xB0 && hzSJISCodeH8 <= 0xF7 && hzSJISCodeL8 >=0xA1) {
		// // addr = ((hzSJISCodeH8 - 0xB0) * 94 + (hzSJISCodeL8 - 0xA1)+ 846)*32;
		// addr = (hzSJISCodeH8 - 0xB0) * 94;
		// addr += (hzSJISCodeL8 - 0xA1) + 846;
		// addr *= 32;
	// }
	
	return getICData(addr, 32);
}

void HC595_Data_Send(uchar *p, uchar han, uchar zishu, uchar offset)
{
	uchar i=0;
	uchar temp=0;
	uchar* data_buff;
	Latch_port = 0;  
	CLK_port = 0;
	
	for(temp=0;temp<8;temp++){
		data_buff = &p[temp];
		for(i=0;i<8;i++){
			if(((data_buff[0]<<i)&0x80)!=0) DA_in_port = 0;
			else DA_in_port = 1;
			CLK_port = 1;
			CLK_port = 0;
		}
	}
	
	for(i=0;i<offset;i++){
		data_buff = &p[temp + (i+8)/8-1]; //TODO
		if(((data_buff[0]<<(i%8))&0x80)!=0) DA_in_port = 0;
		else DA_in_port = 1;
		CLK_port = 1;
		CLK_port = 0;
	}

	EN_port = 1; 

	ABCD_port = (ABCD_port & 0x0f)|(han<<4);

	Latch_port = 1; 
	EN_port = 0;
	Latch_port = 0;	

}

uchar isLeft=0;
void timer0() interrupt 1
{
	// 20ms
	tt++;
	TL0 = 0xB0;		
	TH0 = 0xFC;	
	
	display();
	
	//if(tt >= 5){
	if(tt >= 30){
		// 1s
		tt = 0;
		
		if(nowOffset==15){
			
			EN_port = 1; //off screen
			
			nowOffset=0;
			
			if (nowBufferNum == 0) {
				nowBufferNum = 1;
				readICDataToBuffer(0, &showData[nowPos+=2]);
			} else {
				nowBufferNum = 0;
				readICDataToBuffer(1, &showData[nowPos+=2]);
			}
			
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
        bit9 = RB8;         //P2.2 show parity bit
		
		
		
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