/*************************************************************************************
【功能】
    驱动16x64 LED点阵屏滚动显示任意指定汉字。
	指定方法1：修改变量showDataSize1 / 2和showDataSize1 / 2的值，上电默认循环滚动显示。
	指定方法2：串口发送文字显示
    由于汉字字库体积巨大（16x16的标准汉字点阵字库需要260KB），单片机的ROM存放不下整个字库。
    想显示任意汉字就必须借助外部储存芯片，本程序采用的是专门的点阵字库芯片。（其实也可使用Flash芯片）

【芯片选型】
    单片机
        STC89C52 （或符合下列条件的单片机均可）
            ROM(Flash)  : 8K字节 (或以上）
            RAM(xdata)  : 256字节 (或以上）
            IO          : 12个 (或以上，见端口定义说明)

    外部字库芯片
        GT20L16S1Y （高通的16x16字库芯片）
        <关于供电电压>
          该芯片需要3.3v供电
          可以使用1117等LDO将5V电源转成3.3v(作者采用的方法)
          5v直接供电作者没有试过，可能也可以
          IO电平方面，虽然电压不同但实测可以和5v单片机直连，没有问题
        <不想/不会使用外部字库芯片>
          GT20L16S1Y芯片是SOT23封装的比较小，个人爱好者使用起来不是很方便，特别是对于初学者不太友好。
          如果不用外部芯片也可以使用固定的点阵字模，这种情况下请放开程序里ziku_table数组定义的注释，
          字模放在这个数组里，同时修改程序里的 bufHZ 为 ziku_table即可。相关的引脚悬空。(相关程序也要做调整，加油)

【晶振】
    使用12M以上的晶体。建议使用24M以上的晶体，晶振频率越高，显示字不会出现闪烁感。
    如果使用24M以下的晶体，如果是STC的芯片，建议打开6T(双倍速)功能。

【端口定义】
    点阵屏模块
        LATCH  - P1.0  (串行数据锁存)
        CLK    - P1.1  (串行数据时钟)
        R1     - P1.2  (串行数据输入，R1表示上屏红灯数据信号，R2下红，G1上绿，G2下绿，作者的模块是单红色板且只有32单行的上屏，所以接R1即可)
        EN     - P1.3
        A      - P1.4
        B      - P1.5
        C      - P1.6
        D      - P1.7
    字库芯片 GT20L16S1Y
        CE     - P2^0
        SCLK   - P2^1
        SI     - P2^2
        SO     - P2^3

【关于定时器、串口的说明】
	定时器0：未使用（优先级高于串口中断，所以不可用来扫描，否则会影响串口通信）
	定时器1：作为硬件串口的波特率发生器
	定时器2：行扫描以及滚动显示控制，中断优先级低于串口，不会影响串口通信
*/

/*头文件*/
#include <intrins.h>
#include <STC89C52.h>
#define uint unsigned int
#define uchar unsigned char
#define NOP() _nop_()

#define FOSC 32000000L      // 系统晶振频率
#define SCROLL_SPEED 10    // 文字滚动速度（5-500）越小越快

// 点阵屏定义 #########################################################################################
sbit EN_port 	= P1^3;
sbit DA_in_port = P1^2;
sbit CLK_port 	= P1^1;
sbit Latch_port = P1^0;

#define ABCD_port P1  //HC138 ABCD端口定义
//sbit D_port = P1^7;
//sbit C_port = P1^6;
//sbit B_port = P1^5;
//sbit A_port = P1^4;

// 大Buffer
// 保存16X64点阵信息（128字节）（4个汉字）
// ——————————————————————————————————————————————————————————
// B0	B16		B32		B48		B64		B80		B96		B112
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
uchar xdata bufHZ[128] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

uchar nowShiftOffset=0; 	// 当前左移偏移量(全角 0-15/ 半角 0-7)
int nowShowTextPos=0;		// 当前显示第几个字（严格来说是第几个字节）

// 一次性传给HC595用的64+16位的一整行数据（包括移位用的第五个汉字的缓冲区）
uchar data row_data_buf[10];
void HC595_Data_Send(uchar *p, uchar han, uchar offset);

// 画面刷新到一半时不允许左移
bit isShowing = 0;

