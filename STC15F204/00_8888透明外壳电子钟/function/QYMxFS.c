#include "../header/QYMxFS.h"
#include "../header/uart.h"
#include "../header/common.h"

static_idata_uchar ttflash[] = 0;

static_idata_uchar rcvCharCnt = 0;

// ������ɺ�ģ�鷵�صĴ���֪ͨ��Ϣ
// 7E       : ��Ϣ��ʼ���̶�
// FF       : �����ֽڣ��̶�
// 06       : ���ųɹ����̶�
// 3D       : TF����Ƶ���̶�
// 00 00 00 : ����3���ֽ����ļ���ţ����ļ������������ţ�����ûʲô�ã����յ�ֱ�Ӷ���
// FE 7E    : ����2���ֽ���У���룬ÿ�ζ���һ���������м���У�飬Ҫ�󲻸�ֱ�Ӷ�������
// EF       : ��Ϣ����
static uchar code RESULT_PLAY_SUCCECSS[] = {
	0x7E, 0xFF, 0x06, 0x3D, 0x00, 0x00, 0x00, 0xFE, 0x7E, 0xEF
};

void QYMxFS_rcvscan(unsigned char rcvChar){

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
                if ( rcvChar == '@' || rcvCharCnt >= RCV_BUFF_SIZE )
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
    QYMxFS_SendCMD_NOCHECK(0x0F , 0, 03, sndNo);

    // TODO ����Ҫ�ȴ�����������ɣ���鴮�ڷ��أ����߼�鲥��״̬��8��������Ϊ�͵�ƽ��
    // Ϊ�˱������������ȵ���Ӱ���������ܣ�����ע��һ��֪ͨ�¼�����������ѯ��鷢�ֲ�����ɺ�ص�
    // ��ҪŪһ���������л�������������ָ��������Ƶȫ����������Ժ���������µ�����

}

void QYMxFS_speek(unsigned char h, unsigned char m){

    // TODO ��鵱ǰ�Ƿ����ڲ��������У�����������˴�����

    //����ʾ��
    QYMxFS_sndPlaySound( SND_013_TISHIYIN );
    
    //����ʾ��
    QYMxFS_sndPlaySound( SND_014_TISHIYU );
    
    //��Сʱ
    if (h <= 10) { 
        //���Сʱ��С�ڵ���10��ֱ�Ӳ��Ŷ�Ӧ�������ļ�
        QYMxFS_sndPlaySound( h );
    }
    else if ( h < 20 ) {
        //���Сʱ����10С��20���ȱ���ʮ�� �ٱ���λ��
        QYMxFS_sndPlaySound( SND_010 );
        //���СʱΪ10�ı����򲻱���λ��
        if (h % 10 > 0){
            QYMxFS_sndPlaySound( h % 10);
        }
    } 
    else {
        //�����ȱ�ʮλ���ٱ���ʮ����󱨸�λ��
        QYMxFS_sndPlaySound( h / 10);
        QYMxFS_sndPlaySound( SND_010 );
        //���СʱΪ10�ı����򲻱���λ��
        if (h % 10 > 0){
            QYMxFS_sndPlaySound( h % 10);
        }
    }
    
    //�����㡱
    QYMxFS_sndPlaySound( SND_011_DIAN );
    
    //������
    if ( m == 0 ) { 
        //�������������0
        QYMxFS_sndPlaySound( SND_000 );
    }
    else if( m < 10 ) {  
        //���������С��10���Ȳ��š��㡱�ٲ��Ŷ�Ӧ�������ļ�
        QYMxFS_sndPlaySound( SND_000 );
        QYMxFS_sndPlaySound( m );
    }
    else {
        //�����ȱ�ʮλ���ٱ���ʮ��
        QYMxFS_sndPlaySound( m / 10 );
        QYMxFS_sndPlaySound( SND_010 );
    }
    
    //������Ӵ���10�Ҹ�λ��Ϊ�㻹��Ҫ�����ӵĸ�λ��
    if ( m > 10 && m % 10 > 0) {
        QYMxFS_sndPlaySound( m % 10);
    }
    
    //��󱨡��֡�
    QYMxFS_sndPlaySound( SND_012_FEN );
}
