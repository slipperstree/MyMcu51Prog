#include <snake.h>
#include <stdlib.h>

// ��Ϸ״̬
uchar gameStatus;

// ��ͼ��ά����
uchar data map[SIZE_Y][SIZE_Y];

// ��ͷ��λ��
uchar x_head, y_head;

// ��β��λ��
uchar x_end, y_end;

// ��ǰ����
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

			// ǰ���ǿյز��ƶ�β�ͣ������ʳ�����ƶ�β�ͣ�Ҳ����������䳤
			if (map[x_head][y_head-1] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}

			// ԭ����ͷ������壬��ǰһ�����Ϸ���Ҳ�����µ�ͷ����
			map[x_head][y_head] = STS_BODY_FROM_U;

			// �ƶ�ͷ
			y_head--;
			map[x_head][y_head] = STS_HEAD;

			// �޸ĵ�ǰ����
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
			// ǰ���ǿյز��ƶ�β�ͣ������ʳ�����ƶ�β�ͣ�Ҳ����������䳤
			if (map[x_head][y_head+1] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}
			
			// ԭ����ͷ������壬��ǰһ�����·���Ҳ�����µ�ͷ����
			map[x_head][y_head] = STS_BODY_FROM_D;

			// �ƶ�ͷ
			y_head++;
			map[x_head][y_head] = STS_HEAD;

			// �޸ĵ�ǰ����
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
			// ǰ���ǿյز��ƶ�β�ͣ������ʳ�����ƶ�β�ͣ�Ҳ����������䳤
			if (map[x_head-1][y_head] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}

			// ԭ����ͷ������壬��ǰһ�����󷽣�Ҳ�����µ�ͷ����
			map[x_head][y_head] = STS_BODY_FROM_L;

			// �ƶ�ͷ
			x_head--;
			map[x_head][y_head] = STS_HEAD;

			// �޸ĵ�ǰ����
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
			// ǰ���ǿյز��ƶ�β�ͣ������ʳ�����ƶ�β�ͣ�Ҳ����������䳤
			if (map[x_head+1][y_head] != STS_APPLE) {
				snake_moveEndBlock();
			} else {
				snake_createfood();
			}

			// ԭ����ͷ������壬��ǰһ�����ҷ���Ҳ�����µ�ͷ����
			map[x_head][y_head] = STS_BODY_FROM_R;

			// �ƶ�ͷ
			x_head++;
			map[x_head][y_head] = STS_HEAD;

			// �޸ĵ�ǰ����
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

// ����ʳ��
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

		// ����0(ʳ��/ͷ��/�ϰ���)�Ķ���Ҫ��ʾ�㣬�����Խ�һ��ϸ�֣���ʱ������
		if (map[x][row] > 0)
		{
			rowDataForShow |= 1;
		}
	}

	return rowDataForShow;
	//return 0x02;
}