// 左移函数 （包括从IC取最右边的一列数据，存入大buffer，同时更新当前偏移位置）
void shiftLeft(void);


uchar nowText=0;
int showDataSize=0;
uchar* textForShow;
// 显示文字设置，注意长度和文字内容要匹配
int showDataSize1=972*2;
uchar code textForShow1[] = "《长恨歌》【唐·白居易】汉皇重色思倾国，御宇多年求不得。杨家有女初长成，养在深闺人未识。天生丽质难自弃，一朝选在君王侧。回眸一笑百媚生，六宫粉黛无颜色。春寒赐浴华清池，温泉水滑洗凝脂。侍儿扶起娇无力，始是新承恩泽时。云鬓花颜金步摇，芙蓉帐暖度春宵。春宵苦短日高起，从此君王不早朝。承欢侍宴无闲暇，春从春游夜专夜。后宫佳丽三千人，三千宠爱在一身。金屋妆成娇侍夜，玉楼宴罢醉和春。姊妹弟兄皆列土，可怜光彩生门户。遂令天下父母心，不重生男重生女。骊宫高处入青云，仙乐风飘处处闻。缓歌慢舞凝丝竹，尽日君王看不足。渔阳鼙鼓动地来，惊破霓裳羽衣曲。九重城阙烟尘生，千乘万骑西南行。翠华摇摇行复止，西出都门百余里。六军不发无奈何，宛转蛾眉马前死。花钿委地无人收，翠翘金雀玉搔头。君王掩面救不得，回看血泪相和流。黄埃散漫风萧索，云栈萦纡登剑阁。峨嵋山下少人行，旌旗无光日色薄。蜀江水碧蜀山青，圣主朝朝暮暮情。行宫见月伤心色，夜雨闻铃肠断声。天旋日转回龙驭，到此踌躇不能去。马嵬坡下泥土中，不见玉颜空死处。君臣相顾尽沾衣，东望都门信马归。归来池苑皆依旧，太液芙蓉未央柳。芙蓉如面柳如眉，对此如何不泪垂。春风桃李花开夜，秋雨梧桐叶落时。西宫南内多秋草，落叶满阶红不扫。梨园弟子白发新，椒房阿监青娥老。夕殿萤飞思悄然，孤灯挑尽未成眠。迟迟钟鼓初长夜，耿耿星河欲曙天。鸳鸯瓦冷霜华重，翡翠衾寒谁与共。悠悠生死别经年，魂魄不曾来入梦。临邛道士鸿都客，能以精诚致魂魄。为感君王展转思，遂教方士殷勤觅。排空驭气奔如电，升天入地求之遍。上穷碧落下黄泉，两处茫茫皆不见。忽闻海上有仙山，山在虚无缥渺间。楼阁玲珑五云起，其中绰约多仙子。中有一人字太真，雪肤花貌参差是。金阙西厢叩玉扃，转教小玉报双成。闻到汉家天子使，九华帐里梦魂惊。揽衣推枕起徘回，珠箔银屏逦迤开。云鬓半偏新睡觉，花冠不整下堂来。风吹仙袂飘摇举，犹似霓裳羽衣舞。玉容寂寞泪阑干，梨花一枝春带雨。含情凝睇谢君王，一别音容两渺茫。昭阳殿里恩爱绝，蓬莱宫中日月长。回头下望人寰处，不见长安见尘雾。唯将旧物表深情，钿合金钗寄将去。钗留一股合一扇，钗擘黄金合分钿。但教心似金钿坚，天上人间会相见。临别殷勤重寄词，词中有誓两心知。七月七日长生殿，夜半无人私语时。在天愿作比翼鸟，在地愿为连理枝。天长地久有时尽，此恨绵绵无绝期。";
int showDataSize2=722*2;
uchar code textForShow2[] = "《琵琶行》【唐·白居易】浔阳江头夜送客，枫叶荻花秋瑟瑟。主人下马客在船，举酒欲饮无管弦。醉不成欢惨将别，别时茫茫江浸月。忽闻水上琵琶声，主人忘归客不发。寻声暗问弹者谁，琵琶声停欲语迟。移船相近邀相见，添酒回灯重开宴。千呼万唤始出来，犹抱琵琶半遮面。转轴拨弦三两声，未成曲调先有情。弦弦掩抑声声思，似诉平生不得志。低眉信手续续弹，说尽心中无限事。轻拢慢捻抹复挑，初为《霓裳》后《六幺》。大弦嘈嘈如急雨，小弦切切如私语。嘈嘈切切错杂弹，大珠小珠落玉盘。间关莺语花底滑，幽咽泉流冰下难。冰泉冷涩弦凝绝，凝绝不通声暂歇。别有幽愁暗恨生，此时无声胜有声。银瓶乍破水浆迸，铁骑突出刀枪鸣。曲终收拨当心画，四弦一声如裂帛。东船西舫悄无言，唯见江心秋月白。沉吟放拨插弦中，整顿衣裳起敛容。自言本是京城女，家在虾蟆陵下住。十三学得琵琶成，名属教坊第一部。曲罢曾教善才服，妆成每被秋娘妒。五陵年少争缠头，一曲红绡不知数。钿头银篦击节碎，血色罗裙翻酒污。今年欢笑复明年，秋月春风等闲度。弟走从军阿姨死，暮去朝来颜色故。门前冷落鞍马稀，老大嫁作商人妇。商人重利轻别离，前月浮梁买茶去。去来江口守空船，绕船月明江水寒。夜深忽梦少年事，梦啼妆泪红阑干。我闻琵琶已叹息，又闻此语重唧唧。同是天涯沦落人，相逢何必曾相识！我从去年辞帝京，谪居卧病浔阳城。浔阳地僻无音乐，终岁不闻丝竹声。住近湓江地低湿，黄芦苦竹绕宅生。其间旦暮闻何物？杜鹃啼血猿哀鸣。春江花朝秋月夜，往往取酒还独倾。岂无山歌与村笛？呕哑嘲哳难为听。今夜闻君琵琶语，如听仙乐耳暂明。莫辞更坐弹一曲，为君翻作《琵琶行》。感我此言良久立，却坐促弦弦转急。凄凄不似向前声，满座重闻皆掩泣。座中泣下谁最多？江州司马青衫湿。";

