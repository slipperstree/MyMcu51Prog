
#define DS1302_DATA P1

#define uchar unsigned char
#define uint unsigned int

sbit	SCLK_DS1302=P1^2;
sbit	RST_DS1302=P1^0;
sbit	IO_DS1302=P1^1;

unsigned char code DS1302_ID[]="DaNKer";

//==============ϵͳ��־====================================================
unsigned char clock_ctr=0;
#define        F_CTR_HR    0x80        //1=12Сʱ�ƣ�0=24Сʱ��
#define        F_CTR_AP    0x20        //1=AM,0=PM
#define        F_CTR_AL    0x01        //���ӱ�־,Ϊ1ʱ������
//=========================================================================
unsigned char  SEC_SCAN_MODE = 0;            //������ʾģʽ

#define		ALARM_ADDR	0xD0		//����ʱ���ŵ�ַ
#define		SecMod_WrADDR	0xD6
#define		SecMod_ReADDR	0xD7

unsigned char alarm_shi;alarm_fen;
unsigned char nian,yue,ri,shi,fen,miao;
unsigned char alarm_shi;alarm_fen;

void delay_ms(unsigned int n)
{
	unsigned int x,y;
	for(x=n;x>0;x--)
	{
		for(y=110;y>0;y--);	
	}
}

/************************DS1302**********************/
void SendByte(unsigned char sdate)		  //��Ƭ������һλ���� 
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		
		if(sdate&0x01!=0)		  //�ӵ�λ��ʼ���� 
		IO_DS1302=1;
		else IO_DS1302=0;
		SCLK_DS1302=0;
//		i=i;
//		i++;
//		i--;
		SCLK_DS1302=1;					  //ʱ��������1302����һλ���� 
		sdate=sdate>>1;
	}
	SCLK_DS1302=0;
}
unsigned char InptByte()	   //��Ƭ������һλ���� 
{
	unsigned char tdate,i;
	tdate=0;
	for(i=0;i<8;i++)
	{
		tdate=tdate>>1;
		
		if(IO_DS1302==1)
		{
			tdate|=0x80;
		}
		SCLK_DS1302=1;
//		i++;
//		i--;
		SCLK_DS1302=0;		  //ʱ���½���1302����һλ���� 
	}
	SCLK_DS1302=0;
	return(tdate);
}
void WriteByte(unsigned char add,unsigned char sdate)	 //��Ƭ����ĳ��ַ����һλ����  
{
	RST_DS1302=0;							 //��λ��Ϊ0����ֹ�������ݴ��� 
	SCLK_DS1302=0;							 //ʱ�ӽ�Ϊ0ʱ��λ�Ų��ܱ����� 
	RST_DS1302=1;							 
	SendByte(add);					 //�ȷ��͵�ַ 
	SendByte(sdate);				 //�ٷ�������  
	SCLK_DS1302=0;
	RST_DS1302=0;	
}
unsigned char ReadByte(unsigned char add) 		     //��Ƭ����ĳ��ַ����һλ���� 
{
	unsigned char tdate;
	RST_DS1302=0;
	SCLK_DS1302=0;
	RST_DS1302=1;										   //�ȷ��͵�ַ 
	SendByte(add);								   //�ٶ�����  
	tdate=InptByte();
	SCLK_DS1302=1;
	RST_DS1302=0;
	return(tdate);
}
void WriteTime(unsigned char add,unsigned char tim)		 //��1302�������õ�ʱ�� 
{
	WriteByte(0x8e,0x00);					 //����д���� 
	WriteByte(add,tim);			 //��д��ַ����дʱ�����ݣ�ʱ�����ݷ��������� 
	WriteByte(0x8e,0x80);
}
unsigned char ReadTime(unsigned char add)			 //��1302��ʱ��ֵ  
{
	unsigned char tme;
	WriteByte(0x8e,0x00);
	tme=ReadByte(add); 
	WriteByte(0x8e,0x80);
	return tme;
}
//==============================================================================
//	DS1302��ʹ���������˳�ʹ����ʶ
//	�ȶ�DS1302�ı�ʶ�����������ʶ��ȷ����˵��1302�Ѿ���ʹ���������ٳ�ʹ��
//==============================================================================
void DS1302_init()
{
	unsigned char i,temp,wrong_x=0;	
	unsigned char *strp;
	strp=DS1302_ID;
	WriteTime(0x8e,0x00);
   	WriteTime(0x90,0x55);		//�رյ�س�繦��
   	
	while((*strp)&&(wrong_x<5))
	{
		temp=ReadTime(0xC1+(i<<1));
		if(temp!=*strp)		//���������������ID�벻��
		{
			i=0;
			strp=DS1302_ID;
			wrong_x++;
		}
		else
		{
			i++;
			strp++;
		}
	}
	if(*strp)  
	{
		WriteTime(0xC0,0xaa);
		delay_ms(10);
		temp=ReadTime(0xC1);
		if(temp!=0xaa)
		{
			while(1) // �������ڵ��Խ׶�Ӧ��ע�͵��������ⲿû��DS1302Ӳ����ʱ���һֱͣ������
			{
				//TODO ��1302����뱨��
			}		
		}
		WriteTime(0x80,0x00);	//��
	   	WriteTime(0x82,0x00);  //��
	   	WriteTime(0x84,0x12);  //ʱ
	   	WriteTime(0x86,0x01);  //��
	   	WriteTime(0x88,0x01);  //��
		WriteTime(0x8c,0x12); //��
		
		clock_ctr=0;		//��ʹ��ʱ����ƣ�12/24Сʱ�ƣ�AM/PM������ʹ��
		alarm_shi=12;
		alarm_fen=0;		//��ʹ������ʱ��
		WriteTime(ALARM_ADDR,clock_ctr);
		WriteTime(ALARM_ADDR+2,alarm_shi);
		WriteTime(ALARM_ADDR+4,alarm_fen);
		WriteTime(SecMod_WrADDR,0x00);			//������ʾģʽ
		strp=DS1302_ID;
		while(*strp)
		{
			WriteTime(0xC0+(i<<1),*strp);
			i++;
			strp++;
		}
	}
	WriteTime(0x8e,0x80);
}

