#include <intrins.h>
#include <STC89C52.h>
#define uint unsigned int
#define uchar unsigned char

// 点阵屏定义 #########################################################################################
sbit EN_port 	= P3^6;
sbit DATA_R1    = P3^5;
sbit CLK_port 	= P1^7;
sbit Latch_port = P1^6;

// 红色单色屏未使用定义
sbit DATA_R2    = P3^4;
sbit DATA_G1    = P1^4;
sbit DATA_G2    = P1^5;

#define ABCD_port P1  //HC138 ABCD端口定义
sbit D_port = P1^3; //-> HC138 E1 低电平使能
sbit C_port = P1^2;
sbit B_port = P1^1;
sbit A_port = P1^0;

void testSetFullScreenByte(uchar);

void delay_ms(uchar n)
{
	uchar x,y;
	for(x=n;x>0;x--)
	{
		for(y=5;y>0;y--);	
	}
}

int ttTimer2 = 0;
void Timer2Init(void)
{
	// 89系列定时器2只有16位自动重载模式
	T2MOD = 0;				//初始化模式寄存器
	T2CON = 0;				//初始化控制寄存器
	RCAP2L = TL2 = 0x2B;	//设置定时初值和重载值(1ms@32M@6T)
	RCAP2H = TH2 = 0xFB;	//经验值：EB比较合适画面最稳定，但FB滚动更迅速。设置定时初值和重载值(1ms@32M@6T)
	TR2 = 1;				//定时器2开始计时
    ET2 = 1;                //enable timer2 interrupt
    EA = 1;                 //open global interrupt switch
}

uchar rowIdx=0; // 当前扫描第几行
void main()
{DATA_G1=0;
    //Timer2Init();
	//testSetFullScreenByte(0xFE);
    while(1){
		testSetFullScreenByte(0x55);
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
	
}

// test---------------------------------------------------------------
// 将整个屏幕所有数据设置成同一个数据(如果传入0xFF就是全部点亮)
// 本函数用于测试LED板子，需要放在主程序while循环中才可以（模块需要不停扫描否则不显示）
uchar nowBright=0;
uchar maxBright=100;
uchar setBright=1;
uchar row=0;

uchar flg=0;
void testSetFullScreenByte(uchar bData)
{
	uchar i=0;
	uchar temp=0;

	nowBright++;
	if (flg == 0 && nowBright >= maxBright) {
		//flg = 1;
		nowBright = 0;

		// 将每一位移入595的移位寄存器
		// SM16106的LE（锁存）设置为低电平，数据移入
		Latch_port = 0;
		
		for(temp=0;temp<8;temp++){

			for(i=0;i<8;i++){
				CLK_port = 0;

				if(((bData<<i)&0x80)!=0) {
					DATA_R1 = 0;
				} else {
					DATA_R1 = 1;
				}

				// 上升沿移入数据
				CLK_port = 1;
			}
		}

		/*SM16106的LE（锁存）设置为高电平，数据锁存*/
		Latch_port = 1; 
		

		//控制138打开指定行的输入
		 ABCD_port = (ABCD_port & 0xf0)|row;
		// D_port = 1;
		// A_port = 1;
		// B_port = 1;
		// C_port = 1;

		row++;
		if(row == 16) row = 0;
	}
	
	if (nowBright < setBright) {
		/*HC138输出有效，打开显示*/
		EN_port = 0;
	} else {
		/*HC138输出有效，关闭显示*/
		EN_port = 1;
	}

}