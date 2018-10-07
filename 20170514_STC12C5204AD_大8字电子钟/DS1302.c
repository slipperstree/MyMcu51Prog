#include <DS1302.h>
#include <stdlib.h>

#define uchar unsigned char
#define uint  unsigned int

sbit ds1302_reset = P3^4;
sbit ds1302_sclk  = P1^7;
sbit ds1302_io    = P1^6;

uchar ds1302_disp_buf[20] ={0x00}; //定义显示缓冲区
uchar ds1302_temp [2]={0};			//用来存放设置时的小时、分钟的中间值

uchar ds1302_sec,ds1302_min,ds1302_hour,ds1302_ri,ds1302_yue,ds1302_nian;					//定义秒、分和小时变量

/********以下是写一字节数据函数********/
void write_byte(uchar inbyte)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		ds1302_sclk=0;             //写时低电平改变数据

		if(inbyte&0x01)
			ds1302_io=1;
		else
			ds1302_io=0;

		ds1302_sclk=1;            //高电平把数据写入DS1302

		inbyte=inbyte>>1;
	}
}

/********以下是读一字节数据函数********/
uchar read_byte()    
{
	uchar i,ds1302_temp=0; 
	ds1302_io=1;
	for(i=0;i<7;i++)
	{
		ds1302_sclk=0;
		if(ds1302_io==1)
			ds1302_temp=ds1302_temp|0x80;
		else
			ds1302_temp=ds1302_temp&0x7f;

		ds1302_sclk=1;             //产生下跳沿
		ds1302_temp=ds1302_temp>>1;
	}

	ds1302_io=0;
	return (ds1302_temp);
}

/********写DS1302函数, 往DS1302的某个地址写入数据 ********/
void write_ds1302(uchar cmd,uchar indata)
{
	ds1302_sclk=0;
	ds1302_reset=1;
	write_byte(cmd);        
	write_byte(indata);
	ds1302_sclk=0;
	ds1302_reset=0;
}

/********读DS1302函数,读DS1302某地址的的数据********/
uchar read_ds1302(uchar addr)
{
	uchar backdata;
	ds1302_sclk=0;
	ds1302_reset=1;
	write_byte(addr);      		//先写地址
	backdata=read_byte();  		//然后读数据
	ds1302_sclk=0;
	ds1302_reset=0;
	return (backdata);
}

// 将普通数字转成DS1302各个寄存器需要的格式
// 十位数放在高4位上，个位数放在低4位上
uchar dec2DS1302Format(uchar dec){
	return (dec/10)*16 + (dec%10);
}

/********初始化DS1302函数********/
void ds1302_init(uchar pYear, uchar pMonth, uchar pDay, 
				 uchar pWeek,
				 uchar pHour, uchar pMin, uchar pSec)
{
	ds1302_reset=0;
	ds1302_sclk=0;

	write_ds1302(0x90,0xab); 	//写充电器

	//以下写入初始化时间 日期

	//年寄存器：BIT7:4=年的十位数，BIT3:0=年的个位数
	write_ds1302(0x8c, dec2DS1302Format(pYear)); 		// 年(20xx的xx部分)）（00-99）

	//月寄存器：BIT7:5=0，BIT4=月的十位数，BIT3:0=月的个位数
	write_ds1302(0x88, dec2DS1302Format(pMonth)); 	// 月

	//日寄存器：BIT7:6=0，BIT5:4=日的十位数，BIT3:0=日的个位数
	write_ds1302(0x86, dec2DS1302Format(pDay)); 		// 日

	//星期寄存器：BIT7:3=0，BIT2:0=星期（1-7）
	write_ds1302(0x8a, dec2DS1302Format(pWeek));		// 星期

	//时寄存器：BIT7=24小时制(0)/12小时制(1)
	//			BIT6=0
	//			12小时制：BIT5=AM(0)/PM(1)，BIT4=时的十位数
	//			24小时制：BIT5:4=时的十位数
	//			BIT3:0=时的个位数
	write_ds1302(0x84, dec2DS1302Format(pHour)); 		// 时

	//分寄存器：BIT7=NA，BIT6:4=分的十位数，BIT3:0=分的个位数
	write_ds1302(0x82, dec2DS1302Format(pMin)); 		// 分

	//秒寄存器：BIT7=CH，BIT6:4=秒的十位数，BIT3:0=秒的个位数
	write_ds1302(0x80, dec2DS1302Format(pSec)); 		// 秒

	write_ds1302(0x8e,0x80); 	//写保护控制字，禁止写
}

