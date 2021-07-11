#include "snake.h"

// 游戏状态
uchar gameStatus;

// 地图二维数组
uchar map[SNAKE_SIZE_X][SNAKE_SIZE_Y];

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	// 寻路二维数组
	union dfsMapType mapAI[SNAKE_SIZE_X][SNAKE_SIZE_Y];
	char x_headAI, y_headAI, stepsFromHeadAI;
	void wander(uchar); // 散步
#endif

#ifdef AI_LEVEL_MASTER
	// 避免苹果生成在某个地方导致死循环，如果蛇走了多少步以后还没有吃到苹果则重新生成一次
	#define RECREATE_APPLE_STEPS (SNAKE_SIZE_X*SNAKE_SIZE_Y*2)
	unsigned int reCreateAppleStep;

	// 如果重新生成好多次苹果还是没吃到过那么认为已经吃不到了，放弃这一局自杀
	#define RE_CREATE_APPLE_CNT 10
	uchar reCreateAppleCnt;
#endif

// 蛇头的位置
uchar x_head, y_head;

// 蛇尾的位置
uchar x_end, y_end;

// 食物的位置
uchar x_apple, y_apple;

// 当前朝向
uchar nowTowards;

// 分数
unsigned int score;

void gameover(void);
void createfood(void);
void moveEndBlock(void);
uchar canMoveComm(uchar direction, uchar x, uchar y, uchar stepsFromHead);
uchar canMoveUp(uchar x, uchar y, uchar stepsFromHead);
uchar canMoveDown(uchar x, uchar y, uchar stepsFromHead);
uchar canMoveLeft(uchar x, uchar y, uchar stepsFromHead);
uchar canMoveRight(uchar x, uchar y, uchar stepsFromHead);
uchar canGetTail(uchar x, uchar y);

#ifdef AI_LEVEL_KIDS
	// 各方向上的自由度
	uchar freeUp,freeDown,freeLeft,freeRight = 0;

	// 取绝对值
	uchar myAbs(uchar a, uchar b){
		if(a>=b)
			return a-b;
		else
			return b-a;
	}
	
	// 取某个位置的自由度（周围可走的方向数）
	uchar ai_kids_getFree(uchar x, uchar y){
		uchar stepsFromHead = myAbs(x_head, x) + myAbs(y_head, y);
		return canMoveUp(x,y,stepsFromHead) + canMoveDown(x,y,stepsFromHead) + canMoveLeft(x,y,stepsFromHead) + canMoveRight(x,y,stepsFromHead);
	}
#endif

// 调用者从init函数传进来的回调函数指针
pSnakeEventFunc callbackSnakeEventFunc;

// 所有需要更新map数据的时候都调用这个函数，保证能及时通知控制端更新响应的画面
void updateMap(uchar x, uchar y, uchar sts){
	map[x][y] = sts;
	callbackSnakeEventFunc(SNAKE_EVENT_UPDATE_BLOCK, x, y);
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
void clearMapAI(){
	uchar x,y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			mapAI[x][y].data = 0;
		}
	}

	stepsFromHeadAI=0;
}
#endif

void snake_restart(){
	uchar x, y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			map[x][y] = STS_EMPTY;
		}
	}

	#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	clearMapAI();
	#endif

	// TODO 两个事件参数没有使用
	callbackSnakeEventFunc(SNAKE_EVENT_RESTART, 0, 0);

	// 初始分数
	score = 0;

	gameStatus = GAME_STS_PLAY;

	// 初始block 尾巴-身体-头 ->
	x_head = 2, y_head = 0;
	x_end = 0, y_end = 0;

	updateMap(x_end, y_end, STS_TAIL + STS_NEXT_TO_R);
	updateMap(1, 0, STS_BODY_PREV_R + STS_NEXT_TO_L);
	updateMap(x_head, y_head, STS_HEAD + STS_NEXT_TO_L);
	nowTowards = DIRECTION_RIGHT;

	createfood();
}

void snake_init(pSnakeEventFunc pCallbackSnakeEventFunc){
	callbackSnakeEventFunc = pCallbackSnakeEventFunc;
}

