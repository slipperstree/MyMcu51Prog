#include <memoryGame.h>
#include <stdlib.h>

// 地图大小
#define SIZE_X 	8
#define SIZE_Y	8

// 游戏状态
uchar gameStatus;
#define GAME_STS_PLAY 		0	// 游戏中
#define GAME_STS_GAMEOVER	1	// 游戏结束
#define GAME_STS_SETTING	2	// 设置
#define GAME_STS_PAUSE		3	// 暂停

uchar playStatus;
#define PLAY_STS_BEFORE_SHOWLEVEL 		10	// 显示每关的title前
#define PLAY_STS_SHOWLEVEL 				11	// 显示每关的title中
#define PLAY_STS_QUEST 					13	// 显示问题中
#define PLAY_STS_WAITINPUT				14	// 等待输入回答“？”
#define PLAY_STS_ANSWER_OK				15	// 显示回答结果
#define PLAY_STS_ANSWER_NG				16	// 显示回答结果


// 难度(每次出现个数) (0 - 8)
uchar level;
#define MAX_LEVEL 9

// 速度 (0 - 4)
uchar speed;
#define MAX_SPEED 5

// 基准时间
#define TIME_BASE 1000
#define TIME_SHOWTITLE (TIME_BASE/2)
#define TIME_SPEED1 (TIME_BASE/3)
#define TIME_SPEED2 (TIME_BASE/4)
#define TIME_SPEED3 (TIME_BASE/5)
#define TIME_SPEED4 (TIME_BASE/6)
#define TIME_SPEED5 (TIME_BASE/7)
#define TIME_GAMEOVER  (TIME_BASE)
#define TIME_STOP 0 // 这是一个特殊时间，返回给主程序0表示让主程序暂停定时器，等待用户输入
uint code TIME_SPEEDS[] = {TIME_SPEED1, TIME_SPEED2, TIME_SPEED3, TIME_SPEED4, TIME_SPEED5};

// 谜题数组，存放每次随机产生的谜题序列
uchar puzzleArr[MAX_LEVEL];
#define PUZZLE_U	0
#define PUZZLE_D	1
#define PUZZLE_L	2
#define PUZZLE_R	3

// 按顺序闪现谜题的时候，当前是闪现第几个(0 - 8)
uchar nowShowingPuzzleIdx;

// 按顺序解答谜题的时候，当前是解答第几个(0 - 8)
uchar nowAwnserIdx;

/* 箭头样式1 宽空心箭头
uchar code IMG_ARROWS[4][8] = {
	{0x10,0x28,0x44,0xEE,0x28,0x28,0x28,0x38}, // U
	{0x38,0x28,0x28,0x28,0xEE,0x44,0x28,0x10}, // D
	{0x10,0x30,0x5F,0x81,0x5F,0x30,0x10,0x00}, // L
	{0x08,0x0C,0xFA,0x81,0xFA,0x0C,0x08,0x00}  // R
	};
*/

/* 箭头样式2 细长箭头
uchar code IMG_ARROWS[4][8] = {
	{0x10,0x38,0x7C,0x10,0x10,0x10,0x10,0x00}, // U
	{0x00,0x10,0x10,0x10,0x10,0x7C,0x38,0x10}, // D
	{0x00,0x20,0x60,0xFE,0x60,0x20,0x00,0x00}, // L
	{0x00,0x04,0x06,0x7F,0x06,0x04,0x00,0x00}  // R
	};
*/

uchar code IMG_ARROWS[4][8] = {
	{0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,0x00}, // U
	{0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x3C}, // D
	{0x00,0x00,0xC0,0xC0,0xC0,0xC0,0x00,0x00}, // L
	{0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x00}  // R
	};

uchar code IMG_WAITINPUT[] = {0x3C,0x46,0x42,0x1E,0x18,0x00,0x18,0x18};

