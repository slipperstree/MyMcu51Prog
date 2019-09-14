#define uchar unsigned char
#define uint unsigned int

#define FOSC 11059200L      //System frequency
#define BAUD 9600           //UART baudrate

#define HARDWARE_UART    0
#define SOFTWARE_UART    1

// Ӳ�����ں�������ڿ�ָ��
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

	//������ ������ڱ������ͽϴ��Ƽ�ʹ��11.0592M�ľ���
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
#define UART_HEAD_CMD     'S' // ����������Ϣͷ [S]
#define UART_HEAD_TXT     'T' // �����ı���Ϣͷ [T]

uchar uartStatus = UART_STS_CHK_HEAD;
uchar checkHead_len = 0;
uchar checkFoot_len = 0;
uchar uartHead = 0x00;

// 32��ascii����1�����������ַ�����������'\0' (0x00)�õ�
uchar strBuff[32+1];
// �Ѿ����յ����ַ��������ڼ���Ƿ񳬹���Χ
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

		AUXR = 0x40;        //��ʱ��1Ϊ1Tģʽ
		TMOD &= 0x0F;		//�����ʱ��1ģʽλ
		TMOD |= 0x20;		//�趨��ʱ��1Ϊģʽ2(8λ�Զ�����)

		TH1 = TL1 = -(FOSC/12/32/BAUD); //settingMode auto-reload vaule
		ET1 = 0;			//��ֹ��ʱ��1�ж�

		ES = 1;             //ʹ�ܴ����ж�
		TR1 = 1;            //��ʱ��1��ʼ����
		EA = 1;             //ʹ�����ж�
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
		�����жϷ�����������Ҫ��main������ѭ�����ã�
	*********************************************************/
	void Hard_Uart_Isr() interrupt 4
	{
		ES = 0;			//�رմ����ж�

		if (TI)
		{
			TI = 0;             //Clear transmit interrupt flag
			busy = 0;           //Clear transmit busy flag
		}
		if (RI)
		{
			rcvChar = SBUF;	//�Ӵ��ڻ�����ȡ������
			RI = 0;		//������н��ܱ�־λ
			
			SBUF = rcvChar;//��ӡ�����ڵ�����

			rcvCharProc(rcvChar);
		}

		ES = 1;			//�ָ������ж�
	}