void gameover(){
	gameStatus = GAME_STS_GAMEOVER;

	callbackSnakeEventFunc(SNAKE_EVENT_GAMEOVER, score, 0);
}

// 当前状态下N步以后指定坐标是否为蛇尾
// 这是为了判断当蛇头走过N步数以后来到这个点的时候某个方向上本来是蛇身，
//   走了N步以后有可能变成蛇尾，那么这个方向的自由度也有效
//   如果不考虑这一点那么自由度就是2（左，上），其实应该是3（左，上，右）
//   当蛇头走一步到A点时，蛇尾也移动了一步，于是A点的右侧变成了蛇尾，A点的蛇头是允许转向右侧的（不会撞到蛇尾）
//   比如下图，-为蛇尾，@为蛇头，+为蛇身，现在要判断A点的自由度，
//    -
//   A+	
//   @+	
//   ++    
//   蛇头向上走到A点后变成下面的样子，右侧变成蛇尾，可走的通
//     
//   @-
//   ++
//   ++
//   再向右走，没有问题
//     
//   +@
//   +-
//   ++
uchar isTail(uchar x, uchar y, uchar afterSteps){
	uchar step;
	uchar tmp_tail_x = x_end;
	uchar tmp_tail_y = y_end;
	uchar tailNextTo;

    //先直接判断当前这个点是不是就是蛇尾，如果是就不需要走N步再判断了
	if (tmp_tail_x == x && tmp_tail_y == y)
	{
		return 1;
	}

	// 让蛇尾沿着蛇身走相差的步数
	for (step = 0; step < afterSteps; step++)
	{
		if(step == 0){
			// 第一次是蛇尾，方向从个位数取
			tailNextTo = map[tmp_tail_x][tmp_tail_y] % 10;
		} else {
			// 蛇身，方向从十位数取（个位数是后面连接的方向）
			tailNextTo = map[tmp_tail_x][tmp_tail_y] / 10;
		}
		
		switch (tailNextTo)
		{
		case STS_NEXT_TO_U:
			tmp_tail_y--;
			break;
		case STS_NEXT_TO_D:
			tmp_tail_y++;
			break;
		case STS_NEXT_TO_L:
			tmp_tail_x--;
			break;
		case STS_NEXT_TO_R:
			tmp_tail_x++;
			break;
		default:
			break;
		}

		// 只要在途中蛇尾经过这个点就表示这个点是安全的
		if (tmp_tail_x == x && tmp_tail_y == y)
		{
			return 1;
		}
	}

	return 0;

	// if (tmp_tail_x == x && tmp_tail_y == y)
	// {
	// 	return 1;
	// } else {
	// 	return 0;
	// }
}

// x,y : 评估对象位置
// stepsFromHead : 从蛇头走到x,y已经走过了多少步（为了预估蛇尾的位置）
uchar canMoveComm(uchar direction, uchar x, uchar y, uchar stepsFromHead){
	switch (direction)
	{
	case DIRECTION_UP:
		return canMoveUp(x, y, stepsFromHead);
	case DIRECTION_DOWN:
		return canMoveDown(x, y, stepsFromHead);
	case DIRECTION_LEFT:
		return canMoveLeft(x, y, stepsFromHead);
	case DIRECTION_RIGHT:
		return canMoveRight(x, y, stepsFromHead);
	default:
		return 0;
	}
}

uchar canMoveRight(uchar x, uchar y, uchar stepsFromHead){

	if (x<SNAKE_SIZE_X-1 && map[x+1][y] == STS_APPLE) {
		return 5;
	} else if (x<SNAKE_SIZE_X-1 && (map[x+1][y] == STS_EMPTY ||isTail(x+1,y,stepsFromHead) )) {
		return 1;
	} else {
		return 0;
	}
}

uchar canMoveLeft(uchar x, uchar y, uchar stepsFromHead){

	if (x>0 && map[x-1][y] == STS_APPLE) {
		return 5;
	} else if (x>0 && (map[x-1][y] == STS_EMPTY || isTail(x-1,y,stepsFromHead) )) {
		return 1;
	} else {
		return 0;
	}
}

