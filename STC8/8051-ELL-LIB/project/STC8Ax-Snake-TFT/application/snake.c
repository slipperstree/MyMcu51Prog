#include "snake.h"

// ��Ϸ״̬
uchar gameStatus;

// ��ͼ��ά����
uchar map[SNAKE_SIZE_X][SNAKE_SIZE_Y];

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	// Ѱ·��ά����
	union dfsMapType mapAI[SNAKE_SIZE_X][SNAKE_SIZE_Y];
	char x_headAI, y_headAI, stepsFromHeadAI;
	void wander(uchar); // ɢ��
#endif

#ifdef AI_LEVEL_MASTER
	// ����ƻ��������ĳ���ط�������ѭ������������˶��ٲ��Ժ�û�гԵ�ƻ������������һ��
	#define RECREATE_APPLE_STEPS (SNAKE_SIZE_X*SNAKE_SIZE_Y*2)
	unsigned int reCreateAppleStep;

	// ����������ɺö��ƻ������û�Ե�����ô��Ϊ�Ѿ��Բ����ˣ�������һ����ɱ
	#define RE_CREATE_APPLE_CNT 10
	uchar reCreateAppleCnt;
#endif

// ��ͷ��λ��
uchar x_head, y_head;

// ��β��λ��
uchar x_end, y_end;

// ʳ���λ��
uchar x_apple, y_apple;

// ��ǰ����
uchar nowTowards;

// ����
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
	// �������ϵ����ɶ�
	uchar freeUp,freeDown,freeLeft,freeRight = 0;

	// ȡ����ֵ
	uchar myAbs(uchar a, uchar b){
		if(a>=b)
			return a-b;
		else
			return b-a;
	}
	
	// ȡĳ��λ�õ����ɶȣ���Χ���ߵķ�������
	uchar ai_kids_getFree(uchar x, uchar y){
		uchar stepsFromHead = myAbs(x_head, x) + myAbs(y_head, y);
		return canMoveUp(x,y,stepsFromHead) + canMoveDown(x,y,stepsFromHead) + canMoveLeft(x,y,stepsFromHead) + canMoveRight(x,y,stepsFromHead);
	}
#endif

// �����ߴ�init�����������Ļص�����ָ��
pSnakeEventFunc callbackSnakeEventFunc;

// ������Ҫ����map���ݵ�ʱ�򶼵��������������֤�ܼ�ʱ֪ͨ���ƶ˸�����Ӧ�Ļ���
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

	// TODO �����¼�����û��ʹ��
	callbackSnakeEventFunc(SNAKE_EVENT_RESTART, 0, 0);

	// ��ʼ����
	score = 0;

	gameStatus = GAME_STS_PLAY;

	// ��ʼblock β��-����-ͷ ->
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

