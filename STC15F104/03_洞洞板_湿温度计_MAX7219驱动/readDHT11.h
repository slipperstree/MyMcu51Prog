//#include <reg51.h>
#include<STC15F104W.h>

#ifndef __READ_DHT11_H_
#define __READ_DHT11_H_

// ### 湿温度计DHT11模块 定义START ####################################################################################
// 一次完整的数据传输为40bit,高位先出。
// 数据格式:8bit湿度整数数据+8bit湿度小数数据
//          +8bi温度整数数据+8bit温度小数数据
//          +8bit校验和
//          校验和数据等于
//             “8bit湿度整数数据+8bit湿度小数数据+8bi温度整数数据+8bit温度小数数据”
//             所得结果的末8位。

#define uchar unsigned char
#define uint unsigned int

// 定义温湿度计的data口连接的gpio口
sbit DATA_PIN = P3^2;

// 定义一个阈值，用于判断当前数据位是0还是1。
// 这个数值的设定很关键，直接关系到能不能正确取得数据。
// 这个数的大小跟晶振频率有关系，需要自己试出来。
// 意义是不停循环读取一个io口的电平状态，直到电平从高电平变成低电平为止。
// 这段时间循环的次数高于这个阈值认为是1（70us左右）反之认为是0（26us左右）
#define VAL 50

// 如果取值失败则重新尝试取值的上限次数，10次就足够了
#define RETRY 10

// 根据DHT11的硬件时序图定义各种延时所需的时间间隔(单位：微秒)
// 总线空闲状态为高电平,主机把总线拉低等待DHT11响应,
// 主机把总线拉低必须大于18毫秒,保证DHT11能检测到起始信号。
#define TIME_START 20

// 主机发送开始信号结束后,延时等待20-40us后, 读取DHT11的响应信号,
// 主机发送开始信号后,可以切换到输入模式,或者输出高电平均可, 总线由上拉电阻拉高。
// DHT11接收到主机的开始信号后,等待主机开始信号结束,然后发送80us低电平响应信号。
// 总线为低电平,说明DHT11发送响应信号,DHT11发送响应信号后,再把总线拉高80us,
// 准备发送数据,每一bit数据都以50us低电平时隙开始,高电平的长短定了数据位是0还是1。

// 循环检测高低电平时，如果超过这个值就认为检测失败或者全部数据已经发送完成。
#define MAXCNT 10000

// 如果读取响应信号为高电平,则DHT11没有响应,请检查线路是否连接正常。
// 当最后一bit数据传送完毕后，DHT11拉低总线50us,随后总线由上拉电阻拉高进入空闲状态。

// 读取DHT11的温度湿度数据
// 返回值 0：成功 1：超时失败 2：校验失败
#define READ_DHT11_RET_SUCCESS 0
#define READ_DHT11_RET_TIMEOUT 6
#define READ_DHT11_RET_CHECKESUM_WRONG 2


uchar readDHT11();
uchar getWendu();
uchar getShidu();
int getDebugCnt();

#endif