uchar canMoveDown(uchar x, uchar y, uchar stepsFromHead){

	if (y<SNAKE_SIZE_Y-1 && map[x][y+1] == STS_APPLE) {
		return 5;
	} else if (y<SNAKE_SIZE_Y-1 && ( map[x][y+1] == STS_EMPTY || isTail(x,y+1,stepsFromHead)) ) {
		return 1;
	} else {
		return 0;
	}
}

uchar canMoveUp(uchar x, uchar y, uchar stepsFromHead){

	if (y>0 && map[x][y-1] == STS_APPLE) {
		return 5;
	} else if (y>0 && (map[x][y-1] == STS_EMPTY || isTail(x,y-1,stepsFromHead))) {
		return 1;
	} else {
		return 0;
	}
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	void updateMapAI(uchar x, uchar y, uchar status){
		mapAI[x][y].subDatas.status = status;

		#ifdef SNAKE_AI_DEBUG
		callbackSnakeEventFunc(SNAKE_EVENT_UPDATE_AI_PATH, x, y);
		#endif
	}

	uchar canMoveCommAI(uchar direction){
		switch (direction)
		{
		case DIRECTION_UP:
			if (canMoveUp(x_headAI, y_headAI, stepsFromHeadAI)
					&& mapAI[x_headAI][y_headAI-1].subDatas.status == AI_STATUS_EMPTY
					//&& canGetTail(x_headAI, y_headAI-1)
					)
			{
				return 1;
			} else {
				return 0;
			}
		case DIRECTION_DOWN:
			if (canMoveDown(x_headAI, y_headAI, stepsFromHeadAI) 
					&& mapAI[x_headAI][y_headAI+1].subDatas.status == AI_STATUS_EMPTY 
					//&& canGetTail(x_headAI, y_headAI+1)
					)
			{
				return 1;
			} else {
				return 0;
			}
		case DIRECTION_LEFT:
			if (canMoveLeft(x_headAI, y_headAI, stepsFromHeadAI) 
					&& mapAI[x_headAI-1][y_headAI].subDatas.status == AI_STATUS_EMPTY
					//&& canGetTail(x_headAI-1, y_headAI)
					)
			{
				return 1;
			} else {
				return 0;
			}
		case DIRECTION_RIGHT:
			if (canMoveRight(x_headAI, y_headAI, stepsFromHeadAI)
					&& mapAI[x_headAI+1][y_headAI].subDatas.status == AI_STATUS_EMPTY 
					//&& canGetTail(x_headAI+1, y_headAI)
					)
			{
				return 1;
			} else {
				return 0;
			}
		default:
			return 0;
		}
	}

	void markDeadwayAndGoBackToPrevAI(){
		updateMapAI(x_headAI, y_headAI, AI_STATUS_WAY_TO_DEATH);

		// 回到上个节点
		switch (mapAI[x_headAI][y_headAI].subDatas.prev_direction)
		{
		case DIRECTION_LEFT:
			x_headAI--;
			break;
		case DIRECTION_RIGHT:
			x_headAI++;
			break;
		case DIRECTION_UP:
			y_headAI--;
			break;
		case DIRECTION_DOWN:
			y_headAI++;
			break;
		default:
			break;
		}

		if (stepsFromHeadAI > 0)
		{
			stepsFromHeadAI--;
		}
	}

	uchar markApplewayAndGoBackToPrevAI(){
		updateMapAI(x_headAI, y_headAI, AI_STATUS_WAY_TO_APPLE);

		if (x_headAI == x_head && y_headAI == y_head)
		{
			// 当前是蛇头，则表示回溯结束
			return 0;
		} else {
			// 回到上个节点
			switch (mapAI[x_headAI][y_headAI].subDatas.prev_direction)
			{
			case DIRECTION_LEFT:
				x_headAI--;
				break;
			case DIRECTION_RIGHT:
				x_headAI++;
				break;
			case DIRECTION_UP:
				y_headAI--;
				break;
			case DIRECTION_DOWN:
				y_headAI++;
				break;
			default:
				break;
			}

			return 1;
		}
	}

	void doMoveUpAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_UP;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI][y_headAI-1].subDatas.prev_direction = DIRECTION_DOWN;
		y_headAI--;
		stepsFromHeadAI++;
	}

	void doMoveDownAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_DOWN;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI][y_headAI+1].subDatas.prev_direction = DIRECTION_UP;
		y_headAI++;
		stepsFromHeadAI++;
	}

	void doMoveLeftAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_LEFT;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI-1][y_headAI].subDatas.prev_direction = DIRECTION_RIGHT;
		x_headAI--;
		stepsFromHeadAI++;
	}

	void doMoveRightAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_RIGHT;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI+1][y_headAI].subDatas.prev_direction = DIRECTION_LEFT;
		x_headAI++;
		stepsFromHeadAI++;
	}
