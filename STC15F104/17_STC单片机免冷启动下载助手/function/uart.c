#include "../header/uart.h"
#include "../header/common.h"

#define FOSC 11059200L      //System frequency
#define BAUD 9600           //UART baudrate

#define HARDWARE_UART    0
#define SOFTWARE_UART    1
#define DUMMY_UART       3  //只定义了对外的接口保证编译通过，没有功能

// 硬件串口和软件串口可指定（如果使用软串口会占用定时器0）
#define UART_TYPE SOFTWARE_UART

#if (UART_TYPE == DUMMY_UART)
	void UART_init(){}
	void UART_SoftUartIsr(){}
	void UART_SendByte(unsigned char dat){}
	void UART_SendString(unsigned char *s){}
#else
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
		#define BAUD_TL  0xFE80                  // 9600bps @ 11.0592MHz
		//#define BAUD_TL  0xFF40                  //19200bps @ 11.0592MHz
		//#define BAUD_TL  0xFFA0                  //38400bps @ 11.0592MHz

		//define UART TX/RX port
		sbit RXB = P3^0;
		sbit TXB = P3^1;

		typedef bit BOOL;
		typedef unsigned char BYTE;
		typedef unsigned int WORD;

		static BYTE TBUF,RBUF;
		static BYTE TDAT,RDAT;
		static BYTE TCNT,RCNT;
		static BYTE TBIT,RBIT;
		static BOOL TING,RING;
		static BOOL TEND,REND;

		#define SEND_BUFF_SIZE  32
		// idata : 一般的变量(data，隐式)只能访问片内RAM的低128字节内存超过128个字节
		// 超过128字节就不允许定义一般的变量而需要显式使用idata后就可以访问片内RAM高128个字节了
		// 写这个程序时使用的单片机只有片内的256字节没有外部RAM，所以不可以使用xdata，只能使用idata
		static uchar sendBuf[SEND_BUFF_SIZE];  
		static BYTE t, r;
	#endif

	static uchar rcvChar;
	static uchar bufS[3];
	static uchar bufE[3];

	#define UART_STS_CHK_HEAD     0
	#define UART_STS_RCV_BODY     1
	#define UART_STS_CHK_FOOT     2
	#define UART_HEAD_CMD     'S' // 定义命令消息头 [S]
	#define UART_HEAD_TXT     'T' // 定义文本消息头 [T]

	static uchar uartStatus = UART_STS_CHK_HEAD;
	static uchar checkHead_len = 0;
	static uchar checkFoot_len = 0;
	static uchar uartHead = 0x00;

	// 32个ascii，加1是用来保存字符串结束符号'\0' (0x00)用的
	#define RCV_BUFF_SIZE 32
	static uchar strBuff[RCV_BUFF_SIZE+1];
	// 已经接收到的字符数，用于检测是否超过范围
	static uchar rcvCharCnt = 0;

	void rcvCharProc(uchar rcvChar);
	void doMessage(uchar);
	void doCommand(uchar);

	void UART_init()
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
			TL0 = BAUD_TL;
			TH0 = BAUD_TL>>8;                      //initial timer0 and set reload value
			//TL0 = 65536 - (FOSC/BAUD/3);
			//TH0 = (65536 - (FOSC/BAUD/3))>>8;

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

				rcvCharProc(rcvChar);
			}

			ES = 1;			//恢复串口中断
		}

		void UART_SendString(uchar *s)
		{
			//TODO
		}

	#elif (UART_TYPE == SOFTWARE_UART)
		/*********************************************************
			串行软服务函数（需要在main函数里循环调用）
		*********************************************************/
		void UART_SoftUartIsr()
		{
			if (REND)
			{
				rcvCharProc(RBUF);
				
				REND = 0;
			}
			if (TEND)
			{
				if (r != t)
				{
					TEND = 0;
					TBUF = sendBuf[t++ & 0x1f];
					TING = 1;
					// if (t >= SEND_BUFF_SIZE || buf[t] == 0x00)
					// {
					// 	// 如果到达缓冲区域上限或者到达字符串的尾部0x00则不再继续发送，并将t计数器归零
					// 	t = 0;
					// } else {
					// 	// 通知定时器0发送下一个字符
					// 	TING = 1;
					// }
				}
			}
		}

		void UART_SendByte(BYTE dat)
		{
			sendBuf[r++ & 0x1f] = dat;
		}

		void UART_SendString(uchar *s)
		{
			while (*s)              //Check the end of the string
			{
				UART_SendByte(*s++);     //Send current char and increment string ptr
			}
		}

		// 定时器0 模拟串口服务子函数
		void tm0() interrupt 1
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

					// SBUF = rcvChar; //打印给串口调试用

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
					if ( rcvChar == '@' || rcvCharCnt >= RCV_BUFF_SIZE )
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
			case 'R':
				// 收到命令【R】进行软复位
				// 实现不用冷启动下载程序，需要配合STC-ISP软件的相关功能一起使用，调试程序很方便。
				// 		STC-ISP烧录软件那边的设置方法：
				// 			左侧中部的标签栏向右切换，找到【收到用户命令后复位到ISP监控程序区】
				// 			设置好串口的波特率，并在自定义命令里输入【5b535d52】( 即命令消息 [S]R )
				// 			然后勾选两个复选框，可以达到每次只要编译成功就会自动发送命令消息让单片机进行软复位并开始下载。
				IAP_CONTR = 0x60;
				break;
			case 'D':
				// 收到命令【D】切断输出电源再恢复，让下位单片机冷启动，达到让下位的单片机可以自动下载的目的
				// 实现不用冷启动下载程序，需要配合STC-ISP软件的相关功能一起使用，调试程序很方便。
				// 		STC-ISP烧录软件那边的设置方法：
				// 			左侧中部的标签栏向右切换，找到【收到用户命令后复位到ISP监控程序区】
				// 			设置好串口的波特率，并在自定义命令里输入【5b535d44】( 即命令消息 [S]D )
				// 			然后勾选两个复选框，可以达到每次只要编译成功就会自动发送命令消息让单片机进行软复位并开始下载。
				
				// 断电一小会再上电
				// 采用的是PNP三极管做开关，IO输出低电平时导通，注意这里跟指示灯LED的通断刚好相反
				LED = 0; ONOFF = 1;
				Delay100ms_STC15Fxx_110592M(5);
				LED = 1; ONOFF = 0;
				break;
			case '0':
				P32 = 0;
				break;
			case '1':
				P32 = 1;
				break;
			default:      break;    //接受到其它数据
		}
	}

	void doMessage(uchar rcvCnt){

		// Do anything u want to do here -------
		
		// 测试用串口返回接受到的消息
		// UART_SendString(strBuff);

	}
#endif