//int showDataSize=21;
//uchar code textForShow1[] = "测试1全2角3半角混aBc合";
//int showDataSize=32;
//uchar code textForShow2[] = "!"#$%&'()=~|?^-@`[]{};:+*,.<>/_?";


// 不使用字库芯片而使用固定文字时放开这一段，并修改程序里的 bufHZ 为 ziku_table
// 取模软件，需要指定【阴码+顺向（高位在前）+行列式】的形式
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

// 测试用的字母A的16x8点阵
// 可直接这样显示在画面上 -> setICDataToBuffer(ic_data_A, 16, 0);
uchar code ic_data_A[] = {0x00,0xE0,0x9C,0x82,0x9C,0xE0,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x0F,0x00};

// 字库芯片定义 ###################################################################################
sbit pCE        =P2^0;         // 片选
sbit pSCLK      =P2^1;         // 时钟
sbit pSI        =P2^2;         // 数据输入（单片机->字库芯片）
sbit pSO        =P2^3;         // 数据输出（字库芯片->单片机）

unsigned long addr;
int idx = 0;
uchar data ic_data[32];

// 从芯片取当前文字当前偏移量下的完整点阵
void readICDataToBuffer(uchar* str);
// 从芯片取点阵的子函数
uchar* getICData_ASCII_8x16(uchar ch);
uchar* getICData_Hanzi_16x16(uchar* hz);

// 从芯片取指定文字指定列的上下两个字节（用于位移）
void getICData_Col(uchar* str, uchar colIdx, uchar* colData);

// 芯片取到的数据转换成适合模块的数据格式并保存在大buffer中
void setICDataToBuffer(uchar *pICData, uchar size, uchar pos);

// 串口定义 ######################################################################################
typedef unsigned char BYTE;
typedef unsigned int WORD;

#define BAUD 9600           //UART baudrate
//#define BAUD 1200           //UART baudrate

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

bit busy;

// 串口接受文字缓冲区
uchar serialRcvBuf[50]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uchar serialRcvIdx=0;

// 其他定义 ######################################################################################
//test
void testSetFullScreenByte(uchar);
//test