//===================================================================================
//		����������
//===================================================================================
void get_alarm_data()
{
	unsigned char temp1;
	temp1=ReadTime(ALARM_ADDR+1);
	if(temp1&F_CTR_AL)
	{
		clock_ctr|=F_CTR_AL;
		alarm_shi=ReadTime(ALARM_ADDR+3);
		alarm_fen=ReadTime(ALARM_ADDR+5);
	}
}

//=========================================================================
//        ��ȡ1302�ڵ�ʱ����Ϣ
//=========================================================================
void Get_SYS_Value()
{
    SEC_SCAN_MODE=ReadTime(SecMod_ReADDR);    //��ȡ����ʾģʽ
    get_alarm_data();            //��ȡ������Ϣ
}
//=========================================================================
//        ��ʱ��
//out:    nian,yue,ri,shi,fen,miao
//Author: Danker3
//Date:   2012.12.17
//=========================================================================
void GetTime()
{
    static unsigned char i=0,temp1;
    unsigned char time_H,time_L,temp;
    switch(i)
    {
        case 0:    temp=ReadTime(0x8D);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            nian=time_H+time_L;
            i++;
            break;
        case 1:    temp=ReadTime(0x89);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            yue=time_H+time_L;
            i++;
            break;
        case 2:    temp=ReadTime(0x87);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            ri=time_H+time_L;
            i++;
            break;
        case 3:    temp=ReadTime(0x85);        //ʱ
//            if(temp&F_CTR_HR)
//            {
//                clock_ctr&=~F_CTR_AP;
//                clock_ctr|=temp&F_CTR_AP;
//                temp&=0x1F;        //Ϊ12Сʱ��
//            }
//            else
//            {
                temp&=0x3f;        //Ϊ24Сʱ��
//            }
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            shi=time_H+time_L;
            i++;
            break;
        case 4:    temp=ReadTime(0x83);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            fen=time_H+time_L;
            i++;
            break;
        case 5:    temp=ReadTime(0x81);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            miao=time_H+time_L;
            i++;
            break;
        case 6:
            temp=fen+miao;
            if(temp1!=temp)
            {
                i++;
                temp1=temp;
            }
            else
            {
                i=10;
            }
            break;
        case 7:    temp=ReadTime(0x85);        //ʱ
//            if(temp&F_CTR_HR)
//            {
//                clock_ctr&=~F_CTR_AP;
//                clock_ctr|=temp&F_CTR_AP;
//                temp&=0x1F;        //Ϊ12Сʱ��
//            }
//            else
//            {
                temp&=0x3f;        //Ϊ24Сʱ��
//            }
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            shi=time_H+time_L;
            i++;
            break;
        case 8:    temp=ReadTime(0x83);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            fen=time_H+time_L;
            i++;
            break;
        case 9:    temp=ReadTime(0x81);        //��
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            miao=time_H+time_L;
            i++;
            break;
        case 10:
            i=0;
            //GetDispBuf();
            break;
        default:i=0;break;
    }
//    if((++i)>=6)i=0;
}