#endif

uchar snake_moveComm(uchar direction){
	// 试图移动方向的反方向，移动后的x，移动后的y
	uchar directionBackward, x_next, y_next;
	switch (direction)
	{
	case DIRECTION_UP:
		directionBackward = DIRECTION_DOWN;
		x_next = x_head;
		y_next = y_head-1;
		break;
	case DIRECTION_DOWN:
		directionBackward = DIRECTION_UP;
		x_next = x_head;
		y_next = y_head+1;
		break;
	case DIRECTION_LEFT:
		directionBackward = DIRECTION_RIGHT;
		x_next = x_head-1;
		y_next = y_head;
		break;
	case DIRECTION_RIGHT:
		directionBackward = DIRECTION_LEFT;
		x_next = x_head+1;
		y_next = y_head;
		break;
	default:
		break;
	}

	if (gameStatus == GAME_STS_PLAY)
	{
		// 后退无效
		if (nowTowards == directionBackward){
			return 0;
		}

		if (canMoveComm(direction, x_head, y_head, 0)) 
		{
			// 前方是空地或者是尾巴才移动尾巴，如果是食物则不移动尾巴，也就是让身体变长
			if (map[x_next][y_next] != STS_APPLE) {
				moveEndBlock();

				#ifdef AI_LEVEL_MASTER
					// 每走一步就步数加一
					reCreateAppleStep++;
					if (reCreateAppleStep > RECREATE_APPLE_STEPS)
					{
						reCreateAppleStep = 0;

						// 蛇已经走了很多步了，但仍然不能吃到苹果就重新生成一个新的苹果
						snake_reCreateFood();
						// 每重新生成一次苹果就计数一次
						reCreateAppleCnt++;
						if (reCreateAppleCnt > RE_CREATE_APPLE_CNT)
						{
							// 重新生成苹果多次，AI仍然吃不到苹果就认为无法继续游戏，强制gameover
							reCreateAppleCnt = 0;
							gameover();
						}
					}
					
				#endif
			} else {

				#ifdef AI_LEVEL_MASTER
					// 每吃到一次苹果就将重复生成苹果的次数和走的步数清零重新计数
					reCreateAppleCnt=0;
					reCreateAppleStep=0;
				#endif

				score++;
				createfood();
				callbackSnakeEventFunc(SNAKE_EVENT_EAT_APPLE, score, 0);
			}

			// 原来的头变成身体，且前一块指向新的头部（十位数控制，也就是方向乘以10）
			updateMap(x_head, y_head, (direction * 10) + map[x_head][y_head] % 10);

			// 设置新的头，并更新头位置
			updateMap(x_next, y_next, STS_HEAD + directionBackward);
			x_head = x_next;
			y_head = y_next;

			// 修改当前朝向
			nowTowards = direction;
		} else {
			gameover();
		}
	}

	return 1;
}

uchar snake_moveUp(){
	return snake_moveComm(DIRECTION_UP);
}

uchar snake_moveDown(){
	return snake_moveComm(DIRECTION_DOWN);
}

uchar snake_moveLeft(){
	return snake_moveComm(DIRECTION_LEFT);
}

uchar snake_moveRight(){
	return snake_moveComm(DIRECTION_RIGHT);
}

void moveEndBlock(){
	uchar xTmp = x_end;
	uchar yTmp = y_end;

	// 取个位数，得到蛇尾前一块block的方向
	switch (map[x_end][y_end] % 10) {
		case STS_NEXT_TO_U:
			y_end--;
			break;
		case STS_NEXT_TO_D:
			y_end++;
			break;
		case STS_NEXT_TO_L:
			x_end--;
			break;
		case STS_NEXT_TO_R:
			x_end++;
			break;
	}

	// 原来的蛇尾清空
	updateMap(xTmp, yTmp, STS_EMPTY);

	// 原来的蛇身变成蛇尾(相邻的block方向信息改成蛇身的十位数(前一个block方向))
	updateMap(x_end, y_end, STS_TAIL + map[x_end][y_end] / 10);

}