// 虽然定义了Timer0但是main函数没有启用，目前还用不到，以后如果需要可以利用
int ttTimer0 = 0;
void Timer0Init(void)		//30us@32.000MHz@6T
{
	EA=0;

	TMOD &= 0xF0;	//设置定时器模式
	TMOD |= 0x02;	//设置定时器模式（8位自动重载）
	TL0 = 0xb0;		//设置定时初值(30us@6T@@32.000MHz)
	TH0 = 0xb0;		//设置定时初值(30us@6T@32.000MHz)
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
    ET0 = 1;

	EA=1;
}

int ttTimer2 = 0;
void Timer2Init(void)
{
	// 89系列定时器2只有16位自动重载模式
	T2MOD = 0;				//初始化模式寄存器
	T2CON = 0;				//初始化控制寄存器
	RCAP2L = TL2 = 0x2B;	//设置定时初值和重载值(1ms@32M@6T)
	RCAP2H = TH2 = 0xEB;	//设置定时初值和重载值(1ms@32M@6T)
	TR2 = 1;				//定时器2开始计时
    ET2 = 1;                //enable timer2 interrupt
    EA = 1;                 //open global interrupt switch
}

// 行扫描函数（每调用一次扫描一行）
// 为什么16行的扫描不放在函数里面一次性做掉，是因为希望行扫描之间留有空隙可以做别的工作
// 放在定时器0的中断函数中定时（很短）执行
// 速度越快画面越不闪烁，但每次操作时间不可以超过定时间隔
// 读取字库，更新显示数据的处理必须在定时间隔内完成（如果达不到这么快，至少要在一屏16行数据刚好扫描完时执行，这样看上去相对最稳定（每次在同一个地方拖慢，不至于整体闪烁））
uchar rowIdx=0; // 当前扫描第几行
void display(){
	uchar i;
	
	// 循环从大buffer中取当前行从左到右的全部8个字节作为一整行数据保存在显示用的buff中
	for (i=0;i<10;i++){
		row_data_buf[i]=bufHZ[i*16 + rowIdx];
	}
	
	// 将上面取到的一整行数据串行输出给74HC595阵列，点亮当前行的数据
	HC595_Data_Send(row_data_buf,rowIdx,nowShiftOffset);
			
	// 当前行数加1，继续下一行的扫描
	rowIdx++;
	
	// 扫描完16行，回到第一行继续行扫描
	if(rowIdx==16) rowIdx=0;
}

// 一旦串口接受到文字，则停止滚动显示默认文字，转成显示串口文字模式
// 这种模式暂时不支持滚动
bit isShowSerialData = 0;
void main()
{
	nowText = 0;
	showDataSize = showDataSize1;
	textForShow = textForShow1;

	// 每次从最右端（nowShowTextPos+8的汉字）开始左移进来
	nowShowTextPos = -8;

	UartInit(); //使用Timer1作为波特率发生器
//	Timer0Init();
    Timer2Init();
	
	// For test --------------------------------------------------------------------------------
	//readICDataToBuffer( getICData_Hanzi_16x16("全"), 32, 0);
//	readICDataToBuffer(textForShow);
	//readICDataToBuffer("汉汉皇皇");
	
	//setICDataToBuffer(getICData_ASCII_8x16('A'), 16, 0);
	//setICDataToBuffer(ic_data_A, 16, 0);
	// For test --------------------------------------------------------------------------------
    while(1){
		//display();
		//testSetFullScreenByte(0xff);

		//串口接收到文本显示在画面上 还需要修改
		//每收到8个字节（一屏文字）就刷新画面
		if(serialRcvIdx >= 8){
			TR2=0; //暂停扫描不然会资源冲突画面会变得很慢
			// 切换到串口数据显示模式，停止滚动
			isShowSerialData=1;
			// 取串口文字的点阵
			//readICDataToBuffer("汉汉皇皇");
			readICDataToBuffer(serialRcvBuf);

			serialRcvIdx=0;
			TR2=1; //恢复扫描
		}
	}
}

