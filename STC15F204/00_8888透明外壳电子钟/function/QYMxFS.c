#include "../header/QYMxFS.h"
#include "../header/uart.h"
#include "../header/common.h"

static_idata_uchar ttflash[] = 0;

static_idata_uchar rcvCharCnt = 0;

// 播放完成后模块返回的串口通知消息
// 7E       : 消息开始，固定
// FF       : 保留字节，固定
// 06       : 播放成功，固定
// 3D       : TF卡音频，固定
// 00 00 00 : 接着3个字节是文件编号，非文件名，是物理编号，基本没什么用，接收到直接丢弃
// FE 7E    : 接着2个字节是校验码，每次都不一样，可自行计算校验，要求不高直接丢弃即可
// EF       : 消息结束
static uchar code RESULT_PLAY_SUCCECSS[] = {
	0x7E, 0xFF, 0x06, 0x3D, 0x00, 0x00, 0x00, 0xFE, 0x7E, 0xEF
};

void QYMxFS_rcvscan(unsigned char rcvChar){

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

void QYMxFS_SendCMD_NOCHECK(
    unsigned char CMD,
    unsigned char feedback,
    unsigned char dat1,
    unsigned char dat2)
{
    //开始
    UART_SendByte(0x7E);

    //数据
    UART_SendByte(0xFF);        //保留字节
    UART_SendByte(0x06);        //长度
    UART_SendByte(CMD);         //控制指令
    UART_SendByte(feedback);    //是否需要反馈
    UART_SendByte(dat1);        //datah
    UART_SendByte(dat2);        //datal

    //结束
    UART_SendByte(0xEF);
}

void QYMxFS_sndPlaySound(unsigned char sndNo) {

    // 7E FF 06 0F 00 02 01 EF   : 播放02文件夹下的001.mp3
    // 7E FF 06 0F 00 02 05 EF   : 播放02文件夹下的005.mp3
    QYMxFS_SendCMD_NOCHECK(0x0F , 0, 03, sndNo);

    // TODO 这里要等待声音播放完成（检查串口返回？或者检查播放状态脚8，播放中为低电平）
    // 为了避免在这里死等导致影响其他功能，这里注册一个通知事件，主函数轮询检查发现播放完成后回调
    // 需要弄一个播放序列缓冲区，缓冲区指定个数音频全部播放完成以后才允许播放新的序列

}

void QYMxFS_speek(unsigned char h, unsigned char m){

    // TODO 检查当前是否正在播放序列中，如果是则丢弃此次请求

    //报提示音
    QYMxFS_sndPlaySound( SND_013_TISHIYIN );
    
    //报提示语
    QYMxFS_sndPlaySound( SND_014_TISHIYU );
    
    //报小时
    if (h <= 10) { 
        //如果小时数小于等于10就直接播放对应的声音文件
        QYMxFS_sndPlaySound( h );
    }
    else if ( h < 20 ) {
        //如果小时大于10小于20则先报“十” 再报个位数
        QYMxFS_sndPlaySound( SND_010 );
        //如果小时为10的倍数则不报个位数
        if (h % 10 > 0){
            QYMxFS_sndPlaySound( h % 10);
        }
    } 
    else {
        //否则先报十位数再报“十”最后报个位数
        QYMxFS_sndPlaySound( h / 10);
        QYMxFS_sndPlaySound( SND_010 );
        //如果小时为10的倍数则不报个位数
        if (h % 10 > 0){
            QYMxFS_sndPlaySound( h % 10);
        }
    }
    
    //报“点”
    QYMxFS_sndPlaySound( SND_011_DIAN );
    
    //报分钟
    if ( m == 0 ) { 
        //如果分钟数等于0
        QYMxFS_sndPlaySound( SND_000 );
    }
    else if( m < 10 ) {  
        //如果分钟数小于10就先播放“零”再播放对应的声音文件
        QYMxFS_sndPlaySound( SND_000 );
        QYMxFS_sndPlaySound( m );
    }
    else {
        //否则先报十位数再报“十”
        QYMxFS_sndPlaySound( m / 10 );
        QYMxFS_sndPlaySound( SND_010 );
    }
    
    //如果分钟大于10且个位不为零还需要报分钟的个位数
    if ( m > 10 && m % 10 > 0) {
        QYMxFS_sndPlaySound( m % 10);
    }
    
    //最后报“分”
    QYMxFS_sndPlaySound( SND_012_FEN );
}