void snake_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		snake_moveComm(nowTowards);
	}
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
// 看不到苹果的时候，随便走一步
void wander(uchar exceptRirection){
	uchar canMoveUpValue, canMoveLeftValue, canMoveRightValue, canMoveDownValue;
	uchar directionRnd = rand() % 4 + 1;

	canMoveUpValue = canMoveUp(x_head, y_head, 0);
	canMoveDownValue = canMoveDown(x_head, y_head, 0);
	canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
	canMoveRightValue = canMoveRight(x_head, y_head, 0);

	// 如果无路可走则gameover
	if (!(canMoveUpValue + canMoveDownValue + canMoveLeftValue + canMoveRightValue))
	{
		gameover();
	}
	// 如果只剩一个方向则必须走，不然会死循环
	else if (canMoveUpValue && !(canMoveDownValue + canMoveLeftValue + canMoveRightValue))
	{
		snake_moveUp();
	}
	else if (canMoveDownValue && !(canMoveUpValue + canMoveLeftValue + canMoveRightValue))
	{
		snake_moveDown();
	}
	else if (canMoveLeftValue && !(canMoveUpValue + canMoveDownValue + canMoveRightValue))
	{
		snake_moveLeft();
	}
	else if (canMoveRightValue && !(canMoveUpValue + canMoveDownValue + canMoveLeftValue))
	{
		snake_moveRight();
	}
	// 有两个以上方向可走，随机走一步（这里可以再优化，太随机会搞死自己，需要有策略的随机）
	else{
		while(directionRnd == exceptRirection || canMoveComm(directionRnd, x_head, y_head, 0) == 0){
		directionRnd = rand() % 4 + 1;
		}
		snake_moveComm(directionRnd);
	}
}
#endif

#ifdef AI_LEVEL_MASTER
uchar canGetTail(uchar x_from, uchar y_from){
	clearMapAI();

	x_headAI = x_from;
	y_headAI = y_from;
	while(1){
		// 如果 x当前点 == x蛇尾
		if (x_headAI == x_end && y_headAI == y_end)
		{
			// 可以找到蛇尾，结束寻找
			return 1;
		} else {
			if (canMoveCommAI(DIRECTION_LEFT)){
				doMoveLeftAI();
			} else if (canMoveCommAI(DIRECTION_UP)) {
				doMoveUpAI();
			} else if (canMoveCommAI(DIRECTION_RIGHT)) {
				doMoveRightAI();
			} else if (canMoveCommAI(DIRECTION_DOWN)) {
				doMoveDownAI();
			} else {
				if (x_headAI == x_from && y_headAI == y_from)
				{
					// 当前是起始点，则表示探路找了一圈没找到退回原地了，没有通往蛇尾的路
					return 0;
				}else{
					// 当前点四个方向都不通，标记为死点并回到上一个节点
					markDeadwayAndGoBackToPrevAI();
				}
			}
		}
	}
}

uchar canGetApple(uchar x_from, uchar y_from){
	clearMapAI();

	x_headAI = x_from;
	y_headAI = y_from;
	while(1){
		// 如果 x当前点 == x苹果
		if (x_headAI == x_apple && y_headAI == y_apple)
		{
			// 可以找到苹果，结束寻找
			return 1;
		} else {
			if (canMoveCommAI(DIRECTION_LEFT)){
				doMoveLeftAI();
			} else if (canMoveCommAI(DIRECTION_UP)) {
				doMoveUpAI();
			} else if (canMoveCommAI(DIRECTION_RIGHT)) {
				doMoveRightAI();
			} else if (canMoveCommAI(DIRECTION_DOWN)) {
				doMoveDownAI();
			} else {
				if (x_headAI == x_from && y_headAI == y_from)
				{
					// 当前是起始点，则表示探路找了一圈没找到退回原地了，没有通往蛇尾的路
					return 0;
				}else{
					// 当前点四个方向都不通，标记为死点并回到上一个节点
					markDeadwayAndGoBackToPrevAI();
				}
			}
		}
	}
}
#endif

