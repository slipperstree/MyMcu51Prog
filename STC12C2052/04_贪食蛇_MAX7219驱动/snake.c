#include <snake.h>
#include <stdlib.h>

// 游戏状态
uchar gameStatus;

// 地图二维数组
uchar data map[SIZE_Y][SIZE_Y];

// 蛇头的位置
uchar x_head, y_head;

// 蛇尾的位置
uchar x_end, y_end;

// 当前朝向
uchar nowTowards;

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

void snake_restart(){
	uchar x, y;

	for (x = 0; x < SIZE_X; ++x)
	{
		for (y = 0; y < SIZE_Y; ++y)
		{
			map[x][y] = STS_EMPTY;
		}
	}

	map[0][0] = STS_BODY_FROM_R;
	map[1][0] = STS_BODY_FROM_R;
	map[2][0] = STS_HEAD;

	snake_createfood();

	x_head = 2, y_head = 0;
	x_end = 0, y_end = 0;

	nowTowards = NOW_TOWARDS_R;
	gameStatus = GAME_STS_PLAY;
}

void snake_gameover(){
	gameStatus = GAME_STS_GAMEOVER;
}

void snake_moveUp(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (y_head>0 && (map[x_head][y_head-1] == STS_EMPTY || map[x_head][y_head-1] == STS_APPLE)) {

			// 前方是空地才移动尾巴，如果是食物则不移动尾巴，也就是让身体变长
			if (map[x_head][y_head-1] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}

			// 原来的头变成身体，且前一块在上方（也就是新的头部）
			map[x_head][y_head] = STS_BODY_FROM_U;

			// 移动头
			y_head--;
			map[x_head][y_head] = STS_HEAD;

			// 修改当前朝向
			nowTowards = NOW_TOWARDS_U;
		} else {
			snake_gameover();
		}
	} else {
		snake_restart();
	}
}

void snake_moveDown(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (y_head<SIZE_Y-1 && (map[x_head][y_head+1] == STS_EMPTY || map[x_head][y_head+1] == STS_APPLE))
		{
			// 前方是空地才移动尾巴，如果是食物则不移动尾巴，也就是让身体变长
			if (map[x_head][y_head+1] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}
			
			// 原来的头变成身体，且前一块在下方（也就是新的头部）
			map[x_head][y_head] = STS_BODY_FROM_D;

			// 移动头
			y_head++;
			map[x_head][y_head] = STS_HEAD;

			// 修改当前朝向
			nowTowards = NOW_TOWARDS_D;
		} else {
			snake_gameover();
		}
	} else {
		snake_restart();
	}
}

void snake_moveLeft(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (x_head>0 && (map[x_head-1][y_head] == STS_EMPTY || map[x_head-1][y_head] == STS_APPLE))
		{
			// 前方是空地才移动尾巴，如果是食物则不移动尾巴，也就是让身体变长
			if (map[x_head-1][y_head] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}

			// 原来的头变成身体，且前一块在左方（也就是新的头部）
			map[x_head][y_head] = STS_BODY_FROM_L;

			// 移动头
			x_head--;
			map[x_head][y_head] = STS_HEAD;

			// 修改当前朝向
			nowTowards = NOW_TOWARDS_L;
		} else {
			snake_gameover();
		}
	} else {
		snake_restart();
	}
}

void snake_moveRight(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (x_head<SIZE_X-1 && (map[x_head+1][y_head] == STS_EMPTY || map[x_head+1][y_head] == STS_APPLE))
		{
			// 前方是空地才移动尾巴，如果是食物则不移动尾巴，也就是让身体变长
			if (map[x_head+1][y_head] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}

			// 原来的头变成身体，且前一块在右方（也就是新的头部）
			map[x_head][y_head] = STS_BODY_FROM_R;

			// 移动头
			x_head++;
			map[x_head][y_head] = STS_HEAD;

			// 修改当前朝向
			nowTowards = NOW_TOWARDS_R;
		} else {
			snake_gameover();
		}
	} else {
		snake_restart();
	}
}

void snake_moveEndBlock(){
	uchar xTmp = x_end;
	uchar yTmp = y_end;

	switch (map[x_end][y_end]) {
		case STS_BODY_FROM_U:
			y_end--;
			break;
		case STS_BODY_FROM_D:
			y_end++;
			break;
		case STS_BODY_FROM_L:
			x_end--;
			break;
		case STS_BODY_FROM_R:
			x_end++;
			break;
	}

	map[xTmp][yTmp] = STS_EMPTY;
}

void snake_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		switch(nowTowards){
			case NOW_TOWARDS_U:
				snake_moveUp();
				break;
			case NOW_TOWARDS_D:
				snake_moveDown();
				break;
			case NOW_TOWARDS_L:
				snake_moveLeft();
				break;
			case NOW_TOWARDS_R:
				snake_moveRight();
				break;
		}
	}
}

// 生成食物
void snake_createfood()
{
	uchar x;
	uchar y;
	//int seed=100;
	//srand(seed);
	do
	{
		x = rand() % SIZE_X;
		y = rand() % SIZE_X;
	} while(map[x][y] != STS_EMPTY);

	map[x][y] = STS_APPLE;
}


uchar snake_getRowDataForShow(uchar row){
	uchar x;
	uchar rowDataForShow = 0;
	for (x = 0; x < SIZE_X; ++x)
	{
		rowDataForShow = rowDataForShow << 1;

		// 大于0(食物/头部/障碍物)的都需要显示点，还可以进一步细分，暂时不考虑
		if (map[x][row] > 0)
		{
			rowDataForShow |= 1;
		}
	}

	return rowDataForShow;
	//return 0x02;
}