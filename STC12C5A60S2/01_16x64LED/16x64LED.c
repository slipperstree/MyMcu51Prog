
/*******************************************
程序名称：4个点阵模块显示汉字“非常感谢”
外部参数说明：
单片机选型：本程序必须使用RAM为128以上的单片机，比如STC89C52 STC89C**,STC90C*等12T的单片机
            也可以使用STC12C5A*,STC11F*等1T的单片机，可以使用AT89C51 AT89C52 AT89S51 
            AT89S52等RAM只有128的单片机。

晶振参数：使用12M以上的晶体。建议使用24M以上的晶体，晶振频率越高，显示字不会出现闪烁感。
          如果使用24M以下的晶体，如果是STC的芯片，建议打开6T(双倍速)功能。
          
端口定义：模块的A插针接单片机的P1.4，模块的B插针接单片机的P1.5，模块的C插针接单片机的P1.6
          模块的D插针接单片机的P1.7，模块的LATCH插针接单片机的P1.0
		  模块的CLK插针接单片机的P1.1，模块的DI(程序里面标的是DA_port)插针接单片机的P1.2
		  模块的G(程序里面标的是EN_port)插针接单片机的P1.3
*/

/*头文件*/
#include <STC12C5A60S2.h>      
#include <intrins.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char
#define NOP() _nop_()

/*端口定义*/

sbit EN_port = P1^3;
sbit DA_in_port = P1^2;
sbit CLK_port = P1^1;
sbit Latch_port = P1^0;

// for test led
sbit LED = P3^7;

// 串口操作 --------------------------------------------------------------------------------
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

//sbit bit9 = P2^2;           //P2.2 show UART data bit9
bit busy;

uchar rec[10]={0,0,0,0,0,0,0,0,0,0};
uchar recIdx=0;
// 串口操作 --------------------------------------------------------------------------------

#define ABCD_port P1  //HC138 ABCD端口定义
//sbit D_port = P1^7;
//sbit C_port = P1^6;
//sbit B_port = P1^5;
//sbit A_port = P1^4;

// GT20L16S1Y 接口 ------------------------------------------------
sbit pCE        =P2^0;         //
sbit pSCLK      =P2^1;         //
sbit pSI        =P2^2;         //
sbit pSO        =P2^3;         //

unsigned long addr;
int idx = 0;
uchar data ic_data[32];

uchar* getICData_ASCII_8x16(uchar ch);
uchar* getICData_Hanzi_16x16(uchar* hz);
void readICDataToBuffer(uchar bufNum, uchar* str);

// GT20L16S1Y 接口 ------------------------------------------------

void HC595_Data_Send(uchar *p, uchar han, uchar zishu, uchar offset);
void setICDataToBuffer(uchar nowBufferNum, uchar *pICData, uchar size, uchar pos);

// 保存16X64点阵信息（128+32字节）（五个汉字，多一个汉字是为了移位显示用的缓冲）
uchar xdata bufHZ[1][128+32] = {
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
	// {
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	// }
};

uchar code ic_data_A[] = {0x00,0xE0,0x9C,0x82,0x9C,0xE0,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x0F,0x00};

uchar nowOffset=0;
int nowPos=0;

