#ifndef __QYMxFS_H_
#define __QYMxFS_H_

#define QYMxFS_MODE_01_SIMPLE     1    // 简单模式，这种模式只需要录单个数字，播放效果一般
#define QYMxFS_MODE_02_REAL       2    // 真实模式，这种模式需要24个小时部分以及60个分钟部分的音频，效果好

void QYMxFS_speekEveryHour(unsigned char h, unsigned char m);
void QYMxFS_speek(unsigned char hh, unsigned char mm);

// 模块初始化, 需要在main中调用一次
void QYMxFS_init();

// Uart模块里接收到一个数据时要循环调用该函数
void QYMxFS_rcvscan(unsigned char rcvDat);

void QYMxFS_sndPlaySound(unsigned char sndNo);

void QYMxFS_setMode(unsigned char mode);

void QYMxFS_setPeople(unsigned char peopleNo);

void QYMxFS_setVolume(unsigned char pVolume);

void QYMxFS_SendCMD_NOCHECK(
    unsigned char CMD,
    unsigned char feedback,
    unsigned char dat1,
    unsigned char dat2);

#endif
