/****************************************************************************
程序功能：		用串口调试助手软件通过串口向单片机发送数据。
				当发送的数据为1到8的字符时，分别对应点亮相应的发光二极管。
				发送其他数据，蜂鸣器会响。
****************************************************************************/
#include <STC12C52xx.h>
#include <1602.h>
	
#define uchar    unsigned char 
#define uint     unsigned int

sbit ledR = P2^0;
sbit ledG = P2^1;
sbit ledB = P2^2;

uchar buf;
uchar bufS[3];
uchar bufE[3];

#define UART_STS_CHK_HEAD     0
#define UART_STS_RCV_BODY     1
#define UART_STS_CHK_FOOT     2
#define UART_HEAD_CMD     'S' // 定义命令消息头 [S]
#define UART_HEAD_TXT     'T' // 定义文本消息头 [T]
#define UART_TAIL             'E' // 定义消息尾 [E]
uchar uartStatus = UART_STS_CHK_HEAD;
uchar checkHead_len = 0;
uchar checkFoot_len = 0;
uchar uartHead = 0x00;

uchar ttCounter = 0;

// 串口打印 用定义 ---------------------------------------------------------
bit busy;
void SendData(uchar dat);
void SendString(char *s);
// 串口打印 用定义 ---------------------------------------------------------

void doCommand(uchar);
void doDisplay();

// 1602 用定义 ---------------------------------------------------------

// 1602可显示32个ascii，加1是用来保存字符串结束符号'\0' (0x00)用的
char strBuff[32+1];

// 已经接收到并显示的字符数，用于检测是否超过显示可能范围，以及计算显示位置
uchar rcvCharCnt = 0;

// TX板子特有,移植到别的板子上不需要
sbit dula=P2^6;
sbit wela=P2^7;
// TX板子特有,移植到别的板子上不需要


// AUXR各位设置参考
// AUXR.0:S1BRS，如果为1那么串口1的波特率发生器为独立波特率发生器，否则为定时器1
// AUXR.1:EXTRAM，为1则可以使用内部扩展RAM
// AUXR.2:BRTx12，为1则独立波特率发生器不分频，否则12分频。
// AUXR.3:S2SMOD，为1串口2波特率加倍，否则不加倍
// AUXR.4:BRTR，为1则允许独立波特率允许，否则不允许。
// AUXR.5:UART_M0x6，为1则串口模式0为2分频，否则为12分频
// AUXR.6:T1x12，为1则定时器1不分频，否则12分频
// AUXR.7:T0x12，为1则定时器0不分频，否则12分频

void Timer0Init(void)       //10毫秒@11.0592MHz
{
    AUXR &= 0x7F;       //定时器0时钟12T模式 0111 1111 // AUXR.bit 7:T0x12，为1则定时器0不分频，否则12分频
    TMOD &= 0xF0;       //设置定时器0模式
    TMOD |= 0x01;       //设置定时器0模式
    TL0 = 0x00;     //设置定时初值
    TH0 = 0xDC;     //设置定时初值
    TF0 = 0;        //清除TF0标志
}

void UartInit(void)     //9600bps@11.0592MHz
{
    PCON &= 0x7F;     //波特率不倍速
    SCON = 0x50;        //8位数据,可变波特率
    AUXR &= 0xBF;     //定时器1时钟为Fosc/12,即12T 1011 1111   // AUXR.6:T1x12，为1则定时器1不分频，否则12分频
    AUXR &= 0xFE;     //串口1选择定时器1为波特率发生器 1111 1110 // AUXR.0:S1BRS，如果为1那么串口1的波特率发生器为独立波特率发生器，否则为定时器1
    TMOD &= 0x0F;     //清除定时器1模式位
    TMOD |= 0x20;     //设定定时器1为8位自动重装方式
    TL1 = 0xFD;     //设定定时初值
    TH1 = 0xFD;     //设定定时器重装值
    ET1 = 0;        //禁止定时器1中断
    TR1 = 1;        //启动定时器1
}

