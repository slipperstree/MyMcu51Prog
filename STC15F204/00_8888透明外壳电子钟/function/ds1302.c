#include "../header/ds1302.h"
#include "../header/common.h"

// 在没有调通DS1302之前或者没有DS1302硬件的情况下可以放开下面这句 ############
// 对外接口不变的情况下提供『假』的DS1302，也可用于调试程序用，避免修改真实时间
//#define USE_DUMMY_STAB
// 在没有调通DS1302之前或者没有DS1302硬件的情况下可以放开上面这句 ############

#ifdef USE_DUMMY_STAB

	static idata uchar nian,yue,ri,shi,fen,miao;

	void DS1302_init()
	{
		nian = 19;
		yue = 9;
		ri = 18;
		
		shi = 23;
		fen = 59;
		miao = 50;
	}

	void DS1302_GetTimeFromDS1302()
	{
		miao++;
		if (miao==60){
			miao=0;fen++;
			if (fen==60){
				fen=0;shi++;
				if (shi==24){
					shi=0;ri++;
					if (ri==32){
						ri=1;yue++;
						if (yue==13){
							yue=1;nian++;
		}}}}}
	}

	void DS1302_WriteTime_Sec(uchar nonBcdData) {
		miao = nonBcdData;
	}

	void DS1302_WriteTime_Minute(uchar nonBcdData) {
		fen = nonBcdData;
	}

	void DS1302_WriteTime_Hour(uchar nonBcdData) {
		shi = nonBcdData;
	}

	void DS1302_WriteTime_Day(uchar nonBcdData) {
		ri = nonBcdData;
	}

	void DS1302_WriteTime_Month(uchar nonBcdData) {
		yue = nonBcdData;
	}

	void DS1302_WriteTime_Year(uchar nonBcdData) {
		nian = nonBcdData;
	}
#else
	sbit	SCLK_DS1302=P1^2;
	sbit	RST_DS1302=P1^0;
	sbit	IO_DS1302=P1^1;

	static uchar code DS1302_ID[]="DaNKer";

	//==============系统标志====================================================
	static idata uchar clock_ctr=0;
	#define        F_CTR_HR    0x80        //1=12小时制，0=24小时制
	#define        F_CTR_AP    0x20        //1=AM,0=PM
	#define        F_CTR_AL    0x01        //闹钟标志,为1时打开闹钟
	//=========================================================================
	static idata uchar  SEC_SCAN_MODE = 0;            //秒钟显示模式

	#define		ALARM_ADDR	0xD0		//闹钟时间存放地址
	#define		SecMod_WrADDR	0xD6
	#define		SecMod_ReADDR	0xD7

	static idata uchar alarm_shi;alarm_fen;
	static idata uchar nian,yue,ri,shi,fen,miao;

	// 十进制数转BCD码
	uchar dec2Bcd(uchar hexData){
		return (hexData/10)<<4|(hexData%10);
	}

	// BCD码转十进制数
	uchar bcd2Dec(uchar bcdData){
		return ((bcdData>>4)*10) + (bcdData&0x0f);
	}

	/************************DS1302**********************/
	void SendByte(unsigned char sdate)		  //单片机发送一位数据 
	{
		unsigned char i;
		for(i=0;i<8;i++)
		{
			if(sdate&0x01!=0)		//从低位开始发送 
			IO_DS1302=1;
			else IO_DS1302=0;
			SCLK_DS1302=0;
			SCLK_DS1302=1;			//时钟上升沿1302接收一位数据 
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
	//Author: ChenLing
	//Date:   2012.12.17
	//注意，本函数已经考虑过DS1302读取的是BCD码的问题，已经将BCD码转换成了十进制数字了
	//=========================================================================
	void DS1302_GetTimeFromDS1302()
	{
		static idata uchar i=0,temp1;
		//unsigned char time_H,time_L,temp;
		unsigned char temp;

		switch(i)
		{
			case 0:    temp=ReadTime(0x8D);        //年
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//nian=time_H+time_L;
				nian=bcd2Dec(temp);
				i++;
				break;
			case 1:    temp=ReadTime(0x89);        //月
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//yue=time_H+time_L;
				yue=bcd2Dec(temp);
				i++;
				break;
			case 2:    temp=ReadTime(0x87);        //日
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//ri=time_H+time_L;
				ri=bcd2Dec(temp);
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
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//shi=time_H+time_L;
				shi=bcd2Dec(temp);
				i++;
				break;
			case 4:    temp=ReadTime(0x83);        //分
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//fen=time_H+time_L;
				fen=bcd2Dec(temp);
				i++;
				break;
			case 5:    temp=ReadTime(0x81);        //秒
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//miao=time_H+time_L;
				miao=bcd2Dec(temp);
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
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//shi=time_H+time_L;
				shi=bcd2Dec(temp);
				i++;
				break;
			case 8:    temp=ReadTime(0x83);        //分
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//fen=time_H+time_L;
				fen=bcd2Dec(temp);
				i++;
				break;
			case 9:    temp=ReadTime(0x81);        //秒
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//miao=time_H+time_L;
				miao=bcd2Dec(temp);
				i++;
				break;
			case 10:
				i=0;
				break;
			default:i=0;break;
		}
	}

	// 以下函数考虑了ds1302写入时需要写入BCD码，自动转换成BCD码再写入
	// DS1302芯片写入时间的格式为BDC码，不能直接写原来的数字
	// 要将数字的十位放在高4位，个位放在低4位
	// 比如19这个数，要送十六进制数0x19，不能直接送十进制数19
	// 方法是：
	//      将19整除10的商，也就是十位数的1，左移4位移动到高4位上去(0x10)
	//      再计算19除以10的余数，也就是个位数的9(0x09)
	//      最后将两个数相或就可以了 0x10|0x09=0x19
	// 目前这个时钟的硬件没有显示星期的功能，所以星期部分的数据丢弃不用

	/*
	传入十进制数据，自动转成BCD并写入DS1302
	*/
	void DS1302_WriteTime_Sec(uchar nonBcdData) {
		WriteTime(0x80, dec2Bcd(nonBcdData));	//秒
	}

	/*
	传入十进制数据，自动转成BCD并写入DS1302
	*/
	void DS1302_WriteTime_Minute(uchar nonBcdData) {
		WriteTime(0x82, dec2Bcd(nonBcdData));  	//分
	}

	/*
	传入十进制数据，自动转成BCD并写入DS1302
	*/
	void DS1302_WriteTime_Hour(uchar nonBcdData) {
		WriteTime(0x84, dec2Bcd(nonBcdData));  //时
	}

	/*
	传入十进制数据，自动转成BCD并写入DS1302
	*/
	void DS1302_WriteTime_Day(uchar nonBcdData) {
		WriteTime(0x86, dec2Bcd(nonBcdData));  //日
	}

	/*
	传入十进制数据，自动转成BCD并写入DS1302
	*/
	void DS1302_WriteTime_Month(uchar nonBcdData) {
		WriteTime(0x88, dec2Bcd(nonBcdData));  //月
	}

	/*
	传入十进制数据，自动转成BCD并写入DS1302
	*/
	void DS1302_WriteTime_Year(uchar nonBcdData) {
		WriteTime(0x8c, dec2Bcd(nonBcdData));  //年
	}
#endif

uchar DS1302_GetYear(){
	return nian;
}

uchar DS1302_GetMonth(){
	return yue;
}

uchar DS1302_GetDay(){
	return ri;
}

uchar DS1302_GetHour(){
	return shi;
}

uchar DS1302_GetMinute(){
	return fen;
}

uchar DS1302_GetSecond(){
	return miao;
}