void ds1302_get_time()
{
	write_ds1302(0x8e,0x00);		//控制命令,WP=0,允许写操作   
	write_ds1302(0x90,0xab);		//涓流充电控制

	ds1302_sec=read_ds1302(0x81);			//读取秒寄存器：BIT7=CH，BIT6:4=秒的十位数，BIT3:0=秒的个位数
	ds1302_min=read_ds1302(0x83);			//读取分寄存器：BIT7=NA，BIT6:4=分的十位数，BIT3:0=分的个位数
	ds1302_hour=read_ds1302(0x85);			//读取时寄存器：BIT7=24小时制(0)/12小时制(1)
									//				BIT6=0
									//				12小时制：BIT5=AM(0)/PM(1)，BIT4=时的十位数
									//				24小时制：BIT5:4=时的十位数
									//				BIT3:0=时的个位数
	ds1302_ri=read_ds1302(0x87);			//读取日寄存器：BIT7:6=0，BIT5:4=日的十位数，BIT3:0=日的个位数
	ds1302_yue=read_ds1302(0x89);			//读取月寄存器：BIT7:5=0，BIT4=月的十位数，BIT3:0=月的个位数
	ds1302_nian=read_ds1302(0x8D);			//读取年寄存器：BIT7:4=年的十位数，BIT3:0=年的个位数

	// 根据上面每个寄存器的描述，转换成最终的有效数据
	ds1302_sec=(ds1302_sec&0x7F)/16*10+(ds1302_sec&0x7F)%16;
	ds1302_min=(ds1302_min&0x7F)/16*10+(ds1302_min&0x7F)%16;

	if ((ds1302_hour&0x80)==0x80)
	{
		// 12小时制
		ds1302_hour=(ds1302_hour&0x1F)/16*10+(ds1302_hour&0x1F)%16;
	} else {
		// 24小时制
		ds1302_hour=(ds1302_hour&0x3F)/16*10+(ds1302_hour&0x3F)%16;
	}

	ds1302_ri=ds1302_ri/16*10+ds1302_ri%16;
	ds1302_yue=ds1302_yue/16*10+ds1302_yue%16;
	ds1302_nian=ds1302_nian/16*10+ds1302_nian%16;

	/*
	time_buf[0]=ds1302_sec/16*10+ds1302_sec%16;		//将读取到的16进制数转化为10进制
	time_buf[1]=ds1302_min/16*10+ds1302_min%16;		//将读取到的16进制数转化为10进制
	time_buf[2]=ds1302_hour/16*10+ds1302_hour%16;		//将读取到的16进制数转化为10进制
	time_buf[3]=ds1302_ri/16*10+ds1302_ri%16;			//将读取到的16进制数转化为10进制
	time_buf[4]=ds1302_yue/16*10+ds1302_yue%16;		//将读取到的16进制数转化为10进制
	time_buf[6]=ds1302_nian/16*10+ds1302_nian%16;		//将读取到的16进制数转化为10进制
	*/
}

/********以下是转换函数,负责将走时数据转换为适合LCD显示的数据********/
uchar* ds1302_getStirngYYYYMMDDHHMMSS()
{
	uchar idx = 0;

	ds1302_disp_buf[idx++]=2+0x30;    		//年千位数据
	ds1302_disp_buf[idx++]=0+0x30;			//年百位数据
	ds1302_disp_buf[idx++]=ds1302_nian/10+0x30;   //年十位数据
	ds1302_disp_buf[idx++]=ds1302_nian%10+0x30;	//年时个位数据
	ds1302_disp_buf[idx++]='/';
	ds1302_disp_buf[idx++]=ds1302_yue/10+0x30;    //月十位数据
	ds1302_disp_buf[idx++]=ds1302_yue%10+0x30;	//月个位数据
	ds1302_disp_buf[idx++]='/';
	ds1302_disp_buf[idx++]=ds1302_ri/10+0x30;    	//日十位数据
	ds1302_disp_buf[idx++]=ds1302_ri%10+0x30;		//日个位数据
	ds1302_disp_buf[idx++]=' ';
	ds1302_disp_buf[idx++]=ds1302_hour/10+0x30;   //小时十位数据
	ds1302_disp_buf[idx++]=ds1302_hour%10+0x30;	//小时个位数据
	ds1302_disp_buf[idx++]=':';
	ds1302_disp_buf[idx++]=ds1302_min/10+0x30;    //分钟十位数据
	ds1302_disp_buf[idx++]=ds1302_min%10+0x30;	//分钟个位数据
	ds1302_disp_buf[idx++]=':';
	ds1302_disp_buf[idx++]=ds1302_sec/10+0x30;    //秒十位数据
	ds1302_disp_buf[idx++]=ds1302_sec%10+0x30;	//秒个位数据

	ds1302_disp_buf[idx++]=0x00;

	return ds1302_disp_buf;
}


uchar ds1302_getYear(){
	return ds1302_nian;
}

uchar ds1302_getMonth(){
	return ds1302_yue;
}

uchar ds1302_getDay(){
	return ds1302_ri;
}

uchar ds1302_getHour(){
	return ds1302_hour;
}

uchar ds1302_getMin(){
	return ds1302_min;
}

uchar ds1302_getSec(){
	return ds1302_sec;
}

