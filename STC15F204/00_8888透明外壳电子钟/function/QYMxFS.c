#include "../header/QYMxFS.h"
#include "../header/uart.h"
#include "../header/common.h"

// 文件名定义 =================================================================================
// 实际文件需要以3个数字开头，不足3位数字的前面补0，比如 013提示音.mp3

// 简单模式 
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
#define MODE_01_SND_011_DIAN        11  //点
#define MODE_01_SND_012_FEN         12  //分
#define MODE_01_SND_013_TISHIYIN    13  //提示音
#define MODE_01_SND_014_TISHIYU     14  //提示语

// 真实模式 文件名定义
#define MODE_02_SND_090_TISHIYIN    90  //提示音
#define MODE_02_SND_091_TISHIYU     91  //提示语
#define MODE_02_SND_BASE_HOUR      100  //小时部分语音文件名基准数值（hh+基准=文件名）
#define MODE_02_SND_BASE_MINUTE      0  //分钟部分语音文件名基准数值（mm+基准=文件名）
// 文件名定义 =================================================================================

// 简单模式 or 真实模式
xdata uchar mode = QYMxFS_MODE_01_SIMPLE;

// 等待播放的音频编号队列，最大不能超过16个
#define SIZE_BUF 10
xdata uchar bufSoundNo4Play[SIZE_BUF];
// 本次需要等待播放的音频一共有几条
xdata uchar totalSoundCnt = 0;
// 当前正在播放第几条
xdata uchar nowSoundIdx = 0;
// 声音文件夹（也就是选择不同的人声）
xdata uchar peopleNo = 0;
// 音量（0-30）
xdata uchar mVolume = 18;
// 忙标志
#define BUSY_YES    1
#define BUSY_NO     0
xdata uchar mBusy = BUSY_NO;

// 播放完成后模块返回的串口通知消息
// 7E       : 消息开始，固定
// FF       : 保留字节，固定
// 06       : 播放成功，固定
// 3D       : TF卡音频，固定
// 00 00 00 : 接着3个字节是文件编号，非文件名，是物理编号，基本没什么用，接收到直接丢弃
// 00 00    : 接着2个字节是校验码，每次都不一样，可自行计算校验，要求不高直接丢弃即可
// EF       : 消息结束
#define LEN_RESULT_PLAY_SUCCECSS  20
uchar code RESULT_PLAY_SUCCECSS[] = {
	0x7E, 0xFF, 0x06, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF,
    0x7E, 0xFF, 0x06, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF
};
xdata uchar rcvCharCnt = 0;

// 模块初始化, 需要在main中调用一次
void QYMxFS_init(){
    
    QYMxFS_setVolume(mVolume);
}

// Uart模块里接收到一个数据时调用该函数
void QYMxFS_rcvscan(unsigned char rcvChar){

    // 如果待检查字符是0x00则表示该位数据不做检查丢弃(每次可能不一样)
    if (RESULT_PLAY_SUCCECSS[rcvCharCnt] == 0x00 ||
        rcvChar == RESULT_PLAY_SUCCECSS[rcvCharCnt]) {
        // 检查通过，继续等待下一个串口数据
        rcvCharCnt++;

        // 整组数据接收完成，表示上一个音频已经播放完成可以继续播放下一个了
        if (rcvCharCnt == LEN_RESULT_PLAY_SUCCECSS)
        {
            rcvCharCnt = 0;

            // 检查队列中还有没有等待播放的音频，如果有则继续播放
            if (nowSoundIdx < totalSoundCnt)
            {
                // 发送串口指令播放下一条语音
                QYMxFS_sndPlaySound(bufSoundNo4Play[nowSoundIdx++]);
            } else {
                // 队列已经全部播放完了，停止播放
            }
        }
    } else {
        // 检查不通过，重头开始等待
        rcvCharCnt = 0;
    }
}

// CMD:控制指令
// feedback:是否需要反馈
// dat1:datah
// dat2:datal
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
    QYMxFS_SendCMD_NOCHECK(0x0F , 0, peopleNo, sndNo);
}

// 添加待播放的语音到队列中
void addSoundList4Play(unsigned char sndNo){
    if (totalSoundCnt >= SIZE_BUF)
    {
        // 超过缓冲队列长度，丢弃
    } else {
        bufSoundNo4Play[totalSoundCnt++] = sndNo;
    }
}

void QYMxFS_speek(unsigned char h, unsigned char m){

    // if (mBusy == BUSY_YES) return;

    // 清除播放位置，下次重头播放
    totalSoundCnt = 0;
    nowSoundIdx = 0;
    rcvCharCnt = 0;

    // //根据时间调整音量大小
    // if (h>7 && h<19)
    // {
    //     QYMxFS_setVolume(25);

    // } else {
    //     QYMxFS_setVolume(18);
    // }

    switch (mode)
    {
    case QYMxFS_MODE_02_REAL:
        //报提示音
        //addSoundList4Play( MODE_02_SND_090_TISHIYIN );
        
        //报提示语
        addSoundList4Play( MODE_02_SND_091_TISHIYU );

        //报小时
        addSoundList4Play( MODE_02_SND_BASE_HOUR + h );

        //报分钟
        addSoundList4Play( MODE_02_SND_BASE_MINUTE + m );

        break;

    case QYMxFS_MODE_01_SIMPLE:
        //报提示音
        addSoundList4Play( MODE_01_SND_013_TISHIYIN );
        
        //报提示语
        addSoundList4Play( MODE_01_SND_014_TISHIYU );
        
        //报小时
        if (h <= 10) { 
            //如果小时数小于等于10就直接播放对应的声音文件
            addSoundList4Play( h );
        }
        else if ( h < 20 ) {
            //如果小时大于10小于20则先报“十” 再报个位数
            addSoundList4Play( MODE_01_SND_010 );
            //如果小时为10的倍数则不报个位数
            if (h % 10 > 0){
                addSoundList4Play( h % 10);
            }
        } 
        else {
            //否则先报十位数再报“十”最后报个位数
            addSoundList4Play( h / 10);
            addSoundList4Play( MODE_01_SND_010 );
            //如果小时为10的倍数则不报个位数
            if (h % 10 > 0){
                addSoundList4Play( h % 10);
            }
        }
        
        //报“点”
        addSoundList4Play( MODE_01_SND_011_DIAN );
        
        //报分钟
        if ( m == 0 ) { 
            //如果分钟数等于0
            addSoundList4Play( MODE_01_SND_000 );
        }
        else if( m < 10 ) {  
            //如果分钟数小于10就先播放“零”再播放对应的声音文件
            addSoundList4Play( MODE_01_SND_000 );
            addSoundList4Play( m );
        }
        else {
            //否则先报十位数再报“十”
            addSoundList4Play( m / 10 );
            addSoundList4Play( MODE_01_SND_010 );
        }
        
        //如果分钟大于10且个位不为零还需要报分钟的个位数
        if ( m > 10 && m % 10 > 0) {
            addSoundList4Play( m % 10);
        }
        
        //最后报“分”
        addSoundList4Play( MODE_01_SND_012_FEN );
        break;
    default:
        break;
    }

    //开始播放第一条
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

// 设置音量（0-30 30最大 0静音）
void QYMxFS_setVolume(unsigned char pVolume){
    mVolume = pVolume;

    // if (mBusy == BUSY_YES) return;
    
    // mBusy = BUSY_YES;
    QYMxFS_SendCMD_NOCHECK(0x06 , 0, 0, mVolume);
    // delay_ms(50);
    // mBusy = BUSY_NO;
}