// int showDataSize=960*2;
uchar code showData[] = "汉皇重色思倾国，御宇多年求不得。杨家有女初长成，养在深闺人未识。天生丽质难自弃，一朝选在君王侧。回眸一笑百媚生，六宫粉黛无颜色。春寒赐浴华清池，温泉水滑洗凝脂。侍儿扶起娇无力，始是新承恩泽时。云鬓花颜金步摇，芙蓉帐暖度春宵。春宵苦短日高起，从此君王不早朝。承欢侍宴无闲暇，春从春游夜专夜。后宫佳丽三千人，三千宠爱在一身。金屋妆成娇侍夜，玉楼宴罢醉和春。姊妹弟兄皆列土，可怜光彩生门户。遂令天下父母心，不重生男重生女。骊宫高处入青云，仙乐风飘处处闻。缓歌慢舞凝丝竹，尽日君王看不足。渔阳鼙鼓动地来，惊破霓裳羽衣曲。九重城阙烟尘生，千乘万骑西南行。翠华摇摇行复止，西出都门百余里。六军不发无奈何，宛转蛾眉马前死。花钿委地无人收，翠翘金雀玉搔头。君王掩面救不得，回看血泪相和流。黄埃散漫风萧索，云栈萦纡登剑阁。峨嵋山下少人行，旌旗无光日色薄。蜀江水碧蜀山青，圣主朝朝暮暮情。行宫见月伤心色，夜雨闻铃肠断声。天旋日转回龙驭，到此踌躇不能去。马嵬坡下泥土中，不见玉颜空死处。君臣相顾尽沾衣，东望都门信马归。归来池苑皆依旧，太液芙蓉未央柳。芙蓉如面柳如眉，对此如何不泪垂。春风桃李花开夜，秋雨梧桐叶落时。西宫南内多秋草，落叶满阶红不扫。梨园弟子白发新，椒房阿监青娥老。夕殿萤飞思悄然，孤灯挑尽未成眠。迟迟钟鼓初长夜，耿耿星河欲曙天。鸳鸯瓦冷霜华重，翡翠衾寒谁与共。悠悠生死别经年，魂魄不曾来入梦。临邛道士鸿都客，能以精诚致魂魄。为感君王展转思，遂教方士殷勤觅。排空驭气奔如电，升天入地求之遍。上穷碧落下黄泉，两处茫茫皆不见。忽闻海上有仙山，山在虚无缥渺间。楼阁玲珑五云起，其中绰约多仙子。中有一人字太真，雪肤花貌参差是。金阙西厢叩玉扃，转教小玉报双成。闻到汉家天子使，九华帐里梦魂惊。揽衣推枕起徘回，珠箔银屏逦迤开。云鬓半偏新睡觉，花冠不整下堂来。风吹仙袂飘摇举，犹似霓裳羽衣舞。玉容寂寞泪阑干，梨花一枝春带雨。含情凝睇谢君王，一别音容两渺茫。昭阳殿里恩爱绝，蓬莱宫中日月长。回头下望人寰处，不见长安见尘雾。唯将旧物表深情，钿合金钗寄将去。钗留一股合一扇，钗擘黄金合分钿。但教心似金钿坚，天上人间会相见。临别殷勤重寄词，词中有誓两心知。七月七日长生殿，夜半无人私语时。在天愿作比翼鸟，在地愿为连理枝。天长地久有时尽，此恨绵绵无绝期。";
int showDataSize=297*2;
//uchar code showData[] = "我们好像在哪儿见过你记得吗？好像那是一个春天我刚发芽，我走过，没有回头，我记得，我快忘了。我们好像在哪儿见过你记得吗？记得那是一个夏天盛开如花，我唱歌，没有对我，但我记得，可我快忘了。我们好像在哪见过你记得吗？好像那是一个秋天夕阳西下，你美得让我不敢和你说话，你经过我时风起浮动我的发。我们好像在哪见过你记得吗？记得那是一个冬天漫天雪花，我走过，没有回头，我记得，我快忘了。我们好像在哪见过你记得吗？那时你还是个孩子我在窗棂下，我猜着你的名字刻在了墙上，我画了你的摸样对着弯月亮我们好像在哪见过你记得吗？当我们来到今生各自天涯，天涯相望今生面对谁曾想，还能相遇一切就像梦一样。我们好像在哪见过。";

// 取模软件，需要指定【阴码+顺向（高位在前）+行列式】的形式
// 不使用字库芯片而使用固定文字时放开这一段，并修改程序里的 bufHZ 为 ziku_table
// uchar code ziku_table[]={
// 0x00,0x30,0xC0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x60,0x10,/*"[",0*/

// 0x00,0x78,0x48,0x57,0x50,0x61,0x51,0x4A,0x4B,0x48,0x69,0x51,0x42,0x44,0x40,0x40,
// 0x40,0x40,0x40,0xFE,0x80,0x20,0x20,0x20,0xFC,0x20,0x28,0x24,0x22,0x22,0xA0,0x40,/*"陈",1*/