uchar code IMG_ALL_ON[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uchar code IMG_ALL_OFF[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uchar code IMG_WRONG[] = {0x82,0x44,0x28,0x10,0x28,0x44,0x82,0x00};
uchar code IMG_RIGHT[] = {0x38,0x44,0x82,0x82,0x82,0x44,0x38,0x00};

uchar code IMG_GAMEOVER[] = {0x00,0x00,0x66,0x00,0x00,0x18,0x24,0x00};

uchar code IMG_SETTING_LEVEL[9][8] = {
	{0xC0,0x80,0x08,0x0A,0x09,0x0A,0x88,0xC0},
	{0xC0,0x80,0x38,0x0A,0x39,0x22,0xB8,0xC0},
	{0xC0,0x80,0x38,0x0A,0x39,0x0A,0xB8,0xC0},
	{0xC0,0x80,0x28,0x2A,0x39,0x0A,0x88,0xC0},
	{0xC0,0x80,0x38,0x22,0x39,0x0A,0xB8,0xC0},
	{0xC0,0x80,0x38,0x22,0x39,0x2A,0xB8,0xC0},
	{0xC0,0x80,0x38,0x0A,0x09,0x0A,0x88,0xC0},
	{0xC0,0x80,0x38,0x2A,0x39,0x2A,0xB8,0xC0},
	{0xC0,0x80,0x38,0x2A,0x39,0x0A,0xB8,0xC0},
};

uchar code IMG_PLAYTITLE_LEVEL[9][8] = {
	{0x00,0x00,0x82,0x82,0x82,0x82,0xE2,0x00},
	{0x00,0x00,0x87,0x81,0x87,0x84,0xE7,0x00},
	{0x00,0x00,0x87,0x81,0x87,0x81,0xE7,0x00},
	{0x00,0x00,0x85,0x85,0x87,0x81,0xE1,0x00},
	{0x00,0x00,0x87,0x84,0x87,0x81,0xE7,0x00},
	{0x00,0x00,0x87,0x84,0x87,0x85,0xE7,0x00},
	{0x00,0x00,0x87,0x81,0x81,0x81,0xE1,0x00},
	{0x00,0x00,0x87,0x85,0x87,0x85,0xE7,0x00},
	{0x00,0x00,0x87,0x85,0x87,0x81,0xE7,0x00},
};

void delaytest(uchar dd){
	uchar aa,bb,cc;
	for (aa = 0; aa < 200; ++aa)
	{
		for (bb = 0; bb < 200; ++bb){
			for (cc = 0; cc < dd; ++cc){
				;
			}
		}
	}
}

void mg_restart(){

	gameStatus = GAME_STS_SETTING;

	level = 0;
	speed = 0;
}

void mg_gameover(){
	gameStatus = GAME_STS_GAMEOVER;
}

uint mg_moveUp(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (puzzleArr[nowAwnserIdx] == PUZZLE_U) {
			playStatus = PLAY_STS_ANSWER_OK;
			nowAwnserIdx++;
			return TIME_SPEEDS[speed];
		} else {
			mg_gameover();
			return TIME_GAMEOVER;
		}
	} else if (gameStatus == GAME_STS_SETTING) {
		if (level+1 == MAX_LEVEL)
		{
			level = 0;
		} else {
			level++;
		}
	} else {
		//mg_restart();
	}
}

uint mg_moveDown(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (puzzleArr[nowAwnserIdx] == PUZZLE_D) {
			playStatus = PLAY_STS_ANSWER_OK;
			nowAwnserIdx++;
			return TIME_SPEEDS[speed];
		} else {
			mg_gameover();
			return TIME_GAMEOVER;
		}
	} else if (gameStatus == GAME_STS_SETTING) {
		if (level == 0)
		{
			level = MAX_LEVEL-1;
		} else {
			level--;
		}
	} else {
		//mg_restart();
	}
}

uint mg_moveLeft(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (puzzleArr[nowAwnserIdx] == PUZZLE_L) {
			playStatus = PLAY_STS_ANSWER_OK;
			nowAwnserIdx++;
			return TIME_SPEEDS[speed];
		} else {
			mg_gameover();
			return TIME_GAMEOVER;
		}
	} else if (gameStatus == GAME_STS_SETTING) {
		// DOSOMETHING?
	} else {
		//mg_restart();
	}
}

uint mg_moveRight(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (puzzleArr[nowAwnserIdx] == PUZZLE_R) {
			playStatus = PLAY_STS_ANSWER_OK;
			nowAwnserIdx++;
			return TIME_SPEEDS[speed];
		} else {
			mg_gameover();
			return TIME_GAMEOVER;
		}
	} else if (gameStatus == GAME_STS_SETTING) {
		// 生成一组谜题
		mg_createPuzzle();

		// 显示Title状态
		playStatus = PLAY_STS_BEFORE_SHOWLEVEL;

		// 设置模式下按右键，结束level设置，开始游戏
		gameStatus = GAME_STS_PLAY;
	} else {
		//mg_restart();
	}
}

