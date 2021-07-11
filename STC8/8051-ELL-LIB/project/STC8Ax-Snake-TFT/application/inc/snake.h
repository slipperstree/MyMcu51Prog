#ifndef __TXS_SNAKE_H_
#define __TXS_SNAKE_H_

#include "common.h"
#include "stdlib.h"

// ### 贪食蛇模块 定义START ####################################################################################

#define uchar unsigned char
#define uint unsigned int

// 地图大小
// 10 pixel per block
// #define SNAKE_SIZE_X 	16
// #define SNAKE_SIZE_Y	13
// 5 pixel per block
#define SNAKE_SIZE_X 	32
#define SNAKE_SIZE_Y	25

// snake_AI_moveNext 的AI算法写了好几个实现，越简单rom空间越小，根据需要放开下列4个定义中的某一个（不可同时放开）
//#define AI_LEVEL_BABY     // 娃娃(只会随机乱走，测试最好成绩11)
//#define AI_LEVEL_KIDS     // 儿童(会多预判2步，测试最好成绩32)
//#define AI_LEVEL_SMART    // 聪明(会寻路找到苹果，测试最好成绩65，平均34) // Realse+优化开到最大：rom size<8k
#define AI_LEVEL_MASTER     // 天才(会寻路找到苹果，并且同时避免进入死胡同，测试最好成绩??，平均??)

// 调试模式
//#define SNAKE_AI_DEBUG

// 游戏状态
#define GAME_STS_PLAY 		0	// 游戏中
#define GAME_STS_GAMEOVER	1	// 游戏结束
#define GAME_STS_SETTING	2	// 设置
#define GAME_STS_PAUSE		3	// 暂停

// 地图二维数组取值意义
#define STS_EMPTY 0				// 表示该点为空
#define STS_APPLE 1		        // 表示该点为食物

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

// 蛇身体的定义
// 蛇头：蛇头50+连接蛇头的蛇身方向(1-4)
// 蛇身：蛇身（10/20/30/40）+后面一块蛇身的方向(1-4)
// 蛇尾：蛇尾60+连接蛇尾的蛇身方向(1-4)
//     十位数 : 蛇头，蛇身（包含4个方向信息），蛇尾
//     个位数 : 相邻block的方向
//     如此细分是为了方便外面的绘图模块绘制更好看转弯的身体更连续的蛇，
//     对显示效果要求不高也可以简单取十位数为1-6即可

// 这个十位数的1-4跟其他的方向常量的1-4要保持一致，其他的改成6789的话这里也要改，而且不能跟蛇头蛇尾冲突
#define STS_BODY_PREV_U 10
#define STS_BODY_PREV_D 20
#define STS_BODY_PREV_L 30
#define STS_BODY_PREV_R 40
// 蛇头
#define STS_HEAD 50
// 蛇尾
#define STS_TAIL 60

#define STS_NEXT_TO_U DIRECTION_UP
#define STS_NEXT_TO_D DIRECTION_DOWN
#define STS_NEXT_TO_L DIRECTION_LEFT
#define STS_NEXT_TO_R DIRECTION_RIGHT

// 表示该点为障碍物（效果跟身体一样，只是为了以后扩展程序预留）
#define STS_WALL 90

// 当前朝向，用于自动前进
#define NOW_TOWARDS_U DIRECTION_UP	
#define NOW_TOWARDS_D DIRECTION_DOWN		
#define NOW_TOWARDS_L DIRECTION_LEFT		
#define NOW_TOWARDS_R DIRECTION_RIGHT

// 初始身体长度
#define START_BODY_LENGH 3

// 事件
#define SNAKE_EVENT_RESTART  0
#define SNAKE_EVENT_GAMEOVER 1      // 参数1：分数
#define SNAKE_EVENT_UPDATE_BLOCK 2  // 参数1：需要更新的x坐标，参数2：需要更新的y坐标
#define SNAKE_EVENT_EAT_APPLE  3    // 吃到苹果，参数1：分数
#define SNAKE_EVENT_UPDATE_AI_PATH  4    // 参数1：需要更新的x坐标，参数2：需要更新的y坐标
typedef void (*pSnakeEventFunc)(unsigned int, unsigned int, unsigned int);
void snake_init(pSnakeEventFunc);
void snake_restart(void);

uchar snake_moveUp(void);
uchar snake_moveDown(void);
uchar snake_moveLeft(void);
uchar snake_moveRight(void);

// 向当前方向前进一步
void snake_moveNext(void);

// AI自动向某个方向走一步
void snake_AI_moveNext(void);

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
    #define AI_STATUS_EMPTY 0
    #define AI_STATUS_ON_THE_WAY 1
    #define AI_STATUS_WAY_TO_DEATH 2
    #define AI_STATUS_WAY_TO_APPLE 3
    // DFS深度优先探索地图使用的『影子地图』数据类型定义
    // 每一个数据8位
    // bit[2:0]保存下一个路径的方向（1-4）其实4个方向2位就够了，不过其他代码(STS_BODY_PREV_X)也要改就先1-4了
    // bit[5:3]保存上一个路径的方向（1-4）
    // bit[6:7]
    //    0:未探索  1:在路上（正在探索）  2:已探索且是死路  3:已探索且通向苹果
    union dfsMapType
    {
        struct bit_feild
        {
            uchar next_direction:3;
            uchar prev_direction:3;
            uchar status:2;
        } subDatas;
        uchar data;
    };
#endif

void snake_reCreateFood(void);

void snake_moveAppleUp(void);
void snake_moveAppleDown(void);
void snake_moveAppleLeft(void);
void snake_moveAppleRight(void);

uchar snake_getPointData(uchar, uchar);
unsigned int snake_getNowScroe(void);

#ifdef SNAKE_AI_DEBUG
union dfsMapType snake_getPointDataAI(uchar, uchar);
#endif

#endif
