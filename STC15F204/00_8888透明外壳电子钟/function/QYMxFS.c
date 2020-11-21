#include "../header/QYMxFS.h"
#include "../header/uart.h"
#include "../header/common.h"

// �ļ������� =================================================================================
// ʵ���ļ���Ҫ��3�����ֿ�ͷ������3λ���ֵ�ǰ�油0������ 013��ʾ��.mp3

// ��ģʽ 
#define MODE_01_SND_000        0
#define MODE_01_SND_001        1
#define MODE_01_SND_002        2
#define MODE_01_SND_003        3
#define MODE_01_SND_004        4
#define MODE_01_SND_005        5
#define MODE_01_SND_006        6
#define MODE_01_SND_007        7
#define MODE_01_SND_008        8
#define MODE_01_SND_009        9
#define MODE_01_SND_010        10
#define MODE_01_SND_011_DIAN        11  //��
#define MODE_01_SND_012_FEN         12  //��
#define MODE_01_SND_013_TISHIYIN    13  //��ʾ��
#define MODE_01_SND_014_TISHIYU     14  //��ʾ��

// ��ʵģʽ �ļ�������
#define MODE_02_SND_090_TISHIYIN    90  //��ʾ��
#define MODE_02_SND_091_TISHIYU     91  //��ʾ��
#define MODE_02_SND_BASE_HOUR      100  //Сʱ���������ļ�����׼��ֵ��hh+��׼=�ļ�����
#define MODE_02_SND_BASE_MINUTE      0  //���Ӳ��������ļ�����׼��ֵ��mm+��׼=�ļ�����
// �ļ������� =================================================================================

// ��ģʽ or ��ʵģʽ
xdata uchar mode = QYMxFS_MODE_01_SIMPLE;

// �ȴ����ŵ���Ƶ��Ŷ��У�����ܳ���16��
#define SIZE_BUF 10
xdata uchar bufSoundNo4Play[SIZE_BUF];
// ������Ҫ�ȴ����ŵ���Ƶһ���м���
xdata uchar totalSoundCnt = 0;
// ��ǰ���ڲ��ŵڼ���
xdata uchar nowSoundIdx = 0;
// �����ļ��У�Ҳ����ѡ��ͬ��������
xdata uchar peopleNo = 0;
// ������0-30��
xdata uchar mVolume = 18;
// æ��־
#define BUSY_YES    1
#define BUSY_NO     0
xdata uchar mBusy = BUSY_NO;

// ������ɺ�ģ�鷵�صĴ���֪ͨ��Ϣ
// 7E       : ��Ϣ��ʼ���̶�
// FF       : �����ֽڣ��̶�
// 06       : ���ųɹ����̶�
// 3D       : TF����Ƶ���̶�
// 00 00 00 : ����3���ֽ����ļ���ţ����ļ������������ţ�����ûʲô�ã����յ�ֱ�Ӷ���
// 00 00    : ����2���ֽ���У���룬ÿ�ζ���һ���������м���У�飬Ҫ�󲻸�ֱ�Ӷ�������
// EF       : ��Ϣ����
#define LEN_RESULT_PLAY_SUCCECSS  20
uchar code RESULT_PLAY_SUCCECSS[] = {
	0x7E, 0xFF, 0x06, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF,
    0x7E, 0xFF, 0x06, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF
};
xdata uchar rcvCharCnt = 0;

// ģ���ʼ��, ��Ҫ��main�е���һ��
void QYMxFS_init(){
    
    QYMxFS_setVolume(mVolume);
}

// Uartģ������յ�һ������ʱ���øú���
void QYMxFS_rcvscan(unsigned char rcvChar){

    // ���������ַ���0x00���ʾ��λ���ݲ�����鶪��(ÿ�ο��ܲ�һ��)
    if (RESULT_PLAY_SUCCECSS[rcvCharCnt] == 0x00 ||
        rcvChar == RESULT_PLAY_SUCCECSS[rcvCharCnt]) {
        // ���ͨ���������ȴ���һ����������
        rcvCharCnt++;

        // �������ݽ�����ɣ���ʾ��һ����Ƶ�Ѿ�������ɿ��Լ���������һ����
        if (rcvCharCnt == LEN_RESULT_PLAY_SUCCECSS)
        {
            rcvCharCnt = 0;

            // �������л���û�еȴ����ŵ���Ƶ����������������
            if (nowSoundIdx < totalSoundCnt)
            {
                // ���ʹ���ָ�����һ������
                QYMxFS_sndPlaySound(bufSoundNo4Play[nowSoundIdx++]);
            } else {
                // �����Ѿ�ȫ���������ˣ�ֹͣ����
            }
        }
    } else {
        // ��鲻ͨ������ͷ��ʼ�ȴ�
        rcvCharCnt = 0;
    }
}

// CMD:����ָ��
// feedback:�Ƿ���Ҫ����
// dat1:datah
// dat2:datal
void QYMxFS_SendCMD_NOCHECK(
    unsigned char CMD,
    unsigned char feedback,
    unsigned char dat1,
    unsigned char dat2)
{
    //��ʼ
    UART_SendByte(0x7E);

    //����
    UART_SendByte(0xFF);        //�����ֽ�
    UART_SendByte(0x06);        //����
    UART_SendByte(CMD);         //����ָ��
    UART_SendByte(feedback);    //�Ƿ���Ҫ����
    UART_SendByte(dat1);        //datah
    UART_SendByte(dat2);        //datal

    //����
    UART_SendByte(0xEF);
}

