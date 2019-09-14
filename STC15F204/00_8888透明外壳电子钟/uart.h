#define uchar unsigned char
#define uint unsigned int

#define FOSC 11059200L      //System frequency
#define BAUD 9600           //UART baudrate

#define HARDWARE_UART    0
#define SOFTWARE_UART    1

// 硬件串口和软件串口可指定
#define UART_TYPE SOFTWARE_UART

#if (UART_TYPE == HARDWARE_UART)

	/*Define UART parity mode*/
	#define NONE_PARITY     0   //None parity
	#define ODD_PARITY      1   //Odd parity
	#define EVEN_PARITY     2   //Even parity
	#define MARK_PARITY     3   //Mark parity
	#define SPACE_PARITY    4   //Space parity

	#define PARITYBIT NONE_PARITY   //Testing even parity

	bit busy;
#elif (UART_TYPE == SOFTWARE_UART)
	//BAUD_TL = 65536 - FOSC/3/BAUDRATE/M (1T:M=1; 12T:M=12)
	//NOTE: (FOSC/3/BAUDRATE) must be greater then 98, (RECOMMEND GREATER THEN 110)

	//波特率 软件串口本来误差就较大，推荐使用11.0592M的晶振
	//#define BAUD_TL  0xF400                  // 1200bps @ 11.0592MHz
	//#define BAUD_TL  0xFA00                  // 2400bps @ 11.0592MHz
	//#define BAUD_TL  0xFD00                  // 4800bps @ 11.0592MHz
	//#define BAUD_TL  0xFE80                  // 9600bps @ 11.0592MHz
	//#define BAUD_TL  0xFF40                  //19200bps @ 11.0592MHz
	//#define BAUD_TL  0xFFA0                  //38400bps @ 11.0592MHz

	sbit RXB = P3^0;                        //define UART TX/RX port
	sbit TXB = P3^1;

	typedef bit BOOL;
	typedef unsigned char BYTE;
	typedef unsigned int WORD;

	BYTE TBUF,RBUF;
	BYTE TDAT,RDAT;
	BYTE TCNT,RCNT;
	BYTE TBIT,RBIT;
	BOOL TING,RING;
	BOOL TEND,REND;

	void UART_INIT();

	BYTE t, r;
	BYTE buf[16];
#endif

uchar rcvChar;
uchar bufS[3];
uchar bufE[3];

#define UART_STS_CHK_HEAD     0
#define UART_STS_RCV_BODY     1
#define UART_STS_CHK_FOOT     2
#define UART_HEAD_CMD     'S' // 定义命令消息头 [S]
#define UART_HEAD_TXT     'T' // 定义文本消息头 [T]

uchar uartStatus = UART_STS_CHK_HEAD;
uchar checkHead_len = 0;
uchar checkFoot_len = 0;
uchar uartHead = 0x00;

// 32个ascii，加1是用来保存字符串结束符号'\0' (0x00)用的
uchar strBuff[32+1];
// 已经接收到的字符数，用于检测是否超过范围
uchar rcvCharCnt = 0;

void rcvCharProc(uchar rcvChar);
void doMessage(uchar);
void doCommand(uchar);

void uart_init()
{
	#if (UART_TYPE == HARDWARE_UART)
		#if (PARITYBIT == NONE_PARITY)
			SCON = 0x50;            //8-bit variable UART
		#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
			SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
		#elif (PARITYBIT == SPACE_PARITY)
			SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
		#endif

		EA = 0;

		AUXR = 0x40;        //定时器1为1T模式
		TMOD &= 0x0F;		//清除定时器1模式位
		TMOD |= 0x20;		//设定定时器1为模式2(8位自动重载)

		TH1 = TL1 = -(FOSC/12/32/BAUD); //settingMode auto-reload vaule
		ET1 = 0;			//禁止定时器1中断

		ES = 1;             //使能串口中断
		TR1 = 1;            //定时器1开始工作
		EA = 1;             //使能总中断
	#elif (UART_TYPE == SOFTWARE_UART)
		TING = 0;
		RING = 0;
		TEND = 1;
		REND = 0;
		TCNT = 0;
		RCNT = 0;

		TMOD = 0x00;                        //timer0 in 16-bit auto reload mode
		AUXR = 0x80;                        //timer0 working at 1T mode

		//BAUD_TL = 65536 - FOSC/3/BAUDRATE/M (1T:M=1; 12T:M=12)
		//TL0 = BAUD_TL;
		//TH0 = BAUD_TL>>8;                      //initial timer0 and set reload value
		TL0 = 65536 - (FOSC/BAUD/3);
		TH0 = (65536 - (FOSC/BAUD/3))>>8;

		TR0 = 1;                            //tiemr0 start running
		ET0 = 1;                            //enable timer0 interrupt
		PT0 = 1;                            //improve timer0 interrupt priority
		EA = 1;                             //open global interrupt switch
	#endif
}