// 随机向周围的空地方向走一步
void snake_AI_moveNext(){
	uchar towards;
	uchar isSafe;
	// 各方向可否移动(0:不可，1:可)
	uchar canMoveUpValue, canMoveDownValue, canMoveLeftValue, canMoveRightValue;

	#ifdef AI_LEVEL_MASTER
	// 各方向移动一步后能否找到蛇头(0:不可，1:可)
	uchar canGetTailUpValue, canGetTailDownValue, canGetTailLeftValue, canGetTailRightValue;
	// 各方向移动一步后能否找到苹果(0:不可，1:可)
	uchar canGetAppleUpValue, canGetAppleDownValue, canGetAppleLeftValue, canGetAppleRightValue;
	#endif

	if (gameStatus == GAME_STS_PLAY)
	{
		//先检查目前3个方向中还有没有空地，如果都没有就gameover了,否则会有死循环
		canMoveUpValue = canMoveUp(x_head, y_head, 0);
		canMoveDownValue = canMoveDown(x_head, y_head, 0);
		canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
		canMoveRightValue = canMoveRight(x_head, y_head, 0);

		if( !canMoveUpValue && !canMoveDownValue && !canMoveLeftValue && !canMoveRightValue ) {
			gameover();
		}
		//周围有空地或苹果或尾巴
		else
		{
			#if defined(AI_LEVEL_BABY) || defined(AI_LEVEL_KIDS) || defined(AI_LEVEL_SMART)
				//如果旁边有苹果优先吃苹果(也可能因此挂掉)
				if(x_head+1 < SNAKE_SIZE_X && map[x_head+1][y_head] == STS_APPLE){
					snake_moveRight();
					return;
				}
				else if(x_head !=0 && map[x_head-1][y_head] == STS_APPLE){
					snake_moveLeft();
					return;
				}
				else if(y_head+1 < SNAKE_SIZE_Y && map[x_head][y_head+1] == STS_APPLE){
					snake_moveDown();
					return;
				}
				else if(y_head != 0 && map[x_head][y_head-1] == STS_APPLE){
					snake_moveUp();
					return;
				}
			#endif

			// 娃娃(只会随机乱走)
			#ifdef AI_LEVEL_BABY
				do{
					if(rand() % 2 == 0 && canMoveRightValue) {
						snake_moveRight();
						return;
					} else if(rand() % 2 == 0 && canMoveLeftValue) {
						snake_moveLeft();
						return;
					} else if(rand() % 2 == 0 && canMoveDownValue) {
						snake_moveDown();
						return;
					} else if(rand() % 2 == 0 && canMoveUpValue) {
						snake_moveUp();
						return;
					}
				} while(1);
			#endif

			// 儿童(会多预判2步)
			#ifdef AI_LEVEL_KIDS
				// 分别取下一步可走位置的自由度（走过去以后下下一步有几个方向可走）
				if (canMoveUpValue)
				{
					freeUp = ai_kids_getFree(x_head, y_head-1);
				} else {
					freeUp = 0;
				}
				if (canMoveDownValue)
				{
					freeDown = ai_kids_getFree(x_head, y_head+1);
				} else {
					freeDown = 0;
				}
				if (canMoveLeftValue)
				{
					freeLeft = ai_kids_getFree(x_head-1, y_head);
				} else {
					freeLeft = 0;
				}
				if (canMoveRightValue)
				{
					freeRight = ai_kids_getFree(x_head+1, y_head);
				} else {
					freeRight = 0;
				}
				
				// 朝自由度最大的方向行走
				do{
					if (canMoveUpValue && rand() % 4 == 0 && freeUp >= freeDown && freeUp >= freeLeft && freeUp >= freeRight) {
						snake_moveUp();
						return;
					} else if (canMoveDownValue && rand() % 4 == 0 && freeDown >= freeUp && freeDown >= freeLeft && freeDown >= freeRight) {
						snake_moveDown();
						return;
					} else if (canMoveLeftValue && rand() % 4 == 0 && freeLeft >= freeUp && freeLeft >= freeDown && freeLeft >= freeRight) {
						snake_moveLeft();
						return;
					} else if (canMoveRightValue && rand() % 4 == 0 && freeRight >= freeUp && freeRight >= freeDown && freeRight >= freeLeft) {
						snake_moveRight();
						return;
					}
				}while(1);
			#endif

			// 聪明(会寻路苹果)
			#ifdef AI_LEVEL_SMART
				x_headAI = x_head;
				y_headAI = y_head;

				#ifdef SNAKE_AI_DEBUG
				if (score >= 36)
				{
					score = score;
				}
				#endif

				clearMapAI();
				while(1){
					// 2-根据优先级决定探索方向
					// 如果 x当前点 == x苹果
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// 找到苹果了，利用【上一个节点的方向】信息依次回溯（同时沿路标记apple）到最开始蛇头的位置
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// 寻路结束,走一步,退出大循环(TODO:也可以一次性走完全程)
						snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// 当前点在苹果的右下方，搜索优先级:左上右下
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// 当前点在苹果的右上方，搜索优先级:左下右上
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// 当前点在苹果的左上方，搜索优先级:右下左上
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// 当前点在苹果的左下方，搜索优先级:右上左下
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// 当前点在苹果的正下方，搜索优先级:上左右下
						if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// 当前点在苹果的正上方，搜索优先级:下左右上
						if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正左方，搜索优先级:右下上左
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正右方，搜索优先级:左上下右
						if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					}
				}
			#endif

			// 大师(会优先避免死胡同其次寻路苹果)
			#ifdef AI_LEVEL_MASTER

				#ifdef SNAKE_AI_DEBUG
				if (score >= 36)
				{
					score = score;
				}
				#endif

				// 4个方向移动一步后能否找到蛇尾，苹果
				// TODO : 没必要全看，某方向上能找到蛇尾和苹果则不需要看其他方向了，节省计算量
				canGetTailUpValue = 0;		canGetAppleUpValue = 0;
				canGetTailDownValue = 0;	canGetAppleDownValue = 0;
				canGetTailLeftValue = 0;	canGetAppleLeftValue = 0;
				canGetTailRightValue = 0;	canGetAppleRightValue = 0;

				// if (canMoveUpValue) {
				// 	canGetTailUpValue = canGetTail(x_head, y_head-1);
				// 	canGetAppleUpValue = canGetApple(x_head, y_head-1);
				// }
				// if (canMoveDownValue) {
				// 	canGetTailDownValue = canGetTail(x_head, y_head+1);
				// 	canGetAppleDownValue = canGetApple(x_head, y_head+1);
				// }
				// if (canMoveLeftValue) {
				// 	canGetTailLeftValue = canGetTail(x_head-1, y_head);
				// 	canGetAppleLeftValue = canGetApple(x_head-1, y_head);
				// }
				// if (canMoveRightValue) {
				// 	canGetTailRightValue = canGetTail(x_head+1, y_head);
				// 	canGetAppleRightValue = canGetApple(x_head+1, y_head);
				// }

				// // 优先走 既能 找到蛇尾 也能 找到苹果 的方向
				// if (canGetTailUpValue & canGetAppleUpValue) {
				// 	canGetApple(x_head, y_head-1);
				// }
				// else if (canGetTailDownValue & canGetAppleDownValue){
				// 	canGetApple(x_head, y_head+1);
				// }
				// else if (canGetTailLeftValue & canGetAppleLeftValue){
				// 	canGetApple(x_head-1, y_head);
				// }
				// else if (canGetTailRightValue & canGetAppleRightValue){
				// 	canGetApple(x_head+1, y_head);
				// }

				// // 其次走 只能 找到蛇尾 的方向（TODO 应该优化成走能通向蛇尾并且是最长的路径这么一条路，尽可能空出空间）
				// else if (canGetTailUpValue) {
				// 	snake_moveUp();
				// }
				// else if (canGetTailDownValue){
				// 	snake_moveDown();
				// }
				// else if (canGetTailLeftValue){
				// 	snake_moveLeft();
				// }
				// else if (canGetTailRightValue){
				// 	snake_moveRight();
				// }

				// // 再次走 只能 找到苹果的方向
				// else if (canGetAppleUpValue) {
				// 	snake_moveUp();
				// }
				// else if (canGetAppleDownValue){
				// 	snake_moveDown();
				// }
				// else if (canGetAppleLeftValue){
				// 	snake_moveLeft();
				// }
				// else if (canGetAppleRightValue){
				// 	snake_moveRight();
				// }

				// // 最差的情况，蛇尾和苹果都看不到，只能随便散步了(TODO 散步的策略也有讲究)
				// else {
				// 	wander(0);
				// }

				// 这一段是有效的AI逻辑，如果上面调试失败可以恢复下面这一段
				x_headAI = x_head;
				y_headAI = y_head;
				clearMapAI();
				while(1){
					// 2-根据优先级决定探索方向
					// 如果 x当前点 == x苹果
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// 找到苹果了，利用【上一个节点的方向】信息依次回溯（同时沿路标记apple）到最开始蛇头的位置
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// 寻路结束
						// 防止为了吃苹果走死路，这里加上一个判断，走完这个方向一步后如果还能找到蛇尾则允许走
						isSafe = 0;
						towards = mapAI[x_headAI][y_headAI].subDatas.next_direction;
						switch (towards)
						{
						case DIRECTION_UP:
							isSafe = canGetTail(x_head, y_head-1);
							break;
						case DIRECTION_DOWN:
							isSafe = canGetTail(x_head, y_head+1);
							break;
						case DIRECTION_LEFT:
							isSafe = canGetTail(x_head-1, y_head);
							break;
						case DIRECTION_RIGHT:
							isSafe = canGetTail(x_head+1, y_head);
							break;
						default:
							break;
						}

						if (isSafe)
						{
							snake_moveComm(towards);
						} else {
							wander(towards);
						}

						// // 走一步,退出大循环(TODO:也可以一次性走完全程)
						// snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// 当前点在苹果的右下方，搜索优先级:左上右下
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// 当前点在苹果的右上方，搜索优先级:左下右上
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// 当前点在苹果的左上方，搜索优先级:右下左上
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// 当前点在苹果的左下方，搜索优先级:右上左下
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// 当前点在苹果的正下方，搜索优先级:上左右下
						if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// 当前点在苹果的正上方，搜索优先级:下左右上
						if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正左方，搜索优先级:右下上左
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正右方，搜索优先级:左上下右
						if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					}
				}
			#endif
		}
	}
}

