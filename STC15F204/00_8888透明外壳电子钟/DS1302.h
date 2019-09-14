
#define DS1302_DATA P1

#define uchar unsigned char
#define uint unsigned int

sbit	SCLK_DS1302=P1^2;
sbit	RST_DS1302=P1^0;
sbit	IO_DS1302=P1^1;

unsigned char code DS1302_ID[]="DaNKer";

//==============系统标志====================================================
unsigned char clock_ctr=0;
#define        F_CTR_HR    0x80        //1=12小时制，0=24小时制
#define        F_CTR_AP    0x20        //1=AM,0=PM
#define        F_CTR_AL    0x01        //闹钟标志,为1时打开闹钟
//=========================================================================
unsigned char  SEC_SCAN_MODE = 0;            //秒钟显示模式

#define		ALARM_ADDR	0xD0		//闹钟时间存放地址
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
void SendByte(unsigned char sdate)		  //单片机发送一位数据 
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		
		if(sdate&0x01!=0)		  //从低位开始发送 
		IO_DS1302=1;
		else IO_DS1302=0;
		SCLK_DS1302=0;
//		i=i;
//		i++;
//		i--;
		SCLK_DS1302=1;					  //时钟上升沿1302接收一位数据 
		sdate=sdate>>1;
	}
	SCLK_DS1302=0;
}
unsigned char InptByte()	   //单片机接收一位数据 
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
		SCLK_DS1302=0;		  //时钟下降沿1302发送一位数据 
	}
	SCLK_DS1302=0;
	return(tdate);
}
void WriteByte(unsigned char add,unsigned char sdate)	 //单片机给某地址发送一位数据  
{
	RST_DS1302=0;							 //复位脚为0，终止所有数据传输 
	SCLK_DS1302=0;							 //时钟脚为0时复位脚才能被拉高 
	RST_DS1302=1;							 
	SendByte(add);					 //先发送地址 
	SendByte(sdate);				 //再发送数据  
	SCLK_DS1302=0;
	RST_DS1302=0;	
}
unsigned char ReadByte(unsigned char add) 		     //单片机从某地址接收一位数据 
{
	unsigned char tdate;
	RST_DS1302=0;
	SCLK_DS1302=0;
	RST_DS1302=1;										   //先发送地址 
	SendByte(add);								   //再读数据  
	tdate=InptByte();
	SCLK_DS1302=1;
	RST_DS1302=0;
	return(tdate);
}
void WriteTime(unsigned char add,unsigned char tim)		 //给1302发送设置的时间 
{
	WriteByte(0x8e,0x00);					 //允许写操作 
	WriteByte(add,tim);			 //先写地址，再写时间数据，时间数据放在数组中 
	WriteByte(0x8e,0x80);
}
unsigned char ReadTime(unsigned char add)			 //从1302读时间值  
{
	unsigned char tme;
	WriteByte(0x8e,0x00);
	tme=ReadByte(add); 
	WriteByte(0x8e,0x80);
	return tme;
}
//==============================================================================
//	DS1302初使化，增加了初使化标识
//	先读DS1302的标识，如果读出标识正确，则说明1302已经初使化过，不再初使化
//==============================================================================
void DS1302_init()
{
	unsigned char i,temp,wrong_x=0;	
	unsigned char *strp;
	strp=DS1302_ID;
	WriteTime(0x8e,0x00);
   	WriteTime(0x90,0x55);		//关闭电池充电功能
   	
	while((*strp)&&(wrong_x<5))
	{
		temp=ReadTime(0xC1+(i<<1));
		if(temp!=*strp)		//如果读出来的数与ID码不等
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
			while(1) // 这个检测在调试阶段应该注释掉，否则外部没有DS1302硬件的时候会一直停在这里
			{
				//TODO 无1302则进入报警
			}		
		}
		WriteTime(0x80,0x00);	//秒
	   	WriteTime(0x82,0x00);  //分
	   	WriteTime(0x84,0x12);  //时
	   	WriteTime(0x86,0x01);  //日
	   	WriteTime(0x88,0x01);  //月
		WriteTime(0x8c,0x12); //年
		
		clock_ctr=0;		//初使化时间控制，12/24小时制，AM/PM，闹钟使能
		alarm_shi=12;
		alarm_fen=0;		//初使化闹钟时间
		WriteTime(ALARM_ADDR,clock_ctr);
		WriteTime(ALARM_ADDR+2,alarm_shi);
		WriteTime(ALARM_ADDR+4,alarm_fen);
		WriteTime(SecMod_WrADDR,0x00);			//秒钟显示模式
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
//		读闹钟数据
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
//        读取1302内的时钟信息
//=========================================================================
void Get_SYS_Value()
{
    SEC_SCAN_MODE=ReadTime(SecMod_ReADDR);    //读取秒显示模式
    get_alarm_data();            //读取闹钟信息
}
//=========================================================================
//        读时间
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
        case 0:    temp=ReadTime(0x8D);        //年
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            nian=time_H+time_L;
            i++;
            break;
        case 1:    temp=ReadTime(0x89);        //月
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            yue=time_H+time_L;
            i++;
            break;
        case 2:    temp=ReadTime(0x87);        //日
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            ri=time_H+time_L;
            i++;
            break;
        case 3:    temp=ReadTime(0x85);        //时
//            if(temp&F_CTR_HR)
//            {
//                clock_ctr&=~F_CTR_AP;
//                clock_ctr|=temp&F_CTR_AP;
//                temp&=0x1F;        //为12小时制
//            }
//            else
//            {
                temp&=0x3f;        //为24小时制
//            }
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            shi=time_H+time_L;
            i++;
            break;
        case 4:    temp=ReadTime(0x83);        //分
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            fen=time_H+time_L;
            i++;
            break;
        case 5:    temp=ReadTime(0x81);        //秒
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
        case 7:    temp=ReadTime(0x85);        //时
//            if(temp&F_CTR_HR)
//            {
//                clock_ctr&=~F_CTR_AP;
//                clock_ctr|=temp&F_CTR_AP;
//                temp&=0x1F;        //为12小时制
//            }
//            else
//            {
                temp&=0x3f;        //为24小时制
//            }
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            shi=time_H+time_L;
            i++;
            break;
        case 8:    temp=ReadTime(0x83);        //分
            time_H=(temp>>4)*10;time_L=temp&0x0f;
            fen=time_H+time_L;
            i++;
            break;
        case 9:    temp=ReadTime(0x81);        //秒
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