#elif (UART_TYPE == SOFTWARE_UART)
	/*********************************************************
		���������������Ҫ��main������ѭ�����ã�
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

	// ��ʱ��0 ģ�⴮�ڷ����Ӻ���
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
	// ���ﶨ����һ����Ϣ��Э�飺
	// ��Ϣ��<������Ϣ>��<�ı���Ϣ>���֡�
	// <������Ϣ>
	// 		�����ֽڵ���Ϣͷ[S] + һ���ֽڵ���Ϣ��(��������)    (�� [S]1)
	//		���յ�������Ϣ�󣬻����doCommand������������Ϣ���1���ֽ���Ϊ���������ú�����
	//		���䣺���� [S][S]8 �������룬�������������Ϣ"8"�����ǻ��������Ϣ"["������� "S]8" �ᱻ���� 
	// <�ı���Ϣ> (���ֽ���Ϣ��Ҳ�����ڷ�������)
	//		�����ֽڵ���Ϣͷ[T] + ���32���ֽڵ���Ϣ��(��������)+ һ���ֽڵ���Ϣ������ʶ@
	//		���յ��ı���Ϣʱ������������@֮ǰ�Ὣ���ܵ���char����strBuff��������
	//		һֱ������������@ ���� �ﵽ��������С����32λ ��ֹͣ���ղ�����doMessage�������������յ�����Ϣ����ֽڸ�����Ϊ���������ú�����
	//      �ڸú����ڿɸ�����Ҫ��ȡ�����������ݡ�

	if (uartStatus == UART_STS_CHK_HEAD) {
		// ÿ���յ�һ���ֽ����ȼ���ǲ�����Ϣͷ��ǰ���ַ� [
		if (rcvChar == '[') {
			uartStatus = UART_STS_CHK_HEAD;
			checkHead_len = 1;
			//SBUF = '('; //��ӡ�����ڵ�����
			
			// ��һ����Ϣ����ַ�����0
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
			//SBUF = rcvChar; //��ӡ�����ڵ�����
		}
		else if (checkHead_len == 2 && rcvChar == ']') {
			uartStatus = UART_STS_RCV_BODY; //Э��ͷ���ɹ�����ʼ������Ϣ��
			checkHead_len = 0;
			//SBUF = ')'; //��ӡ�����ڵ�����

		} else {
			// ��Ϣͷ��;���ʧ�ܣ�����[S�ĺ��治��]���Ǳ���ַ������ж�ʧ��
			// ��Ҫ��ͷ��ʼ�ж�
			checkHead_len = 0;
		}
	}
	else if (uartStatus == UART_STS_RCV_BODY) {
		//SBUF = rcvChar; //��ӡ�����ڵ�����

		switch(uartHead)
		{
			case UART_HEAD_CMD:
				// ����ģʽ�£�ֻʹ�ý�����head�ĵ�һ���ֽڣ�������ֽڶ��������������Чhead����Ϊ����Ϣ����
				// �����ص������һ��head��״̬
				uartStatus = UART_STS_CHK_HEAD;

				// �����������
				doCommand(rcvChar);
				break;

			case UART_HEAD_TXT:
				// ������Ϣģʽ�£����յ����ַ����뻺������������ɺ�������
				// ע�⣬���ﲻ�����յ�һ��������ʾ�豸����һ��,��ʾ���ֱȽϺ�ʱ���ᵼ�²��ִ������ݶ�ʧ
				// ѭ������ֱ��32�����ֽ�������Ϊֹ
				// ���п��ܷ��͵�������С��32�������ܷ�û��֪���������ֵ�������ͨ���ƶ�Э����Խ������
				// �ʣ�����������������2������֮һ��ֹͣ���գ����ѽ��յ���������ʾ��ȥ���ص����head��״̬
				// 1 - ���յ�����λ@
				// 2 - �ѽ�����32���ֽڣ�ǿ��ֹͣ����
				if ( rcvChar == '@' || rcvCharCnt >= 32 )
				{
					// �յ�����λ��ֹͣ���գ���ʼ������������Ϣ  @����������Ϣ������
					// ĩβ�����ַ����Ľ�������
    				strBuff[rcvCharCnt] = 0x00;

					doMessage(rcvCharCnt);

					// �ַ�����0
					rcvCharCnt = 0;

					// �лؽ�������Ϣģʽ
					checkHead_len = 0;
					uartStatus = UART_STS_CHK_HEAD;

				} else {
					// ���ǽ���λ���������յ����ֽڷ��뻺����
					strBuff[rcvCharCnt++] = rcvChar;
				}
				break;
			default:
				break;
		}
	}
}

void doCommand(uchar cmd){
    // ������ҵ���жϴ���
    switch(cmd)
    {
        case 0x31:    break;    //���ܵ�1
        case 0x32:    break;    //���ܵ�2
        case 0x33:    break;    //���ܵ�3
        default:      break;    //���ܵ���������
    }
}

void doMessage(uchar rcvCnt){
	
	unsigned char i=0;
	unsigned char chkFlg=0;
	char YY, MM, DD, W, hh, mm, ss;

    // Do anything u want to do here -------
    // ������Ԥ����յ���Уʱ��Ϣ��ʽ
    // YYYYMMDDWhhmmss
    // ��15���ֽ� ��4 ��2 ��2 ����1 Сʱ2 ����2 ��2
	
    // ���ȼ���յ����ַ��������Ƿ����15
    if (rcvCnt != 15)
    {
    	// ������15 ��ʾ��Ϣ���󣬶���������

    } else {
    	// ����λ�Ƿ������֣�����κ�һλ�������֣���ʾ��Ϣ���󣬶���������
    	chkFlg = 0;
    	for (i = 0; i < rcvCnt; ++i)
	    {
	    	if (strBuff[i] < 0x30 || strBuff[i] > 0x39)
	    	{
	    		// �������ֵ��ַ�
	    		chkFlg = 1;
	    		break;
	    	}
	    }

	    if (chkFlg == 1)
	    {
	    	// �������ַ����ڣ���Ϣ���󣬶���������
	    } else {

	    	// ȡ�����ݲ�ת��������
	    	// 0  2  4  6  8 9  11 13 (��λ��ʼ�±�) 
	    	// YY YY MM DD W hh mm ss
	    	YY = (strBuff[2] -0x30) * 10 + (strBuff[3] -0x30);
	    	MM = (strBuff[4] -0x30) * 10 + (strBuff[5] -0x30);
	    	DD = (strBuff[6] -0x30) * 10 + (strBuff[7] -0x30);
	    	W  = (strBuff[8] -0x30);
	    	hh = (strBuff[9] -0x30) * 10 + (strBuff[10]-0x30);
	    	mm = (strBuff[11]-0x30) * 10 + (strBuff[12]-0x30);
	    	ss = (strBuff[13]-0x30) * 10 + (strBuff[14]-0x30);

	    	// ��У��
	    	if (MM<1 || MM>12 || DD<1 || DD>31 || W<1 || W>7 || hh>23 || mm>59 || ss>59)
	    	{
	    		// ����ʱ�����󣬶���
	    	} else {
				// ����ʱ���ʽ��ȷ��д��ʱ��оƬ�����Уʱ
				WriteTime_Sec(ss);
				WriteTime_Min(mm);
				WriteTime_Hour(hh);
				WriteTime_Day(DD);
				WriteTime_Month(MM);
				WriteTime_Year(YY);

                //SBUF = '#'; // �����ã��������ڷ��ظ�̾�ű�ʾУʱ���
	    	}
	    }
    }
}
//***************************20180325 ����Уʱ**********************************************************