void snake_reCreateFood(){
	updateMap(x_apple, y_apple, STS_EMPTY);
	createfood();
}

uchar isFull(){
	uchar x,y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			if (map[x][y] == STS_EMPTY) return 0;
		}
	}
	return 1;
}

// 生成食物
void createfood()
{
	if(isFull()){
		gameover();
		return;
	}

	do
	{
		x_apple = rand() % SNAKE_SIZE_X;
		y_apple = rand() % SNAKE_SIZE_Y;
	} while(map[x_apple][y_apple] != STS_EMPTY);

	updateMap(x_apple, y_apple, STS_APPLE);
}

void snake_moveAppleUp(){
	if(y_apple-1 >= 0 && map[x_apple][y_apple-1] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple, y_apple-1, STS_APPLE);
		y_apple--;
	}
}

void snake_moveAppleDown(){
	if(y_apple+1 < SNAKE_SIZE_Y && map[x_apple][y_apple+1] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple, y_apple+1, STS_APPLE);
		y_apple++;
	}
}

void snake_moveAppleLeft(){
	if(x_apple-1 >= 0 && map[x_apple-1][y_apple] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple-1, y_apple, STS_APPLE);
		x_apple--;
	}
}

void snake_moveAppleRight(){
	if(x_apple+1 < SNAKE_SIZE_X && map[x_apple+1][y_apple] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple+1, y_apple, STS_APPLE);
		x_apple++;
	}
}

uchar snake_getPointData(uchar x, uchar y){
	return map[x][y];
}

#ifdef SNAKE_AI_DEBUG
union dfsMapType snake_getPointDataAI(uchar x, uchar y){
	return mapAI[x][y];
}
#endif

unsigned int snake_getNowScroe(){
	return score;
}