void QYMxFS_sndPlaySound(unsigned char sndNo) {

    // 7E FF 06 0F 00 02 01 EF   : ����02�ļ����µ�001.mp3
    // 7E FF 06 0F 00 02 05 EF   : ����02�ļ����µ�005.mp3
    QYMxFS_SendCMD_NOCHECK(0x0F , 0, peopleNo, sndNo);
}

// ��Ӵ����ŵ�������������
void addSoundList4Play(unsigned char sndNo){
    if (totalSoundCnt >= SIZE_BUF)
    {
        // ����������г��ȣ�����
    } else {
        bufSoundNo4Play[totalSoundCnt++] = sndNo;
    }
}

void QYMxFS_speek(unsigned char h, unsigned char m){

    // if (mBusy == BUSY_YES) return;

    // �������λ�ã��´���ͷ����
    totalSoundCnt = 0;
    nowSoundIdx = 0;
    rcvCharCnt = 0;

    // //����ʱ�����������С
    // if (h>7 && h<19)
    // {
    //     QYMxFS_setVolume(25);

    // } else {
    //     QYMxFS_setVolume(18);
    // }

    switch (mode)
    {
    case QYMxFS_MODE_02_REAL:
        //����ʾ��
        //addSoundList4Play( MODE_02_SND_090_TISHIYIN );
        
        //����ʾ��
        addSoundList4Play( MODE_02_SND_091_TISHIYU );

        //��Сʱ
        addSoundList4Play( MODE_02_SND_BASE_HOUR + h );

        //������
        addSoundList4Play( MODE_02_SND_BASE_MINUTE + m );

        break;

    case QYMxFS_MODE_01_SIMPLE:
        //����ʾ��
        addSoundList4Play( MODE_01_SND_013_TISHIYIN );
        
        //����ʾ��
        addSoundList4Play( MODE_01_SND_014_TISHIYU );
        
        //��Сʱ
        if (h <= 10) { 
            //���Сʱ��С�ڵ���10��ֱ�Ӳ��Ŷ�Ӧ�������ļ�
            addSoundList4Play( h );
        }
        else if ( h < 20 ) {
            //���Сʱ����10С��20���ȱ���ʮ�� �ٱ���λ��
            addSoundList4Play( MODE_01_SND_010 );
            //���СʱΪ10�ı����򲻱���λ��
            if (h % 10 > 0){
                addSoundList4Play( h % 10);
            }
        } 
        else {
            //�����ȱ�ʮλ���ٱ���ʮ����󱨸�λ��
            addSoundList4Play( h / 10);
            addSoundList4Play( MODE_01_SND_010 );
            //���СʱΪ10�ı����򲻱���λ��
            if (h % 10 > 0){
                addSoundList4Play( h % 10);
            }
        }
        
        //�����㡱
        addSoundList4Play( MODE_01_SND_011_DIAN );
        
        //������
        if ( m == 0 ) { 
            //�������������0
            addSoundList4Play( MODE_01_SND_000 );
        }
        else if( m < 10 ) {  
            //���������С��10���Ȳ��š��㡱�ٲ��Ŷ�Ӧ�������ļ�
            addSoundList4Play( MODE_01_SND_000 );
            addSoundList4Play( m );
        }
        else {
            //�����ȱ�ʮλ���ٱ���ʮ��
            addSoundList4Play( m / 10 );
            addSoundList4Play( MODE_01_SND_010 );
        }
        
        //������Ӵ���10�Ҹ�λ��Ϊ�㻹��Ҫ�����ӵĸ�λ��
        if ( m > 10 && m % 10 > 0) {
            addSoundList4Play( m % 10);
        }
        
        //��󱨡��֡�
        addSoundList4Play( MODE_01_SND_012_FEN );
        break;
    default:
        break;
    }

    //��ʼ���ŵ�һ��
    QYMxFS_sndPlaySound(bufSoundNo4Play[nowSoundIdx++]);
}

xdata uchar lastHour = 25;
void QYMxFS_speekEveryHour(unsigned char h, unsigned char m){
    if (m == 0 && h != lastHour)
    {
        QYMxFS_setPeople(11);
        QYMxFS_setMode( QYMxFS_MODE_02_REAL );

        QYMxFS_speek(h, m);
        lastHour = h;
    }
}

void QYMxFS_setPeople(unsigned char pNo){
    peopleNo = pNo;
}

void QYMxFS_setMode(unsigned char pMode){
    mode = pMode;
}

// ����������0-30 30��� 0������
void QYMxFS_setVolume(unsigned char pVolume){
    mVolume = pVolume;

    // if (mBusy == BUSY_YES) return;
    
    // mBusy = BUSY_YES;
    QYMxFS_SendCMD_NOCHECK(0x06 , 0, 0, mVolume);
    // delay_ms(50);
    // mBusy = BUSY_NO;
}