// 将接收到的IC点阵数据存入缓冲区用于显示
// pICData	- 接收到的点阵数组
// size		- 点阵数组的大小。通常汉字为32字节，半角ASCII码为16字节。
// pos		- 指定该点阵数据显示的开始位置(0-7)，以8位（列）为一个单位。会覆盖原来的数据。
void setICDataToBuffer(uchar *pICData, uchar size, uchar pos)
{
	// ############# 从字库芯片GT20L16S1Y取出的点阵信息是竖置横列模式（例：字母A） ############# 
	// 从字库芯片GT20L16S1Y取出的点阵信息是竖置横列模式，也就是以竖排为单位取到的数据
	// ---- 下面从芯片取到的字母A的点阵数据 竖置横排的（而且是高位在下面，低位在上面） ---- 
	//       B0	B1	B2	B3	B4	B5	B6	B7
	// bit0  0	0	0	0	0	0	0	0
	// bit1  0	0	0	●	0	0	0	0
	// bit2  0	0	●	0	●	0	0	0
	// bit3  0	0	●	0	●	0	0	0
	// bit4  0	0	●	0	●	0	0	0
	// bit5  0	●	0	0	0	●	0	0
	// bit6  0	●	0	0	0	●	0	0
	// bit7  0	●	●	●	●	●	0	0
	//       B8	B9	B10	B11	B12	B13	B14	B15
	// bit0  ●	0	0	0	0	0	●	0
	// bit1  ●	0	0	0	0	0	●	0
	// bit2  ●	0	0	0	0	0	●	0
	// bit3  ●	0	0	0	0	0	●	0
	// bit4  0	0	0	0	0	0	0	0
	// bit5  0	0	0	0	0	0	0	0
	// bit6  0	0	0	0	0	0	0	0
	// bit7  0	0	0	0	0	0	0	0
	// 而手头的这款点阵屏的驱动方式是行驱动，也就是每次扫描时需要传入一整行的点阵信息
	// 所以需要把每次取到的点阵信息转存到横置竖排（也就是汉字取模软件中的“行列式”）形式的buf中去，
	// 显示时从buf中取连续的一整行的8个字节的数据串行传入595用于扫描显示
	// 而存放的坐标跟这个字所在的显示位置有关系，比如我们打算将上面这个A的点阵显示在屏幕的最左边
	// 那么，转存以后应该是下面这个样子（右侧的数据省略）
	// 也就是说要将取到的B0-B7的最高位组合成buf里的一个字节B0（字符A为例就是0x00）
	// 将取到的B0-B7的次高位组合成buf里的字节B1（字符A为例就是0x10）
	//     bit7 ---------------------- bit0
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
	//memset(&bufHZ[pos*16], size, 0x00);
	
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
				pTmpByteBufHZ = &(bufHZ[pos*16 + bufByteIdx]);
				//display();//test
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
					
				pTmpByteBufHZ = &(bufHZ[pos*16 + bufByteIdx]);
				
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
void readICDataToBuffer(uchar* str){
	uchar pos;
	
	// GB2312-80编码的编码范围是高位0xa1－0xfe，低位是 0xa1-0xfe ，
	// 其中汉字范围为 0xb0a1 和 0xf7fe，如果只是简单地判断汉字，则只要查看高字节是否大于等于0xa1就可以了
	for (pos=0; pos<8; pos++){
		if (str[pos] >= 0x20 && str[pos] <= 0x7E) {
			// 判定为半角ASCII码，调用ASCII码的取点阵函数并存放在当前的显示位置上
			setICDataToBuffer(getICData_ASCII_8x16(str[pos]), 16, pos); 
		} else {
			// 判定为全角汉字（其实还有很多别的可能比如日文汉字，这里我们默认输入的字符串是GB2312编码）
			// 调用汉字的取点阵函数并存放在当前的显示位置上
			setICDataToBuffer(getICData_Hanzi_16x16(&str[pos]), 32, pos);
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
		
		//display();//test
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
void HC595_Data_Send(uchar *p, uchar han, uchar offset)
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

	EN_port = 1; /*关屏显示，原理为使HC138输出全为1，从而三极管截止，点阵不显示*/

	ABCD_port = (ABCD_port & 0x0f)|(han<<4);

	Latch_port = 1; /*允许HC595数据输出到Q1-Q8端口*/
	EN_port = 0; /*HC138输出有效，打开显示*/
	Latch_port = 0;	/*锁定HC595数据输出*/
}

// 当前未使用该中断函数
void Timer0() interrupt 1
{
	// 30us一次
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
	TH1 = TL1 = -(FOSC/6/32/BAUD); //Set auto-reload vaule(FOSC/6的这个6表示下载时指定了6T模式，如果是默认12T则改成12)
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

SBUF=SBUF;
		if(SBUF == 0x55){
			serialRcvIdx=0;
		} else {
			serialRcvBuf[serialRcvIdx] = SBUF;
			serialRcvIdx++;
		}
		TR0=1;
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
void SendData(BYTE dat)
{
    while (busy){
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


void Timer2() interrupt 5
{
	// 1ms一次 @ 32M @ 6T
	// 视觉上感觉不闪烁要求最少2ms以内扫描一行
	// 16行就是30ms，一秒钟也就是30帧左右再慢就闪烁了。
	// 但也不可太快，因为太快的画间隔太短不够取数据更新数据的时间，也会导致闪烁

	// 定时器2的TF2标志位必须手动清零
	TF2 = 0;

	// 行扫描----------------
	// 每扫描一次，全局变量rowIdx自加一，一直到15再回到0
	display();

	// 如果当前是显示串口文字模式则不进行左移
	if (isShowSerialData == 1) {
		return;
	}

	// 左移----------------
	ttTimer2++;
	// 当行扫描一帧结束时，并且滚动时间间隔到了
	// 进行左移
	// 判断rowIdx=0的作用是，左移处理只允许在画面完整的一帧扫描完以后才可以做
	// 这样可以避免画面闪烁。因为左移处理中需要读取字库芯片等耗时的操作
	// 如果任意打断画面扫描，整个画面会不规则闪烁
	// 最理想的是每两行扫描之间的那点时间足够执行完左移操作
	// 也就是左移花费的时间要尽量少于当前定时器0的中断间隔
	if ( rowIdx == 0 && ttTimer2 >= SCROLL_SPEED) {
		ttTimer2 = 0;
		// 左移1位
		//EN_port = 1; //off screen
		shiftLeft();
		//EN_port = 0; //on screen
	}
}

// 画面左移一位
void shiftLeft() {

	uchar idx = 0;

	// 从IC取得右侧将要移入的列数据（上下各一个字节）
	// 第一个字节用来返回是Ascii码还是汉字，以便移位offset的重置
	uchar shiftInColData[3] = {0x00,0x00,0x00};
	getICData_Col(&textForShow[nowShowTextPos+8], nowShiftOffset, shiftInColData);

	// 大Buffer全体左移一位
	for (idx=0; idx<128; idx++){
		// 先左移1位
		bufHZ[idx]<<=1;
		// 最低位用相邻右侧数据的最高位填充
		bufHZ[idx] |= ((bufHZ[idx+16]>>7)&0x01);
	}

	// 取到的上下两个纵列字节()的数据的每一位写入大buffer最右侧
	// 也就是写入B112-B127这16个字节的最低位
	// <大Buff最右侧数据>
	// B112   ...bit0 <- shiftInColData[1]的 bit0
	// ...
	// B119   ...bit0 <- shiftInColData[1]的 bit7
	// B120   ...bit0 <- shiftInColData[2]的 bit0
	// ...
	// B127   ...bit0 <- shiftInColData[2]的 bit7
	for(idx = 0; idx < 8; idx++)
	{
		bufHZ[idx+112] &= 0xFE; // 112-119 清零最低位
		bufHZ[idx+120] &= 0xFE; // 120-127 清零最低位
		
		if ((shiftInColData[1]<<(7-idx) & 0x80) == 0x80 )  {
			bufHZ[idx+112] |= 0x01; // 112-119 最低位置1
		}

		if ((shiftInColData[2]<<(7-idx) & 0x80) == 0x80 )  {
			bufHZ[idx+120] |= 0x01; // 120-127 最低位置1
		}
	}

	// 偏移量加一，通知显示子函数刷新画面
	if(shiftInColData[0] == 0){
		// 当前移进来的字符是Ascii码8位宽度
		if(nowShiftOffset==7){
			nowShiftOffset=0;
			nowShowTextPos+=1;  // 全部移动完成后，当前显示文字位置向后移动1个字节（一个Ascii即1个字节）
			if(nowShowTextPos>=(showDataSize-8))
			{
				nowShowTextPos=-8;

				// 切换两行文字的显示
				if(nowText==0){
					nowText = 1;
					showDataSize = showDataSize2;
					textForShow = textForShow2;
				}else{
					nowText = 0;
					showDataSize = showDataSize1;
					textForShow = textForShow1;
				}
				
			}
		} else {
			nowShiftOffset++;
		}
	} else {
		// 当前移进来的字符是汉字16位宽度
		if(nowShiftOffset==15){
			nowShiftOffset=0;
			nowShowTextPos+=2;  // 全部移动完成后，当前显示文字位置向后移动2个字节（一个汉字即2个字节）
			if(nowShowTextPos>=(showDataSize-8))
			{
				nowShowTextPos=-8;

				// 切换两行文字的显示
				if(nowText==0){
					nowText = 1;
					showDataSize = showDataSize2;
					textForShow = textForShow2;
				}else{
					nowText = 0;
					showDataSize = showDataSize1;
					textForShow = textForShow1;
				}
				
			}
		} else {
			nowShiftOffset++;
		}
	}
	
}

// 取得指定文字的指定列的点阵数据（两个字节）
// 返回参数结构：
//    colData[0] : 返回是否为Asc/汉字标志位。0为Ascii,1为汉字。
//    colData[1] : 返回指定列 上半部分数据
//    colData[2] : 返回指定列 下半部分数据
void getICData_Col(uchar* str, uchar colIdx, uchar* colData)
{
	unsigned long hzGBCodeH8, hzGBCodeL8;

	// 先清空目标字节
	colData[0] = 0x00; //返回是否为Asc/汉字标志位。0为Ascii,1为汉字。 
	colData[1] = 0x00; //返回指定列 上半部分数据
	colData[2] = 0x00; //返回指定列 下半部分数据

	// 计算点阵所在地址
	// GB2312-80编码的编码范围是高位0xa1－0xfe，低位是 0xa1-0xfe ，
	// 其中汉字范围为 0xb0a1 和 0xf7fe，如果只是简单地判断汉字，则只要查看高字节是否大于等于0xa1就可以了
	if (str[0] >= 0x20 && str[0] <= 0x7E) {
		// ASCII字符
		addr = (str[0]-0x20)*16+0x3b7c0;

		// 只需要指定列的2个字节，分两次取单字节
		colData[1] = getICData(addr+colIdx, 1)[0];
		colData[2] = getICData(addr+colIdx+8, 1)[0];

		colData[0] = 0;
	} else {
		// 全角汉字
		// 由于后面有地址的计算，所以必须使用可容纳大数的long型作为中间变量，否则做地址运算时会溢出
		// 直接将uchar型赋值给long型的话，没有赋值到的高位有可能乱掉，所以跟0xFF相与一次，确保只留下uchar部分的数据
		hzGBCodeH8 = str[0] & 0xFF;
		hzGBCodeL8 = str[1] & 0xFF;

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

		// 只需要整个汉字指定列的2个字节，分两次取单字节
		colData[1] = getICData(addr+colIdx, 1)[0];
		colData[2] = getICData(addr+colIdx+16, 1)[0];

		colData[0] = 1;
	}

	
}

// test---------------------------------------------------------------
// 将整个屏幕所有数据设置成同一个数据(如果传入0xFF就是全部点亮)
// 本函数用于测试LED板子，需要放在主程序while循环中才可以（模块需要不停扫描否则不显示）
void testSetFullScreenByte(uchar bData)
{
	uchar row=0;
	uchar i=0;
	uchar temp=0;
	Latch_port = 0;  /*HC595锁定输出,避免数据传输过程中，屏数据变化从而显示闪烁*/
	CLK_port = 0;
	
	for(row = 0; row < 16; row++)
	{
		for(temp=0;temp<8;temp++){
			for(i=0;i<8;i++){
				if(((bData<<i)&0x80)!=0) DA_in_port = 0;
				else DA_in_port = 1;
				CLK_port = 1;
				CLK_port = 0;
			}
		}

		EN_port = 1; /*关屏显示，原理为使HC138输出全为1，从而三极管截止，点阵不显示*/

		ABCD_port = (ABCD_port & 0x0f)|(row<<4);

		Latch_port = 1; /*允许HC595数据输出到Q1-Q8端口*/
		EN_port = 0; /*HC138输出有效，打开显示*/
		Latch_port = 0;	/*锁定HC595数据输出*/
	}
}