// ��ǰ״̬��N���Ժ�ָ�������Ƿ�Ϊ��β
// ����Ϊ���жϵ���ͷ�߹�N�����Ժ�����������ʱ��ĳ�������ϱ���������
//   ����N���Ժ��п��ܱ����β����ô�����������ɶ�Ҳ��Ч
//   �����������һ����ô���ɶȾ���2�����ϣ�����ʵӦ����3�����ϣ��ң�
//   ����ͷ��һ����A��ʱ����βҲ�ƶ���һ��������A����Ҳ�������β��A�����ͷ������ת���Ҳ�ģ�����ײ����β��
//   ������ͼ��-Ϊ��β��@Ϊ��ͷ��+Ϊ��������Ҫ�ж�A������ɶȣ�
//    -
//   A+	
//   @+	
//   ++    
//   ��ͷ�����ߵ�A�������������ӣ��Ҳ�����β�����ߵ�ͨ
//     
//   @-
//   ++
//   ++
//   �������ߣ�û������
//     
//   +@
//   +-
//   ++
uchar isTail(uchar x, uchar y, uchar afterSteps){
	uchar step;
	uchar tmp_tail_x = x_end;
	uchar tmp_tail_y = y_end;
	uchar tailNextTo;

    //��ֱ���жϵ�ǰ������ǲ��Ǿ�����β������ǾͲ���Ҫ��N�����ж���
	if (tmp_tail_x == x && tmp_tail_y == y)
	{
		return 1;
	}

	// ����β�������������Ĳ���
	for (step = 0; step < afterSteps; step++)
	{
		if(step == 0){
			// ��һ������β������Ӹ�λ��ȡ
			tailNextTo = map[tmp_tail_x][tmp_tail_y] % 10;
		} else {
			// ���������ʮλ��ȡ����λ���Ǻ������ӵķ���
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

		// ֻҪ��;����β���������ͱ�ʾ������ǰ�ȫ��
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

// x,y : ��������λ��
// stepsFromHead : ����ͷ�ߵ�x,y�Ѿ��߹��˶��ٲ���Ϊ��Ԥ����β��λ�ã�
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

		// �ص��ϸ��ڵ�
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
			// ��ǰ����ͷ�����ʾ���ݽ���
			return 0;
		} else {
			// �ص��ϸ��ڵ�
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
	// ��ͼ�ƶ�����ķ������ƶ����x���ƶ����y
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
		// ������Ч
		if (nowTowards == directionBackward){
			return 0;
		}

		if (canMoveComm(direction, x_head, y_head, 0)) 
		{
			// ǰ���ǿյػ�����β�Ͳ��ƶ�β�ͣ������ʳ�����ƶ�β�ͣ�Ҳ����������䳤
			if (map[x_next][y_next] != STS_APPLE) {
				moveEndBlock();

				#ifdef AI_LEVEL_MASTER
					// ÿ��һ���Ͳ�����һ
					reCreateAppleStep++;
					if (reCreateAppleStep > RECREATE_APPLE_STEPS)
					{
						reCreateAppleStep = 0;

						// ���Ѿ����˺ܶಽ�ˣ�����Ȼ���ܳԵ�ƻ������������һ���µ�ƻ��
						snake_reCreateFood();
						// ÿ��������һ��ƻ���ͼ���һ��
						reCreateAppleCnt++;
						if (reCreateAppleCnt > RE_CREATE_APPLE_CNT)
						{
							// ��������ƻ����Σ�AI��Ȼ�Բ���ƻ������Ϊ�޷�������Ϸ��ǿ��gameover
							reCreateAppleCnt = 0;
							gameover();
						}
					}
					
				#endif
			} else {

				#ifdef AI_LEVEL_MASTER
					// ÿ�Ե�һ��ƻ���ͽ��ظ�����ƻ���Ĵ������ߵĲ����������¼���
					reCreateAppleCnt=0;
					reCreateAppleStep=0;
				#endif

				score++;
				createfood();
				callbackSnakeEventFunc(SNAKE_EVENT_EAT_APPLE, score, 0);
			}

			// ԭ����ͷ������壬��ǰһ��ָ���µ�ͷ����ʮλ�����ƣ�Ҳ���Ƿ������10��
			updateMap(x_head, y_head, (direction * 10) + map[x_head][y_head] % 10);

			// �����µ�ͷ��������ͷλ��
			updateMap(x_next, y_next, STS_HEAD + directionBackward);
			x_head = x_next;
			y_head = y_next;

			// �޸ĵ�ǰ����
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

	// ȡ��λ�����õ���βǰһ��block�ķ���
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

	// ԭ������β���
	updateMap(xTmp, yTmp, STS_EMPTY);

	// ԭ������������β(���ڵ�block������Ϣ�ĳ������ʮλ��(ǰһ��block����))
	updateMap(x_end, y_end, STS_TAIL + map[x_end][y_end] / 10);

}

void snake_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		snake_moveComm(nowTowards);
	}
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
// ������ƻ����ʱ�������һ��
void wander(uchar exceptRirection){
	uchar canMoveUpValue, canMoveLeftValue, canMoveRightValue, canMoveDownValue;
	uchar directionRnd = rand() % 4 + 1;

	canMoveUpValue = canMoveUp(x_head, y_head, 0);
	canMoveDownValue = canMoveDown(x_head, y_head, 0);
	canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
	canMoveRightValue = canMoveRight(x_head, y_head, 0);

	// �����·������gameover
	if (!(canMoveUpValue + canMoveDownValue + canMoveLeftValue + canMoveRightValue))
	{
		gameover();
	}
	// ���ֻʣһ������������ߣ���Ȼ����ѭ��
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
	// ���������Ϸ�����ߣ������һ��������������Ż���̫���������Լ�����Ҫ�в��Ե������
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
		// ��� x��ǰ�� == x��β
		if (x_headAI == x_end && y_headAI == y_end)
		{
			// �����ҵ���β������Ѱ��
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
					// ��ǰ����ʼ�㣬���ʾ̽·����һȦû�ҵ��˻�ԭ���ˣ�û��ͨ����β��·
					return 0;
				}else{
					// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
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
		// ��� x��ǰ�� == xƻ��
		if (x_headAI == x_apple && y_headAI == y_apple)
		{
			// �����ҵ�ƻ��������Ѱ��
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
					// ��ǰ����ʼ�㣬���ʾ̽·����һȦû�ҵ��˻�ԭ���ˣ�û��ͨ����β��·
					return 0;
				}else{
					// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
					markDeadwayAndGoBackToPrevAI();
				}
			}
		}
	}
}
#endif