#if (UART_TYPE == HARDWARE_UART)
	/*********************************************************
		串行中断服务函数（不需要在main函数里循环调用）
	*********************************************************/
	void Hard_Uart_Isr() interrupt 4
	{
		ES = 0;			//关闭串行中断

		if (TI)
		{
			TI = 0;             //Clear transmit interrupt flag
			busy = 0;           //Clear transmit busy flag
		}
		if (RI)
		{
			rcvChar = SBUF;	//从串口缓冲区取得数据
			RI = 0;		//清除串行接受标志位
			
			SBUF = rcvChar;//打印给串口调试用

			rcvCharProc(rcvChar);
		}

		ES = 1;			//恢复串口中断
	}
#elif (UART_TYPE == SOFTWARE_UART)
	/*********************************************************
		串行软服务函数（需要在main函数里循环调用）
	*********************************************************/
	void Soft_Uart_Isr()
	{
		if (REND)
        {
            //buf[r++ & 0x0f] = RBUF;

			rcvCharProc(RBUF);
			
			REND = 0;
        }
        if (TEND)
        {
            if (t != r)
            {
                TEND = 0;
                TBUF = buf[t++ & 0x0f];
                TING = 1;
            }
        }
	}

	// 定时器0 模拟串口服务子函数
	void tm0() interrupt 1 using 1
	{
		if (RING)
		{
			if (--RCNT == 0)
			{
				RCNT = 3;                   //reset send baudrate counter
				if (--RBIT == 0)
				{
					RBUF = RDAT;            //save the data to RBUF
					RING = 0;               //stop receive
					REND = 1;               //set receive completed flag
				}
				else
				{
					RDAT >>= 1;
					if (RXB) RDAT |= 0x80;  //shift RX data to RX buffer
				}
			}
		}
		else if (!RXB)
		{
			RING = 1;                       //set start receive flag
			RCNT = 4;                       //initial receive baudrate counter
			RBIT = 9;                       //initial receive bit number (8 data bits + 1 stop bit)
		}

		if (--TCNT == 0)
		{
			TCNT = 3;                       //reset send baudrate counter
			if (TING)                       //judge whether sending
			{
				if (TBIT == 0)
				{
					TXB = 0;                //send start bit
					TDAT = TBUF;            //load data from TBUF to TDAT
					TBIT = 9;               //initial send bit number (8 data bits + 1 stop bit)
				}
				else
				{
					TDAT >>= 1;             //shift data to CY
					if (--TBIT == 0)
					{
						TXB = 1;
						TING = 0;           //stop send
						TEND = 1;           //set send completed flag
					}
					else
					{
						TXB = CY;           //write CY to TX port
					}
				}
			}
		}
	}
#endif

void rcvCharProc(uchar rcvChar) {
	// 这里定义了一个消息的协议：
	// 消息分<命令消息>和<文本消息>两种。
	// <命令消息>
	// 		三个字节的消息头[S] + 一个字节的消息体(内容任意)    (如 [S]1)
	//		接收到命令消息后，会调用doCommand函数，并将消息体的1个字节作为参数传给该函数。
	//		补充：形如 [S][S]8 这种误码，并不会解析成消息"8"，而是会解析成消息"["，后面的 "S]8" 会被丢弃 
	// <文本消息> (多字节消息，也可用于发送数据)
	//		三个字节的消息头[T] + 最多32个字节的消息体(内容任意)+ 一个字节的消息结束标识@
	//		接收到文本消息时在遇到结束符@之前会将接受到的char存入strBuff缓冲区。
	//		一直到遇到结束符@ 或者 达到缓冲区大小上限32位 会停止接收并调用doMessage函数，并将接收到的消息体的字节个数作为参数传给该函数。
	//      在该函数内可根据需要读取缓冲区的内容。

	if (uartStatus == UART_STS_CHK_HEAD) {
		// 每接收到一个字节首先检查是不是消息头的前导字符 [
		if (rcvChar == '[') {
			uartStatus = UART_STS_CHK_HEAD;
			checkHead_len = 1;
			//SBUF = '('; //打印给串口调试用
			
			// 上一个消息体的字符数清0
			rcvCharCnt = 0;
		}
		else if (checkHead_len == 1 && ( 
					rcvChar == UART_HEAD_CMD ||
					rcvChar == UART_HEAD_TXT
					)
				)
		{
			checkHead_len = 2;
			uartHead = rcvChar;
			//SBUF = rcvChar; //打印给串口调试用
		}
		else if (checkHead_len == 2 && rcvChar == ']') {
			uartStatus = UART_STS_RCV_BODY; //协议头检查成功，开始接受消息体
			checkHead_len = 0;
			//SBUF = ')'; //打印给串口调试用

		} else {
			// 消息头中途检测失败，比如[S的后面不是]而是别的字符，则判断失败
			// 需要从头开始判断
			checkHead_len = 0;
		}
	}
	else if (uartStatus == UART_STS_RCV_BODY) {
		//SBUF = rcvChar; //打印给串口调试用

		switch(uartHead)
		{
			case UART_HEAD_CMD:
				// 命令模式下，只使用紧跟着head的第一个字节，后面的字节丢弃（如果包含有效head则视为新消息处理）
				// 继续回到检查下一个head的状态
				uartStatus = UART_STS_CHK_HEAD;

				// 调用命令处理函数
				doCommand(rcvChar);
				break;

			case UART_HEAD_TXT:
				// 文字消息模式下，接收到的字符存入缓冲区，接收完成后做处理
				// 注意，这里不可以收到一个就向显示设备发送一个,显示部分比较耗时，会导致部分串口数据丢失
				// 循环往复直到32个文字接收满了为止
				// 但有可能发送的文字书小于32，但接受方没法知道发送文字的总数（通过制定协议可以解决），
				// 故，本程序中满足下列2个条件之一就停止接收，将已接收到的内容显示出去并回到检查head的状态
				// 1 - 接收到结束位@
				// 2 - 已接受满32个字节，强制停止接收
				if ( rcvChar == '@' || rcvCharCnt >= 32 )
				{
					// 收到结束位，停止接收，开始处理接收完的消息  @本身不放入消息缓冲区
					// 末尾加上字符串的结束符号
    				strBuff[rcvCharCnt] = 0x00;

					doMessage(rcvCharCnt);

					// 字符数清0
					rcvCharCnt = 0;

					// 切回接收新消息模式
					checkHead_len = 0;
					uartStatus = UART_STS_CHK_HEAD;

				} else {
					// 不是结束位，继续将收到的字节放入缓冲区
					strBuff[rcvCharCnt++] = rcvChar;
				}
				break;
			default:
				break;
		}
	}
}

