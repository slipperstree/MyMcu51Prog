#include <STC12x20xx.h>

#ifndef __TXS_SNAKE_H_
#define __TXS_SNAKE_H_

// ### 贪食蛇模块 定义START ####################################################################################

#define uchar unsigned char
#define uint unsigned int

// 地图大小
#define SIZE_X 	8
#define SIZE_Y	8

// 游戏状态
#define GAME_STS_PLAY 		0	// 游戏中
#define GAME_STS_GAMEOVER	1	// 游戏结束
#define GAME_STS_SETTING	2	// 设置
#define GAME_STS_PAUSE		3	// 暂停

// 地图二维数组取值意义
#define STS_EMPTY 0				// 表示该点为空
#define STS_APPLE 1		// 表示该点为食物
#define STS_HEAD 10				// 蛇头(前面没有身体了)
#define STS_BODY_FROM_U 11		// 蛇身，而且连着前一块身体在自己的上方
#define STS_BODY_FROM_D 12		// 蛇身，而且连着前一块身体在自己的下方
#define STS_BODY_FROM_L 13		// 蛇身，而且连着前一块身体在自己的左方
#define STS_BODY_FROM_R 14		// 蛇身，而且连着前一块身体在自己的右方
#define STS_WALL 20		// 表示该点为障碍物（效果跟身体一样，只是为了以后扩展程序预留）

// 当前朝向，用于自动前进
#define NOW_TOWARDS_U 1		
#define NOW_TOWARDS_D 2		
#define NOW_TOWARDS_L 3		
#define NOW_TOWARDS_R 4		

// 初始身体长度
#define START_BODY_LENGH 3

void snake_restart();
void snake_gameover();

void snake_moveUp();
void snake_moveDown();
void snake_moveLeft();
void snake_moveRight();
void snake_moveNext();

void snake_createfood();

void snake_moveEndBlock();

uchar snake_getRowDataForShow(uchar);

#endif