void main(void) 
{
	int p;
	
	// TX板子特有,移植到别的板子上不需要
	// TX1C板子上的P0口是 LCD1602和7段数码管共用的
	// 想用LCD的时候,必须把跟数码管连接的地方锁存以免影响LCD(段锁存器,位锁存器)
	// 用到这里真想骂娘,郭天祥开始觉得是神人,现在看来,程序一堆问题,硬件设计很多也不合理,代码里数据的英文data居然写成date,唉...
	dula = 0; wela = 0;
	
	P1=0xff;
	
	LCD_Init();
    Timer0Init();
	UartInit();

    // 这两句是必须的，但是STCISP软件没有自动生成，记得加上
    ES = 1;         //允许串口中断
    ET0 = 1;        //允许定时器0中断
    ET1 = 0;        //禁止定时器1中断（因为定时器1作为串口1的波特率发生器了）
    EA = 1;         //开总中断

	
	//p=LCD_PutStr("=    Hi! It\'s me =\n",-1);
	//p=LCD_PutStr("    Robot ver ",p);
	p=LCD_PutNum(100, 17);
	
	while(1);
}

/*********************************************************
    定时器0中断服务函数
*********************************************************/
void tm0_isr() interrupt 1
{
    // 10毫秒进来一次
    TL0 = 0x00;     //设置定时初值
    TH0 = 0xDC;     //设置定时初值
    ttCounter++;

    if (ttCounter >= 20)          //10ms * 20 -> 200ms
    {
        ttCounter = 0;
        doDisplay();
    }
}