// TODO
// 这个函数处理完下一步的数据(更新状态等)后返回一个时间，
// 这个时间用来告诉主控，当前显示帧（或动作）期待的持续时间
// 主控接收到这个时间后应该显示当前帧并持续指定时间,然后再一次调用本函数继续下一步,循环往复形成连续的动画
uint mg_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (playStatus == PLAY_STS_BEFORE_SHOWLEVEL) {
			playStatus = PLAY_STS_SHOWLEVEL;
			return TIME_SHOWTITLE;
		} else if (playStatus == PLAY_STS_SHOWLEVEL) {
			// 由当前等级画面转移到问题画面
			playStatus = PLAY_STS_QUEST;
			nowShowingPuzzleIdx = 0;
			return TIME_SPEEDS[speed];
		} else if (playStatus == PLAY_STS_QUEST) {
			if (nowShowingPuzzleIdx==level) // 说明已经显示完这个系列的谜题了
			{
				playStatus = PLAY_STS_WAITINPUT;
				nowAwnserIdx = 0; // 回答idx从0开始
				// 通知主程序停止定时器，等待用户输入
				return TIME_STOP;
			} else {
				// 否则按照当前的速度继续闪现谜题序列
				nowShowingPuzzleIdx++;
				return TIME_SPEEDS[speed];
			}
		} 
		else if (playStatus == PLAY_STS_WAITINPUT || playStatus == PLAY_STS_ANSWER_OK) {
			if (nowAwnserIdx > level) // 说明已经解答完这组谜题了（当前速度下）
			{
				// 重新生成一组谜题
				mg_createPuzzle();

				nowAwnserIdx = 0;

				// 准备好显示Title状态
				playStatus = PLAY_STS_BEFORE_SHOWLEVEL;

				if (speed == MAX_SPEED - 1) // 说明已经解答完当前等级下最高速度的谜题了
				{
					// 进入下一等级，从最低速度开始
					level++;
					speed=0;
				} else {
					// 当前等级，提高速度
					speed++;
				}

				return TIME_SHOWTITLE;
			} 
			else {
				playStatus = PLAY_STS_WAITINPUT;
				return TIME_STOP;
			} 
		}
	} else if (gameStatus == GAME_STS_GAMEOVER) {
		// 如果当前是GameOver的画面，则转到restart
		mg_restart();
	} else {
		// 没有特殊要求的话，为了保证主程序的定时器正常执行，这里随便返回一个较短的时间即可
		return 20;
	}
}

void mg_createPuzzle()
{
	uchar i;
	puzzleArr[0] = rand() % 4;

	for (i = 1; i < MAX_LEVEL; ++i)
	{
		do
		{
			puzzleArr[i] = rand() % 4; // 0,1,2,3的随机数
		} while(puzzleArr[i] == puzzleArr[i-1]); //不允许相邻重复（避免闪现时画面没有变化看漏了）
	}
}

// 该函数用于给主程序提供显示内容（行单位）
uchar mg_getRowDataForShow(uchar row){

	if (gameStatus == GAME_STS_PLAY)
	{
		if (playStatus == PLAY_STS_SHOWLEVEL || playStatus == PLAY_STS_BEFORE_SHOWLEVEL)
		{
			return IMG_PLAYTITLE_LEVEL[level][row];

		} else if (playStatus == PLAY_STS_QUEST) {
			return IMG_ARROWS[puzzleArr[nowShowingPuzzleIdx]][row];

		} else if (playStatus == PLAY_STS_WAITINPUT) {
			return IMG_WAITINPUT[row];

		} else if (playStatus == PLAY_STS_ANSWER_OK) {
			return IMG_RIGHT[row];

		} else if (playStatus == PLAY_STS_ANSWER_NG) {
			return IMG_WRONG[row];

		} else {
			// TODO
		}
		
	}
	else if (gameStatus == GAME_STS_SETTING)
	{
		return IMG_SETTING_LEVEL[level][row];
	}
	else if (gameStatus == GAME_STS_GAMEOVER) {
		return IMG_GAMEOVER[row];
	}
	else {
		return 0x55;
	}
}

uint mg_KeepPressingU(){
	mg_restart();
	return 20;
}

uint mg_KeepPressingD(){
	return 20;
}

uint mg_KeepPressingL(){
	return 20;
}

uint mg_KeepPressingR(){
	return 20;
}