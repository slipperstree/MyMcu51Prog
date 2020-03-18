#ifndef __QYMxFS_H_
#define __QYMxFS_H_

#define SND_000        0
#define SND_001        1
#define SND_002        2
#define SND_003        3
#define SND_004        4
#define SND_005        5
#define SND_006        6
#define SND_007        7
#define SND_008        8
#define SND_009        9
#define SND_010        10
#define SND_011_DIAN        11  //点
#define SND_012_FEN         12  //分
#define SND_013_TISHIYIN    13  //提示音
#define SND_014_TISHIYU     14  //提示语

void QYMxFS_speek(unsigned char hh, unsigned char mm);

// Uart模块里接收到一个数据时要循环调用该函数
void QYMxFS_rcvscan(unsigned char rcvDat);

void QYMxFS_sndPlaySound(unsigned char sndNo);

void QYMxFS_SendCMD_NOCHECK(
    unsigned char CMD,
    unsigned char feedback,
    unsigned char dat1,
    unsigned char dat2);

#endif