// 0x00,0xFF,0x01,0x01,0x01,0x7F,0x41,0x41,0x49,0x45,0x41,0x49,0x45,0x41,0x41,0x40,
// 0x00,0xFE,0x00,0x00,0x00,0xFC,0x04,0x04,0x44,0x24,0x04,0x44,0x24,0x04,0x14,0x08,/*"雨",2*/

// 0x00,0xFF,0x02,0x7A,0x4A,0x7A,0x00,0xFF,0x02,0x7A,0x4A,0x4A,0x7A,0x02,0x0A,0x04,
// 0x20,0x20,0x20,0x7E,0x42,0x84,0x10,0x10,0x10,0x10,0x28,0x28,0x28,0x44,0x44,0x82,/*"歌",3*/

// 0x00,0xC0,0x30,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x20,0xC0,/*"]",4*/
// };

// 一次性传给HC595用的64+16位的一整行数据（包括移位用的第五个汉字的缓冲区）
uchar data disp_buff[10];

int tt = 0;
int tt1 = 0;
void Timer0Init(void)		//20毫秒@30.000MHz
{
	EA=1;
    ET0=1;
	
	//AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0xB0;		//设置定时初值
	TH0 = 0x3C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

uchar rowIdx=0; // 第几行
uchar nowBufferNum=0;
void display(){
	uchar i;
	
	// 循环从大buffer中取当前行从左到右的全部8个字节作为一整行数据保存在显示用的buff中
	for (i=0;i<10;i++){
		// 下面是大buffer的示意图，每次需要取一行的数据
		// ——————————————————————————————————————————————————————————
		// B0	B16		B32		B48		B64		B80		B96		B112   --> 一整行数据传给595输出
		// ——————————————————————————————————————————————————————————
		// B1	B17		.		.		.       .       .       .
		// ——————————————————————————————————————————————————————————
		// B2	B18		.		.		.       .       .       .
		// ——————————————————————————————————————————————————————————
		// .	.		.       .       .       .       .       .
		// .	.		.       .       .       .       .       .
		// .	.		.       .       .       .       .       .
		// ——————————————————————————————————————————————————————————
		// B15	B31		B47		B63		B79		B95		B111	B127
		// ——————————————————————————————————————————————————————————
		disp_buff[i]=bufHZ[nowBufferNum][i*16 + rowIdx];
	}
	
	// 将上面取到的一整行数据串行输出给74HC595阵列，点亮当前行的数据
	HC595_Data_Send(disp_buff,rowIdx,4,nowOffset);
	
	// 当前行数加1，继续下一行的扫描
	rowIdx++;
	
	// 扫描完16行，回到第一行继续行扫描
	if(rowIdx==16) rowIdx=0;
}

void main()
{
	UartInit();
	
	Timer0Init();
	
	// For test --------------------------------------------------------------------------------
	//readICDataToBuffer(nowBufferNum, getICData_Hanzi_16x16("全"), 32, 0);
	readICDataToBuffer(nowBufferNum, showData);
	//readICDataToBuffer(nowBufferNum,"汉汉皇皇");
	
	//setICDataToBuffer(nowBufferNum, getICData_ASCII_8x16('A'), 16, 0);
	//setICDataToBuffer(nowBufferNum, ic_data_A, 16, 0);
	// For test --------------------------------------------------------------------------------

	while (1){
		display();
		
		if(recIdx >= 8){
			readICDataToBuffer(nowBufferNum,rec);
			recIdx=0;
		}
	}
}

// 将接收到的IC点阵数据存入缓冲区用于显示
// pICData	- 接收到的点阵数组
// size		- 点阵数组的大小。通常汉字为32字节，半角ASCII码为16字节。
// pos		- 指定该点阵数据显示的开始位置(0-7)，以8位（列）为一个单位。会覆盖原来的数据。
void setICDataToBuffer(uchar bufNum, uchar *pICData, uchar size, uchar pos)
{
	// ############# 从字库芯片GT20L16S1Y取出的点阵信息是竖置横列模式（例：字母A） ############# 
	// 从字库芯片GT20L16S1Y取出的点阵信息是竖置横列模式，也就是以竖排为单位取到的数据
	// ---- 下面从芯片取到的字母A的点阵数据 竖置横排的 ---- 
	// B0	B1	B2	B3	B4	B5	B6	B7
	// 0	0	0	0	0	0	0	0
	// 0	0	0	●	0	0	0	0
	// 0	0	●	0	●	0	0	0
	// 0	0	●	0	●	0	0	0
	// 0	0	●	0	●	0	0	0
	// 0	●	0	0	0	●	0	0
	// 0	●	0	0	0	●	0	0
	// 0	●	●	●	●	●	0	0
	// B8	B9	B10	B11	B12	B13	B14	B15
	// ●	0	0	0	0	0	●	0
	// ●	0	0	0	0	0	●	0
	// ●	0	0	0	0	0	●	0
	// ●	0	0	0	0	0	●	0
	// 0	0	0	0	0	0	0	0
	// 0	0	0	0	0	0	0	0
	// 0	0	0	0	0	0	0	0
	// 0	0	0	0	0	0	0	0
	// 而手头的这款点阵屏的驱动方式是行驱动，也就是每次扫描时需要传入一整行的点阵信息
	// 所以需要把每次取到的点阵信息转存到横置竖排（也就是汉字取模软件中的“行列式”）形式的buf中去，
	// 显示时从buf中取连续的一整行的8个字节的数据串行传入595用于扫描显示
	// 而存放的坐标跟这个字所在的显示位置有关系，比如我们打算将上面这个A的点阵显示在屏幕的最左边
	// 那么，转存以后应该是下面这个样子（右侧的数据省略）
	// 也就是说要将取到的B0-B7的最高位组合成buf里的一个字节B0（字符A为例就是0x00）
	// 将取到的B0-B7的次高位组合成buf里的字节B1（字符A为例就是0x10）
	// B0	0	0	0	0	0	0	0	0
	// B1	0	0	0	●	0	0	0	0
	// B2	0	0	●	0	●	0	0	0
	// B3	0	0	●	0	●	0	0	0
	// B4	0	0	●	0	●	0	0	0
	// B5	0	●	0	0	0	●	0	0
	// B6	0	●	0	0	0	●	0	0
	// B7	0	●	●	●	●	●	0	0
	// B8	●	0	0	0	0	0	●	0
	// B9	●	0	0	0	0	0	●	0
	// B10	●	0	0	0	0	0	●	0
	// B11	●	0	0	0	0	0	●	0
	// B12	0	0	0	0	0	0	0	0
	// B13	0	0	0	0	0	0	0	0
	// B14	0	0	0	0	0	0	0	0
	// B15	0	0	0	0	0	0	0	0

	uchar bufByteIdx = 0;
	uchar icByteIdx = 0;
	
	uchar* pTmpByteBufHZ;
	
	//clear target data
	//memset(&bufHZ[nowBufferNum][pos*16], size, 0x00);
	
	switch(size){
		case 32:
			// 汉字
			// 汉字的IC点阵数据16列X16行: 
			// B0-B7 为POS+0区域的上8行数据，B16-B23为POS+0区域的下8行数据
			// B8-B15为POS+1区域的上8行数据，B24-B31为POS+1区域的下8行数据
			// 所以大buff的B0-B7   的数据从IC数据的B0-B7   中取出来（行列互换）
			//     大buff的B8-B15  的数据从IC数据的B16-B23 中取出来（行列互换）
			//     大buff的B16-B23 的数据从IC数据的B8-B15  中取出来（行列互换）
			//     大buff的B24-B31 的数据从IC数据的B24-B31 中取出来（行列互换）
			for (bufByteIdx=0; bufByteIdx<size; bufByteIdx++){
				pTmpByteBufHZ = &(bufHZ[bufNum][pos*16 + bufByteIdx]);
				display();//test
				// 先清空目标字节
				pTmpByteBufHZ[0] = 0x00;
				
				if(bufByteIdx < 8){
					for (icByteIdx=0; icByteIdx<8; icByteIdx++){
						// 每次给最低位赋值后左移一位，最后得到8位数据
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							// 如果ic的该位数据是1，则给缓冲字节最低位赋值1
							pTmpByteBufHZ[0] |= 0x01;
						} else {
							// 否则赋值0
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
							// 否则赋值0
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
							// 否则赋值0
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
							// 否则赋值0
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
			}
			break;
		case 16:
			// 半角ACSII符号
			// 半角字符的IC点阵数据8列X16行: B0-B7为点阵的上8行数据，B8-B15为点阵的下8行数据（见上面的字符A的例子）
			// 所以大buff的B0-B7 的数据从IC数据的B0-B7 中取出来（行列互换）
			//   而大buff的B8-B15的数据从IC数据的B8-B15中取出来（行列互换）
			for (bufByteIdx=0; bufByteIdx<size; bufByteIdx++){
				//display();//test
					
				pTmpByteBufHZ = &(bufHZ[bufNum][pos*16 + bufByteIdx]);
				
				// 先清空目标字节
				pTmpByteBufHZ[0] = 0x00;
				
				if(bufByteIdx < 8){
					for (icByteIdx=0; icByteIdx<8; icByteIdx++){
						// 每次给最低位赋值后左移一位，最后得到8位数据
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							// 如果ic的该位数据是1，则给缓冲字节最低位赋值1
							pTmpByteBufHZ[0] |= 0x01; // 0000 0001
						} else {
							// 否则赋值0，默认是0所以什么都不做
							pTmpByteBufHZ[0] &= 0xFE; // 1111 1110
						}
					}
				}
				
				if(bufByteIdx >= 8 && bufByteIdx < 16 ){
					for (icByteIdx=8; icByteIdx<16; icByteIdx++){
						// 每次给最低位赋值后左移一位，最后得到8位数据
						pTmpByteBufHZ[0] <<= 1;
						
						if (((pICData[icByteIdx]<<(7-bufByteIdx%8)) & 0x80) != 0){
							// 如果ic的该位数据是1，则给缓冲字节最低位赋值1
							pTmpByteBufHZ[0] |= 0x01; // 0000 0001
						} else {
							// 否则赋值0，默认是0所以什么都不做
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

// 暂时只支持最前面的8个字节的显示（8个半角或4个全角）
void readICDataToBuffer(uchar bufNum, uchar* str){
	uchar pos;
	
	// GB2312-80编码的编码范围是高位0xa1－0xfe，低位是 0xa1-0xfe ，
	// 其中汉字范围为 0xb0a1 和 0xf7fe，如果只是简单地判断汉字，则只要查看高字节是否大于等于0xa1就可以了
	for (pos=0; pos<10; pos++){
		if (str[pos] >= 0x20 && str[pos] <= 0x7E) {
			// 判定为半角ASCII码，调用ASCII码的取点阵函数并存放在当前的显示位置上
			setICDataToBuffer(bufNum, getICData_ASCII_8x16(str[pos]), 16, pos); 
		} else {
			// 判定为全角汉字（其实还有很多别的可能比如日文汉字，这里我们默认输入的字符串是GB2312编码）
			// 调用汉字的取点阵函数并存放在当前的显示位置上
			setICDataToBuffer(bufNum, getICData_Hanzi_16x16(&str[pos]), 32, pos);
			pos++;//由于汉字占两个字节，这里手动让循环变量跳过下一个字节
		}
	}
}

// 取得点阵数据
// addr: 计算好的点阵数据的起始地址
// size：从起始地址连续取多少字节的点阵数据，目前只支持16（ASCII码）和32（GB2312汉字）两种
uchar* getICData(unsigned long addr, uchar size) {
	
	//init
	pCE=1;
	pSI=0;
	pSCLK=0;
	
	//memset(ic_data, 32, 0x00);
	
	// 首先把片选信号（CS#）变为低
	pCE=0;
	
	// 紧跟着的是通过串行数据输入引脚（SI）移位输入 1 个字节的命令字（03 h）
	// 每一位在串行时钟（SCLK）上升沿被锁存。
	for (idx = 0; idx<8; idx++){
		pSCLK=0;
		pSI=0x03<<idx & 0x80;
		pSCLK=1;
	}
	
	// 然后继续通过串行数据输入引脚（SI）移位输入 3 个字节的地址
	// 每一位在串行时钟（SCLK）上升沿被锁存。
	for (idx = 0; idx<24; idx++){
		pSCLK=0;
		pSI=addr<<idx & 0x800000;
		
		// For test break point
		if(idx==23){
			idx = 23;
		}
		
		pSCLK=1;
	}
	
	// 经过仔细调试发现，芯片返回数据的时间线跟官方文档有所出入
	// 官方文档给出的时间线是传输完命令的最后一位以后的第一个SCLK的下降沿会从MO传回第一个字节的最高位，
	// 实际上，在传输命令的最后一个上升沿的瞬间，MO就已经开始往回传数据了，然后紧接着就会在每个下降沿传回后面的数据
	// 不知道为什么，但我手上的芯片确实如此，导致一开始总是缺少第一位数据
	// 所以在下面的循环之前先取第一位的数据
	
	// 奇怪，用stc12c5a60s2时，时序又没有问题了，搞不懂。。。下面两句暂时不用了。。。
	// 如果用了下面两句，表现为整个显示多网上位移一位。。。
	//ic_data[0] = ic_data[0] | pSO;
	//ic_data[0] <<= 1;
	
	// 然后芯片会将该地址的字节数据通过串行数据输出引脚（SO）移位输出，
	// 每一位在串行时钟（SCLK）下降沿被移出。
	// 由于第一位数据已经取过了，所以下面的循环从1而不是0开始
	for (idx = 0; idx<8*size ; idx++){
		pSCLK=0;
		
		// 每次产生下降沿，IC会继续返回一位点阵数据
		// 取得这一位点阵数据存放在当前字节的最低位
		// 注意，位赋值时不可以直接用或运算！！因为这种做法无法将原来的1改成0！！
		//       结果就是屏幕上最后一行会出现无规则的点，这些点都是从数组范围以外位移进来并且没有被置为0的点。（pSO=0）
		if (pSO == 0) {
			ic_data[idx/8] &= 0xFE; // 1111 1110 
		} else {
			ic_data[idx/8] |= 0x01; // 0000 0001
		}
		
		if(((idx+1)%8) > 0){
			// 进行左移以空出最低位以便继续接受下一位数据
			ic_data[idx/8] <<= 1;
		} else {
			// 每个字节只需要位移7次即可，最后一位，也就是真正的最低位的数据设置以后不再做左移
			// 否则会丢失最高位的数据，并产生点阵偏移
			//ic_data[idx/8] >>= 1;
		}
		
		pSCLK=1;
		
		display();//test
	}
	
	// 读取字节数据后，则把片选信号（CS#）变为高，结束本次操作。
	pCE=1;
	return ic_data;
}

uchar* getICData_ASCII_8x16(uchar ch) {
	// ASCII字符所在地址的计算公式
	addr = (ch-0x20)*16+0x3b7c0;
	return getICData(addr, 16);
}

uchar* getICData_Hanzi_16x16(uchar* hz) {
	unsigned long hzGBCodeH8, hzGBCodeL8;
	
	// 由于后面有地址的计算，所以必须使用可容纳大数的long型作为中间变量，否则做地址运算时会溢出
	// 直接将uchar型赋值给long型的话，没有赋值到的高位有可能乱掉，所以跟0xFF相与一次，确保只留下uchar部分的数据
	hzGBCodeH8 = hz[0] & 0xFF;
	hzGBCodeL8 = hz[1] & 0xFF;
	
	// GB2312汉字所在地址的计算公式
	if(hzGBCodeH8 ==0xA9 && hzGBCodeL8 >=0xA1)
		addr = (282 + (hzGBCodeL8 - 0xA1 ))*32;
	else if(hzGBCodeH8 >=0xA1 && hzGBCodeH8 <= 0xA3 && hzGBCodeL8 >=0xA1)
		addr =( (hzGBCodeH8 - 0xA1) * 94 + (hzGBCodeL8 - 0xA1))*32;
	else if(hzGBCodeH8 >=0xB0 && hzGBCodeH8 <= 0xF7 && hzGBCodeL8 >=0xA1) {
		// addr = ((hzGBCodeH8 - 0xB0) * 94 + (hzGBCodeL8 - 0xA1)+ 846)*32;
		addr = (hzGBCodeH8 - 0xB0) * 94;
		addr += (hzGBCodeL8 - 0xA1) + 846;
		addr *= 32;
	}
	
	return getICData(addr, 32);
}

/*--------------------------------------------------------------
    点阵LED第几行显示数据
   其中*p为传输的点阵数据，han为第几行显示，zishu为显示多少个字
---------------------------------------------------------------*/
void HC595_Data_Send(uchar *p, uchar han, uchar zishu, uchar offset)
{
	uchar i=0;
	uchar temp=0;
	uchar* data_buff;
	Latch_port = 0;  /*HC595锁定输出,避免数据传输过程中，屏数据变化从而显示闪烁*/
	CLK_port = 0;
	
	// 前面的4个汉字8个字节先送出去
	for(temp=0;temp<8;temp++){
		data_buff = &p[temp];
		for(i=0;i<8;i++){
			if(((data_buff[0]<<i)&0x80)!=0) DA_in_port = 0;
			else DA_in_port = 1;
			CLK_port = 1;
			CLK_port = 0;
		}
	}
	
	// 为了实现左移效果，继续送第五个汉字缓冲区当前偏移的位数
	for(i=0;i<offset;i++){
		// 根据位移量分别使用第五个汉字的高八位或第八位
		data_buff = &p[temp + (i+8)/8-1]; //TODO
		if(((data_buff[0]<<(i%8))&0x80)!=0) DA_in_port = 0;
		else DA_in_port = 1;
		CLK_port = 1;
		CLK_port = 0;
	}

	EN_port = 1; /*关屏显示，原理为使HC138输出全为1，从而三极管截止，点阵不显示*/

	ABCD_port = (ABCD_port & 0x0f)|(han<<4);

	Latch_port = 1; /*允许HC595数据输出到Q1-Q8端口*/
	EN_port = 0; /*HC138输出有效，打开显示*/
	Latch_port = 0;	/*锁定HC595数据输出*/

}

uchar isLeft=0;
void timer0() interrupt 1
{
	// 20ms
	tt++;
	TL0 = 0xB0;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	
	//if(tt >= 5){
	if(tt >= 50){
		// 1s
		tt = 0;
		
		// if(isLeft==0){
			// nowOffset++;
			// if(nowOffset==16){
				// isLeft=1;
			// }
		// } else {
			// nowOffset--;
			// if(nowOffset==0){
				// isLeft=0;
			// }
		// }
		
		if(nowOffset==15){
			
			EN_port = 1; //off screen
			
			nowOffset=0;
			
			// 切换显示后台已经准备好数据的buffer
			if (nowBufferNum == 0) {
				//nowBufferNum = 1;
				nowBufferNum = 0; //89C52的内存不够大，不能用双缓存，STC1260S2可以
				readICDataToBuffer(0, &showData[nowPos+=2]);
			} else {
				nowBufferNum = 0;
				readICDataToBuffer(1, &showData[nowPos+=2]);
			}
			
			//为了避免读新的汉字数据占用大量时间导致显示时间不均匀整个屏幕会闪烁的问题
			//读取ic数据时也需要调用display函数
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
        //bit9 = RB8;         //P2.2 show parity bit
		
		
		
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
		
		//SendData("OK好了");
		//setICDataToBuffer(nowBufferNum, getICData_ASCII_8x16(SBUF), 16, 3);
		//setICDataToBuffer(nowBufferNum, ic_data_A, 16, 1);
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