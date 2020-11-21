#include "../header/ds1302.h"
#include "../header/common.h"

// ��û�е�ͨDS1302֮ǰ����û��DS1302Ӳ��������¿��Էſ�������� ############
// ����ӿڲ����������ṩ���١���DS1302��Ҳ�����ڵ��Գ����ã������޸���ʵʱ��
//#define USE_DUMMY_STAB
// ��û�е�ͨDS1302֮ǰ����û��DS1302Ӳ��������¿��Էſ�������� ############

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

	// �״�ʹ��1302���г�ʼ��������1302�ڲ�RAMд���ʶ�ַ�����
	// ÿ�������ϵ綼��������ڴ��������������ʶ�ַ��������û�����ʶ1302���û���˻������״��ϵ硣
	// ����1302�ڲ�RAM����31���ֽڣ���C0(C1)-FC(FD)�� (ǰ����д��ַ���������Ƕ���ַ)
	// ��������������ʹ����E0֮���RAM�����ԣ���ʶ�ַ�����Ҫ����8���ַ���
	#define ADDR_DS1302_ID_R 0xC1
	#define ADDR_DS1302_ID_W 0xC0
	static uchar code DS1302_ID[]="Mango";

	//==============ϵͳ��־====================================================
	static idata uchar clock_ctr=0;
	#define        F_CTR_HR    0x80        //1=12Сʱ�ƣ�0=24Сʱ��
	#define        F_CTR_AP    0x20        //1=AM,0=PM
	#define        F_CTR_AL    0x01        //���ӱ�־,Ϊ1ʱ������
	//=========================================================================
	static idata uchar  SEC_SCAN_MODE = 0;            //������ʾģʽ

	#define		ALARM_ADDR	0xD0		//����ʱ���ŵ�ַ
	#define		SecMod_WrADDR	0xD6
	#define		SecMod_ReADDR	0xD7

	static idata uchar alarm_shi;alarm_fen;
	static idata uchar nian,yue,ri,shi,fen,miao;

	// ����ʱ��==================================
	#define ADDR_COUNTDOWN_MINUTE_W   0xE0  // �ϴε���ʱ�ķ������ݣ�DS1302�ڲ�RAM�ĵ�ַ
	#define ADDR_COUNTDOWN_MINUTE_R   0xE1
	static idata char countdownSec;
	static idata char countdownMinute;
	static idata char countdownLastSec;
	// ����ʱ��==================================

	// ʮ������תBCD��
	uchar dec2Bcd(uchar hexData){
		return (hexData/10)<<4|(hexData%10);
	}

	// BCD��תʮ������
	uchar bcd2Dec(uchar bcdData){
		return ((bcdData>>4)*10) + (bcdData&0x0f);
	}

	/************************DS1302**********************/
	void SendByte(unsigned char sdate)		  //��Ƭ������һλ���� 
	{
		unsigned char i;
		for(i=0;i<8;i++)
		{
			if(sdate&0x01!=0)		//�ӵ�λ��ʼ���� 
			IO_DS1302=1;
			else IO_DS1302=0;
			SCLK_DS1302=0;
			SCLK_DS1302=1;			//ʱ��������1302����һλ���� 
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
			SCLK_DS1302=0;		  //ʱ���½���1302����һλ���� 
		}
		SCLK_DS1302=0;
		return(tdate);
	}

	//��ĳ��ַ����һ���ֽ�����
	void WriteByte(unsigned char add,unsigned char sdate)
	{
		RST_DS1302=0;		//��λ��Ϊ0����ֹ�������ݴ��� 
		SCLK_DS1302=0;		//ʱ�ӽ�Ϊ0ʱ��λ�Ų��ܱ����� 
		RST_DS1302=1;							 
		SendByte(add);		//�ȷ��͵�ַ 
		SendByte(sdate);	//�ٷ�������  
		SCLK_DS1302=0;
		RST_DS1302=0;
	}

	//��ĳ��ַ��ȡһ���ֽ�����
	unsigned char ReadByte(unsigned char add)
	{
		unsigned char tdate;
		RST_DS1302=0;
		SCLK_DS1302=0;
		RST_DS1302=1;
		SendByte(add);		//�ȷ��͵�ַ
		tdate=InptByte();	//�ٶ�����
		SCLK_DS1302=1;
		RST_DS1302=0;
		return(tdate);
	}

	//��1302�������õ�ʱ��
	void WriteTime(unsigned char add,unsigned char tim)		 
	{
		WriteByte(0x8e,0x00);		//���д���� 
		WriteByte(add,tim);			//��д��ַ����дʱ�����ݣ�ʱ�����ݷ��������� 
		WriteByte(0x8e,0x80);		//�ָ�д���������λΪ1���ɣ�
	}

	//��1302��ʱ��ֵ
	unsigned char ReadTime(unsigned char add)
	{
		return ReadByte(add);
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

		// ��ʼ����������Ϊ61��������ϵ������㱨ʱ
		fen = 61;
		
		while((*strp)&&(wrong_x<5))
		{
			temp=ReadTime(ADDR_DS1302_ID_R+(i<<1));
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
			// ����һ�´����ڲ�RAM�Ĺ����ǲ�������
			// �����д��һ���ֽڣ�����0xAA
			WriteTime(ADDR_DS1302_ID_W,0xaa);
			delay_ms(10);
			// Ȼ���ȡ���������ǲ���д���0xAA
			temp=ReadTime(ADDR_DS1302_ID_R);
			if(temp!=0xaa)
			{
				while(1) // �������ڵ��Խ׶�Ӧ��ע�͵��������ⲿû��DS1302Ӳ����ʱ���һֱͣ������
				{
					//TODO �������д����������ʾû�����DS1302����Ӳ�������⡣���Խ��б�����
				}		
			}
			WriteTime(0x80,0x00);  //��
			WriteTime(0x82,0x00);  //��
			WriteTime(0x84,0x12);  //ʱ
			WriteTime(0x86,0x01);  //��
			WriteTime(0x88,0x01);  //��
			WriteTime(0x8c,0x12);  //��
			
			clock_ctr=0;		//��ʹ��ʱ����ƣ�12/24Сʱ�ƣ�AM/PM������ʹ��
			alarm_shi=12;
			alarm_fen=0;		//��ʹ������ʱ��
			WriteTime(ALARM_ADDR,clock_ctr);
			WriteTime(ALARM_ADDR+2,alarm_shi);
			WriteTime(ALARM_ADDR+4,alarm_fen);
			WriteTime(SecMod_WrADDR,0x00);			//������ʾģʽ

			// д���ʶ�ַ������Ա�����ʱ���1302��û�н��й���ʼ��
			strp=DS1302_ID;
			while(*strp)
			{
				WriteTime(ADDR_DS1302_ID_W+(i<<1),*strp);
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

	// ����ʱ��������Ҫ����ͣ����
	void doCountDown(){

		if (countdownMinute == 0 && countdownSec == 0)
		{
			// �Ѿ�����ʱ������ʲô������
		}
		
		if (miao != countdownLastSec)
		{
			countdownLastSec = miao;
			countdownSec--;
			if (countdownSec < 0)
			{
				if (countdownMinute > 0)
				{
					// ���ӻ�û��0�����ӱ�59�����Ӽ�һ
					countdownSec = 59;
					countdownMinute--;
				} else {
					// �����Ѿ�����0�ˣ������ӱ���0
					countdownSec = 0;
				}
			}
		}
	}
	//=========================================================================
	//        ��ʱ��
	//out:    nian,yue,ri,shi,fen,miao
	//Author: ChenLing
	//Date:   2012.12.17
	//ע�⣬�������Ѿ����ǹ�DS1302��ȡ����BCD������⣬�Ѿ���BCD��ת������ʮ����������
	//=========================================================================
	void DS1302_GetTimeFromDS1302()
	{
		static idata uchar i=0,temp1;
		//unsigned char time_H,time_L,temp;
		unsigned char temp;

		//���е���ʱ
		doCountDown();

		switch(i)
		{
			case 0:    temp=ReadTime(0x8D);        //��
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//nian=time_H+time_L;
				nian=bcd2Dec(temp);
				i++;
				break;
			case 1:    temp=ReadTime(0x89);        //��
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//yue=time_H+time_L;
				yue=bcd2Dec(temp);
				i++;
				break;
			case 2:    temp=ReadTime(0x87);        //��
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//ri=time_H+time_L;
				ri=bcd2Dec(temp);
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
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//shi=time_H+time_L;
				shi=bcd2Dec(temp);
				i++;
				break;
			case 4:    temp=ReadTime(0x83);        //��
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//fen=time_H+time_L;
				fen=bcd2Dec(temp);
				i++;
				break;
			case 5:    temp=ReadTime(0x81);        //��
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
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//shi=time_H+time_L;
				shi=bcd2Dec(temp);
				i++;
				break;
			case 8:    temp=ReadTime(0x83);        //��
				//time_H=(temp>>4)*10;time_L=temp&0x0f;
				//fen=time_H+time_L;
				fen=bcd2Dec(temp);
				i++;
				break;
			case 9:    temp=ReadTime(0x81);        //��
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

	// ���º���������ds1302д��ʱ��Ҫд��BCD�룬�Զ�ת����BCD����д��
	// DS1302оƬд��ʱ��ĸ�ʽΪBDC�룬����ֱ��дԭ��������
	// Ҫ�����ֵ�ʮλ���ڸ�4λ����λ���ڵ�4λ
	// ����19�������Ҫ��ʮ��������0x19������ֱ����ʮ������19
	// �����ǣ�
	//      ��19����10���̣�Ҳ����ʮλ����1������4λ�ƶ�����4λ��ȥ(0x10)
	//      �ټ���19����10��������Ҳ���Ǹ�λ����9(0x09)
	//      ������������Ϳ����� 0x10|0x09=0x19
	// Ŀǰ���ʱ�ӵ�Ӳ��û����ʾ���ڵĹ��ܣ��������ڲ��ֵ����ݶ�������

	/*
	����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
	*/
	void DS1302_WriteTime_Sec(uchar nonBcdData) {
		WriteTime(0x80, dec2Bcd(nonBcdData));	//��
	}

	/*
	����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
	*/
	void DS1302_WriteTime_Minute(uchar nonBcdData) {
		WriteTime(0x82, dec2Bcd(nonBcdData));  	//��
	}

	/*
	����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
	*/
	void DS1302_WriteTime_Hour(uchar nonBcdData) {
		WriteTime(0x84, dec2Bcd(nonBcdData));  //ʱ
	}

	/*
	����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
	*/
	void DS1302_WriteTime_Day(uchar nonBcdData) {
		WriteTime(0x86, dec2Bcd(nonBcdData));  //��
	}

	/*
	����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
	*/
	void DS1302_WriteTime_Month(uchar nonBcdData) {
		WriteTime(0x88, dec2Bcd(nonBcdData));  //��
	}

	/*
	����ʮ�������ݣ��Զ�ת��BCD��д��DS1302
	*/
	void DS1302_WriteTime_Year(uchar nonBcdData) {
		WriteTime(0x8c, dec2Bcd(nonBcdData));  //��
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

// ���õ���ʱ������ʼ����ʱ
void DS1302_StartCountDown(uchar cdMinute){
	countdownLastSec = miao;
	countdownMinute = cdMinute;
	countdownSec = 0;
	
	WriteTime(ADDR_COUNTDOWN_MINUTE_W, cdMinute);
}

// ȡ��������õĵ���ʱ�ķ���
uchar DS1302_GetLastCdMinute(){
	return ReadTime(ADDR_COUNTDOWN_MINUTE_R);
}

// ����ʱ��
uchar DS1302_GetCdSecond(){
	return countdownSec;
}

// ����ʱ��
uchar DS1302_GetCdMinute(){
	return countdownMinute;
}