// �������Χ�Ŀյط�����һ��
void snake_AI_moveNext(){
	uchar towards;
	uchar isSafe;
	// ������ɷ��ƶ�(0:���ɣ�1:��)
	uchar canMoveUpValue, canMoveDownValue, canMoveLeftValue, canMoveRightValue;

	#ifdef AI_LEVEL_MASTER
	// �������ƶ�һ�����ܷ��ҵ���ͷ(0:���ɣ�1:��)
	uchar canGetTailUpValue, canGetTailDownValue, canGetTailLeftValue, canGetTailRightValue;
	// �������ƶ�һ�����ܷ��ҵ�ƻ��(0:���ɣ�1:��)
	uchar canGetAppleUpValue, canGetAppleDownValue, canGetAppleLeftValue, canGetAppleRightValue;
	#endif

	if (gameStatus == GAME_STS_PLAY)
	{
		//�ȼ��Ŀǰ3�������л���û�пյأ������û�о�gameover��,���������ѭ��
		canMoveUpValue = canMoveUp(x_head, y_head, 0);
		canMoveDownValue = canMoveDown(x_head, y_head, 0);
		canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
		canMoveRightValue = canMoveRight(x_head, y_head, 0);

		if( !canMoveUpValue && !canMoveDownValue && !canMoveLeftValue && !canMoveRightValue ) {
			gameover();
		}
		//��Χ�пյػ�ƻ����β��
		else
		{
			#if defined(AI_LEVEL_BABY) || defined(AI_LEVEL_KIDS) || defined(AI_LEVEL_SMART)
				//����Ա���ƻ�����ȳ�ƻ��(Ҳ������˹ҵ�)
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

			// ����(ֻ���������)
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

			// ��ͯ(���Ԥ��2��)
			#ifdef AI_LEVEL_KIDS
				// �ֱ�ȡ��һ������λ�õ����ɶȣ��߹�ȥ�Ժ�����һ���м���������ߣ�
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
				
				// �����ɶ����ķ�������
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

			// ����(��Ѱ·ƻ��)
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
					// 2-�������ȼ�����̽������
					// ��� x��ǰ�� == xƻ��
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// �ҵ�ƻ���ˣ����á���һ���ڵ�ķ�����Ϣ���λ��ݣ�ͬʱ��·���apple�����ʼ��ͷ��λ��
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// Ѱ·����,��һ��,�˳���ѭ��(TODO:Ҳ����һ��������ȫ��)
						snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������󷽣��������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������ҷ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					}
				}
			#endif

			// ��ʦ(�����ȱ�������ͬ���Ѱ·ƻ��)
			#ifdef AI_LEVEL_MASTER

				#ifdef SNAKE_AI_DEBUG
				if (score >= 36)
				{
					score = score;
				}
				#endif

				// 4�������ƶ�һ�����ܷ��ҵ���β��ƻ��
				// TODO : û��Ҫȫ����ĳ���������ҵ���β��ƻ������Ҫ�����������ˣ���ʡ������
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

				// // ������ ���� �ҵ���β Ҳ�� �ҵ�ƻ�� �ķ���
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

				// // ����� ֻ�� �ҵ���β �ķ���TODO Ӧ���Ż�������ͨ����β���������·����ôһ��·�������ܿճ��ռ䣩
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

				// // �ٴ��� ֻ�� �ҵ�ƻ���ķ���
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

				// // �����������β��ƻ������������ֻ�����ɢ����(TODO ɢ���Ĳ���Ҳ�н���)
				// else {
				// 	wander(0);
				// }

				// ��һ������Ч��AI�߼�������������ʧ�ܿ��Իָ�������һ��
				x_headAI = x_head;
				y_headAI = y_head;
				clearMapAI();
				while(1){
					// 2-�������ȼ�����̽������
					// ��� x��ǰ�� == xƻ��
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// �ҵ�ƻ���ˣ����á���һ���ڵ�ķ�����Ϣ���λ��ݣ�ͬʱ��·���apple�����ʼ��ͷ��λ��
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// Ѱ·����
						// ��ֹΪ�˳�ƻ������·���������һ���жϣ������������һ������������ҵ���β��������
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

						// // ��һ��,�˳���ѭ��(TODO:Ҳ����һ��������ȫ��)
						// snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������󷽣��������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������ҷ����������ȼ�:��������
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
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
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

// ����ʳ��
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