/*********************************************************
    串行中断服务函数
*********************************************************/
void serial() interrupt 4 
{
    ES = 0;                                //关闭串行中断

    if(RI==1)
    {
        RI = 0;                                //清除串行接受标志位
        buf = SBUF;                        //从串口缓冲区取得数据

        // 这里定义了一个消息的协议：
        // 每条有效的消息为4个字节:
        // 三个字节的消息头[S] + 一个字节的消息体    (如 [S]1)
        // 以后允许多字节消息体的时候考虑加上消息尾[E]
        // 目前，形如 [S][S]8 这种误码，并不会解析成消息"8"，而是会解析成消息"["，后面的 "S]8" 会被丢弃 

        if (uartStatus == UART_STS_CHK_HEAD) {
            // 每接收到一个字节首先检查是不是消息头的前导字符 [
            if (buf == '[') {
                uartStatus = UART_STS_CHK_HEAD;
                checkHead_len = 1;
                SBUF = '('; //打印给串口调试用
                
                // 上一个消息体的字符数清0
                rcvCharCnt = 0;
            }
            else if (checkHead_len == 1 && ( 
                        buf == UART_HEAD_CMD ||
                        buf == UART_HEAD_TXT
                        )
                    )
            {
                checkHead_len = 2;
                uartHead = buf;
                SBUF = buf; //打印给串口调试用
            }
            else if (checkHead_len == 2 && buf == ']') {
                uartStatus = UART_STS_RCV_BODY; //协议头检查成功，开始接受消息体
                checkHead_len = 0;
                SBUF = ')'; //打印给串口调试用

                // 如果是开始接收[T]字符串，则启动定时器0，
                // 以检测什么时候数据接收结束（一段时间内不再有数据进来就认为发送结束了）
                if (uartHead == UART_HEAD_TXT) TR0 = 1;
            } else {
                // 消息头中途检测失败，比如[S的后面不是]而是别的字符，则判断失败
                // 需要从头开始判断
                checkHead_len = 0;
            }
        }
        else if (uartStatus == UART_STS_RCV_BODY) {
            //SBUF = buf; //打印给串口调试用

            switch(uartHead)
            {
                case UART_HEAD_CMD:
                    // 命令模式下，只使用紧跟着head的第一个字节，后面的字节丢弃（如果包含有效head则视为新消息处理）
                    // 继续回到检查下一个head的状态
                    uartStatus = UART_STS_CHK_HEAD;

                    // 调用命令处理函数
                    doCommand(buf);
                    break;

                case UART_HEAD_TXT:
                    // 文字消息模式下，接收到的字符存入缓冲区，接收完成后显示到显示设备上
                    // 注意，这里不可以收到一个就向显示设备发送一个,显示部分比较耗时，会导致部分串口数据丢失
                    // 循环往复直到32个文字接收满了为止(1602最大显示32个英数字)
                    // 但有可能发送的文字书小于32，但接受方没法知道发送文字的总数（通过制定协议可以解决），
                    // 故，本程序中满足下列2个条件之一就停止接收，将已接收到的内容显示出去并回到检查head的状态
                    // 1 - 离上次接收到串口的字节已经过去了指定一段时间没有新的串口数据进来,认为本次发送结束（定时器0的工作）
                    // 2 - 已接受满32个字节，强制停止接收，显示已接受的文字

                    // 收到一个字节放入缓冲区
                    strBuff[rcvCharCnt++] = buf;

                    SBUF = strBuff[rcvCharCnt-1];

                    // 重置定时器0的计数变量(这句话就相当于给单片机的看门狗喂狗一个道理)
                    // 只要有数据进来，定时器0那边的计数就不会溢出，也就不会强制停止本次接收了
                    ttCounter = 0;

                    // 已经接受满了32个字节，即使后面还有数据也不再接受
                    if (rcvCharCnt >= 32) {
                        doDisplay();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    else if(TI==1)
    {
        TI=0;
        busy = 0;
    }
    

    ES = 1;        //恢复串口中断
}

void doCommand(uchar cmd){
    // 这里就可以做业务判断处理了（作为例子，这里是点亮天祥51学习板上的1-8号LED灯）
    switch(cmd)
    {
        case 0x31:    P1=0xfe;    break;    //接受到1，第一个LED亮                 
        case 0x32:    P1=0xfd;    break;    //接受到2，第二个LED亮                
        case 0x33:    P1=0xfb;    break;    //接受到3，第三个LED亮                
        case 0x34:    P1=0xf7;    break;    //接受到4，第四个LED亮             
        case 0x35:    P1=0xef;    break;    //接受到5，第五个LED亮                        
        case 0x36:    P1=0xdf;    break;    //接受到6，第六个LED亮                                     
        case 0x37:    P1=0xbf;    break;    //接受到7，第七个LED亮
        case 0x38:    P1=0x7f;    break;    //接受到8，第八个LED亮

        case 'r':    ledR=~ledR;    break;    //接受到r
        case 'g':    ledG=~ledG;    break;    //接受到g
        case 'b':    ledB=~ledB;    break;    //接受到b

        default:        P1=0xff;    break;    //接受到其它数据
    }
}

void doDisplay(){
    
    // 末尾加上字符串的结束符号
    strBuff[rcvCharCnt] = 0x00;

    // 定时器0 停止工作
    TR0 = 0;

    // 定时器0 计数清零
    ttCounter = 0;

    // 显示缓冲区的字符串, 位置-1表示清屏，从位置0开始显示
    //LCD_PutStr(strBuff, -1);
    LCD_PutStr(strBuff, -1);

    // Debug
    //SendString(strBuff);
    //SendString("hello world!");

    // 字符数清0
    rcvCharCnt = 0;

    // 切回接收新消息模式
    checkHead_len = 0;
    uartStatus = UART_STS_CHK_HEAD;
}







void SendData(uchar dat)
{
    //while (busy);           //Wait for the completion of the previous data is sent
    busy = 1;
    SBUF = dat;             //Send data to UART buffer
}


void SendString(char *s)
{
    while (*s)              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr
    }
}