void doCommand(uchar cmd){
    // 这里做业务判断处理
    switch(cmd)
    {
        case 0x31:    break;    //接受到1
        case 0x32:    break;    //接受到2
        case 0x33:    break;    //接受到3
        default:      break;    //接受到其它数据
    }
}

void doMessage(uchar rcvCnt){
	
	unsigned char i=0;
	unsigned char chkFlg=0;
	char YY, MM, DD, W, hh, mm, ss;

    // Do anything u want to do here -------
    // 本程序预想的收到的校时消息格式
    // YYYYMMDDWhhmmss
    // 共15个字节 年4 月2 日2 星期1 小时2 分钟2 秒2
	
    // 首先检查收到的字符串长度是否等于15
    if (rcvCnt != 15)
    {
    	// 不等于15 表示消息有误，丢弃不处理

    } else {
    	// 检查各位是否都是数字，如果任何一位不是数字，表示消息有误，丢弃不处理
    	chkFlg = 0;
    	for (i = 0; i < rcvCnt; ++i)
	    {
	    	if (strBuff[i] < 0x30 || strBuff[i] > 0x39)
	    	{
	    		// 不是数字的字符
	    		chkFlg = 1;
	    		break;
	    	}
	    }

	    if (chkFlg == 1)
	    {
	    	// 非数字字符存在，消息有误，丢弃不处理
	    } else {

	    	// 取各数据并转换成数字
	    	// 0  2  4  6  8 9  11 13 (各位开始下标) 
	    	// YY YY MM DD W hh mm ss
	    	YY = (strBuff[2] -0x30) * 10 + (strBuff[3] -0x30);
	    	MM = (strBuff[4] -0x30) * 10 + (strBuff[5] -0x30);
	    	DD = (strBuff[6] -0x30) * 10 + (strBuff[7] -0x30);
	    	W  = (strBuff[8] -0x30);
	    	hh = (strBuff[9] -0x30) * 10 + (strBuff[10]-0x30);
	    	mm = (strBuff[11]-0x30) * 10 + (strBuff[12]-0x30);
	    	ss = (strBuff[13]-0x30) * 10 + (strBuff[14]-0x30);

	    	// 做校验
	    	if (MM<1 || MM>12 || DD<1 || DD>31 || W<1 || W>7 || hh>23 || mm>59 || ss>59)
	    	{
	    		// 日期时间有误，丢弃
	    	} else {
				// 日期时间格式正确，写入时钟芯片，完成校时
				WriteTime_Sec(ss);
				WriteTime_Min(mm);
				WriteTime_Hour(hh);
				WriteTime_Day(DD);
				WriteTime_Month(MM);
				WriteTime_Year(YY);

                //SBUF = '#'; // 调试用，看到串口返回感叹号表示校时完成
	    	}
	    }
    }
}
//***************************20180325 串口校时**********************************************************
