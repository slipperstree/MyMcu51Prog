/****************************************************************************
�����ܣ�		�ô��ڵ����������ͨ��������Ƭ���������ݡ�
				�����͵�����Ϊ1��8���ַ�ʱ���ֱ��Ӧ������Ӧ�ķ�������ܡ�
				�����������ݣ����������졣
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
#define UART_HEAD_CMD     'S' // ����������Ϣͷ [S]
#define UART_HEAD_TXT     'T' // �����ı���Ϣͷ [T]
#define UART_TAIL             'E' // ������Ϣβ [E]
uchar uartStatus = UART_STS_CHK_HEAD;
uchar checkHead_len = 0;
uchar checkFoot_len = 0;
uchar uartHead = 0x00;

uchar ttCounter = 0;

// ���ڴ�ӡ �ö��� ---------------------------------------------------------
bit busy;
void SendData(uchar dat);
void SendString(char *s);
// ���ڴ�ӡ �ö��� ---------------------------------------------------------

void doCommand(uchar);
void doDisplay();

// 1602 �ö��� ---------------------------------------------------------

// 1602����ʾ32��ascii����1�����������ַ�����������'\0' (0x00)�õ�
char strBuff[32+1];

// �Ѿ����յ�����ʾ���ַ��������ڼ���Ƿ񳬹���ʾ���ܷ�Χ���Լ�������ʾλ��
uchar rcvCharCnt = 0;

// TX��������,��ֲ����İ����ϲ���Ҫ
sbit dula=P2^6;
sbit wela=P2^7;
// TX��������,��ֲ����İ����ϲ���Ҫ


// AUXR��λ���òο�
// AUXR.0:S1BRS�����Ϊ1��ô����1�Ĳ����ʷ�����Ϊ���������ʷ�����������Ϊ��ʱ��1
// AUXR.1:EXTRAM��Ϊ1�����ʹ���ڲ���չRAM
// AUXR.2:BRTx12��Ϊ1����������ʷ���������Ƶ������12��Ƶ��
// AUXR.3:S2SMOD��Ϊ1����2�����ʼӱ������򲻼ӱ�
// AUXR.4:BRTR��Ϊ1���������������������������
// AUXR.5:UART_M0x6��Ϊ1�򴮿�ģʽ0Ϊ2��Ƶ������Ϊ12��Ƶ
// AUXR.6:T1x12��Ϊ1��ʱ��1����Ƶ������12��Ƶ
// AUXR.7:T0x12��Ϊ1��ʱ��0����Ƶ������12��Ƶ

void Timer0Init(void)       //10����@11.0592MHz
{
    AUXR &= 0x7F;       //��ʱ��0ʱ��12Tģʽ 0111 1111 // AUXR.bit 7:T0x12��Ϊ1��ʱ��0����Ƶ������12��Ƶ
    TMOD &= 0xF0;       //���ö�ʱ��0ģʽ
    TMOD |= 0x01;       //���ö�ʱ��0ģʽ
    TL0 = 0x00;     //���ö�ʱ��ֵ
    TH0 = 0xDC;     //���ö�ʱ��ֵ
    TF0 = 0;        //���TF0��־
}

void UartInit(void)     //9600bps@11.0592MHz
{
    PCON &= 0x7F;     //�����ʲ�����
    SCON = 0x50;        //8λ����,�ɱ䲨����
    AUXR &= 0xBF;     //��ʱ��1ʱ��ΪFosc/12,��12T 1011 1111   // AUXR.6:T1x12��Ϊ1��ʱ��1����Ƶ������12��Ƶ
    AUXR &= 0xFE;     //����1ѡ��ʱ��1Ϊ�����ʷ����� 1111 1110 // AUXR.0:S1BRS�����Ϊ1��ô����1�Ĳ����ʷ�����Ϊ���������ʷ�����������Ϊ��ʱ��1
    TMOD &= 0x0F;     //�����ʱ��1ģʽλ
    TMOD |= 0x20;     //�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
    TL1 = 0xFD;     //�趨��ʱ��ֵ
    TH1 = 0xFD;     //�趨��ʱ����װֵ
    ET1 = 0;        //��ֹ��ʱ��1�ж�
    TR1 = 1;        //������ʱ��1
}

void main(void) 
{
	int p;
	
	// TX��������,��ֲ����İ����ϲ���Ҫ
	// TX1C�����ϵ�P0���� LCD1602��7������ܹ��õ�
	// ����LCD��ʱ��,����Ѹ���������ӵĵط���������Ӱ��LCD(��������,λ������)
	// �õ�������������,�����鿪ʼ����������,���ڿ���,����һ������,Ӳ����ƺܶ�Ҳ������,���������ݵ�Ӣ��data��Ȼд��date,��...
	dula = 0; wela = 0;
	
	P1=0xff;
	
	LCD_Init();
    Timer0Init();
	UartInit();

    // �������Ǳ���ģ�����STCISP���û���Զ����ɣ��ǵü���
    ES = 1;         //�������ж�
    ET0 = 1;        //����ʱ��0�ж�
    ET1 = 0;        //��ֹ��ʱ��1�жϣ���Ϊ��ʱ��1��Ϊ����1�Ĳ����ʷ������ˣ�
    EA = 1;         //�����ж�

	
	//p=LCD_PutStr("=    Hi! It\'s me =\n",-1);
	//p=LCD_PutStr("    Robot ver ",p);
	p=LCD_PutNum(100, 17);
	
	while(1);
}

/*********************************************************
    ��ʱ��0�жϷ�����
*********************************************************/
void tm0_isr() interrupt 1
{
    // 10�������һ��
    TL0 = 0x00;     //���ö�ʱ��ֵ
    TH0 = 0xDC;     //���ö�ʱ��ֵ
    ttCounter++;

    if (ttCounter >= 20)          //10ms * 20 -> 200ms
    {
        ttCounter = 0;
        doDisplay();
    }
}

/*********************************************************
    �����жϷ�����
*********************************************************/
void serial() interrupt 4 
{
    ES = 0;                                //�رմ����ж�

    if(RI==1)
    {
        RI = 0;                                //������н��ܱ�־λ
        buf = SBUF;                        //�Ӵ��ڻ�����ȡ������

        // ���ﶨ����һ����Ϣ��Э�飺
        // ÿ����Ч����ϢΪ4���ֽ�:
        // �����ֽڵ���Ϣͷ[S] + һ���ֽڵ���Ϣ��    (�� [S]1)
        // �Ժ�������ֽ���Ϣ���ʱ���Ǽ�����Ϣβ[E]
        // Ŀǰ������ [S][S]8 �������룬�������������Ϣ"8"�����ǻ��������Ϣ"["������� "S]8" �ᱻ���� 

        if (uartStatus == UART_STS_CHK_HEAD) {
            // ÿ���յ�һ���ֽ����ȼ���ǲ�����Ϣͷ��ǰ���ַ� [
            if (buf == '[') {
                uartStatus = UART_STS_CHK_HEAD;
                checkHead_len = 1;
                SBUF = '('; //��ӡ�����ڵ�����
                
                // ��һ����Ϣ����ַ�����0
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
                SBUF = buf; //��ӡ�����ڵ�����
            }
            else if (checkHead_len == 2 && buf == ']') {
                uartStatus = UART_STS_RCV_BODY; //Э��ͷ���ɹ�����ʼ������Ϣ��
                checkHead_len = 0;
                SBUF = ')'; //��ӡ�����ڵ�����

                // ����ǿ�ʼ����[T]�ַ�������������ʱ��0��
                // �Լ��ʲôʱ�����ݽ��ս�����һ��ʱ���ڲ��������ݽ�������Ϊ���ͽ����ˣ�
                if (uartHead == UART_HEAD_TXT) TR0 = 1;
            } else {
                // ��Ϣͷ��;���ʧ�ܣ�����[S�ĺ��治��]���Ǳ���ַ������ж�ʧ��
                // ��Ҫ��ͷ��ʼ�ж�
                checkHead_len = 0;
            }
        }
        else if (uartStatus == UART_STS_RCV_BODY) {
            //SBUF = buf; //��ӡ�����ڵ�����

            switch(uartHead)
            {
                case UART_HEAD_CMD:
                    // ����ģʽ�£�ֻʹ�ý�����head�ĵ�һ���ֽڣ�������ֽڶ��������������Чhead����Ϊ����Ϣ����
                    // �����ص������һ��head��״̬
                    uartStatus = UART_STS_CHK_HEAD;

                    // �����������
                    doCommand(buf);
                    break;

                case UART_HEAD_TXT:
                    // ������Ϣģʽ�£����յ����ַ����뻺������������ɺ���ʾ����ʾ�豸��
                    // ע�⣬���ﲻ�����յ�һ��������ʾ�豸����һ��,��ʾ���ֱȽϺ�ʱ���ᵼ�²��ִ������ݶ�ʧ
                    // ѭ������ֱ��32�����ֽ�������Ϊֹ(1602�����ʾ32��Ӣ����)
                    // ���п��ܷ��͵�������С��32�������ܷ�û��֪���������ֵ�������ͨ���ƶ�Э����Խ������
                    // �ʣ�����������������2������֮һ��ֹͣ���գ����ѽ��յ���������ʾ��ȥ���ص����head��״̬
                    // 1 - ���ϴν��յ����ڵ��ֽ��Ѿ���ȥ��ָ��һ��ʱ��û���µĴ������ݽ���,��Ϊ���η��ͽ�������ʱ��0�Ĺ�����
                    // 2 - �ѽ�����32���ֽڣ�ǿ��ֹͣ���գ���ʾ�ѽ��ܵ�����

                    // �յ�һ���ֽڷ��뻺����
                    strBuff[rcvCharCnt++] = buf;

                    SBUF = strBuff[rcvCharCnt-1];

                    // ���ö�ʱ��0�ļ�������(��仰���൱�ڸ���Ƭ���Ŀ��Ź�ι��һ������)
                    // ֻҪ�����ݽ�������ʱ��0�Ǳߵļ����Ͳ��������Ҳ�Ͳ���ǿ��ֹͣ���ν�����
                    ttCounter = 0;

                    // �Ѿ���������32���ֽڣ���ʹ���滹������Ҳ���ٽ���
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
    

    ES = 1;        //�ָ������ж�
}

void doCommand(uchar cmd){
    // ����Ϳ�����ҵ���жϴ����ˣ���Ϊ���ӣ������ǵ�������51ѧϰ���ϵ�1-8��LED�ƣ�
    switch(cmd)
    {
        case 0x31:    P1=0xfe;    break;    //���ܵ�1����һ��LED��                 
        case 0x32:    P1=0xfd;    break;    //���ܵ�2���ڶ���LED��                
        case 0x33:    P1=0xfb;    break;    //���ܵ�3��������LED��                
        case 0x34:    P1=0xf7;    break;    //���ܵ�4�����ĸ�LED��             
        case 0x35:    P1=0xef;    break;    //���ܵ�5�������LED��                        
        case 0x36:    P1=0xdf;    break;    //���ܵ�6��������LED��                                     
        case 0x37:    P1=0xbf;    break;    //���ܵ�7�����߸�LED��
        case 0x38:    P1=0x7f;    break;    //���ܵ�8���ڰ˸�LED��

        case 'r':    ledR=~ledR;    break;    //���ܵ�r
        case 'g':    ledG=~ledG;    break;    //���ܵ�g
        case 'b':    ledB=~ledB;    break;    //���ܵ�b

        default:        P1=0xff;    break;    //���ܵ���������
    }
}

void doDisplay(){
    
    // ĩβ�����ַ����Ľ�������
    strBuff[rcvCharCnt] = 0x00;

    // ��ʱ��0 ֹͣ����
    TR0 = 0;

    // ��ʱ��0 ��������
    ttCounter = 0;

    // ��ʾ���������ַ���, λ��-1��ʾ��������λ��0��ʼ��ʾ
    //LCD_PutStr(strBuff, -1);
    LCD_PutStr(strBuff, -1);

    // Debug
    //SendString(strBuff);
    //SendString("hello world!");

    // �ַ�����0
    rcvCharCnt = 0;

    // �лؽ�������Ϣģʽ
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