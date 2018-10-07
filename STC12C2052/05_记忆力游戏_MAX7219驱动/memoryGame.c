#include <memoryGame.h>
#include <stdlib.h>

// ��ͼ��С
#define SIZE_X 	8
#define SIZE_Y	8

// ��Ϸ״̬
uchar gameStatus;
#define GAME_STS_PLAY 		0	// ��Ϸ��
#define GAME_STS_GAMEOVER	1	// ��Ϸ����
#define GAME_STS_SETTING	2	// ����
#define GAME_STS_PAUSE		3	// ��ͣ

uchar playStatus;
#define PLAY_STS_BEFORE_SHOWLEVEL 		10	// ��ʾÿ�ص�titleǰ
#define PLAY_STS_SHOWLEVEL 				11	// ��ʾÿ�ص�title��
#define PLAY_STS_QUEST 					13	// ��ʾ������
#define PLAY_STS_WAITINPUT				14	// �ȴ�����ش𡰣���
#define PLAY_STS_ANSWER_OK				15	// ��ʾ�ش���
#define PLAY_STS_ANSWER_NG				16	// ��ʾ�ش���


// �Ѷ�(ÿ�γ��ָ���) (0 - 8)
uchar level;
#define MAX_LEVEL 9

// �ٶ� (0 - 4)
uchar speed;
#define MAX_SPEED 5

// ��׼ʱ��
#define TIME_BASE 1000
#define TIME_SHOWTITLE (TIME_BASE/2)
#define TIME_SPEED1 (TIME_BASE/3)
#define TIME_SPEED2 (TIME_BASE/4)
#define TIME_SPEED3 (TIME_BASE/5)
#define TIME_SPEED4 (TIME_BASE/6)
#define TIME_SPEED5 (TIME_BASE/7)
#define TIME_GAMEOVER  (TIME_BASE)
#define TIME_STOP 0 // ����һ������ʱ�䣬���ظ�������0��ʾ����������ͣ��ʱ�����ȴ��û�����
uint code TIME_SPEEDS[] = {TIME_SPEED1, TIME_SPEED2, TIME_SPEED3, TIME_SPEED4, TIME_SPEED5};

// �������飬���ÿ�������������������
uchar puzzleArr[MAX_LEVEL];
#define PUZZLE_U	0
#define PUZZLE_D	1
#define PUZZLE_L	2
#define PUZZLE_R	3

// ��˳�����������ʱ�򣬵�ǰ�����ֵڼ���(0 - 8)
uchar nowShowingPuzzleIdx;

// ��˳���������ʱ�򣬵�ǰ�ǽ��ڼ���(0 - 8)
uchar nowAwnserIdx;

/* ��ͷ��ʽ1 ����ļ�ͷ
uchar code IMG_ARROWS[4][8] = {
	{0x10,0x28,0x44,0xEE,0x28,0x28,0x28,0x38}, // U
	{0x38,0x28,0x28,0x28,0xEE,0x44,0x28,0x10}, // D
	{0x10,0x30,0x5F,0x81,0x5F,0x30,0x10,0x00}, // L
	{0x08,0x0C,0xFA,0x81,0xFA,0x0C,0x08,0x00}  // R
	};
*/

/* ��ͷ��ʽ2 ϸ����ͷ
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
		// ����һ������
		mg_createPuzzle();

		// ��ʾTitle״̬
		playStatus = PLAY_STS_BEFORE_SHOWLEVEL;

		// ����ģʽ�°��Ҽ�������level���ã���ʼ��Ϸ
		gameStatus = GAME_STS_PLAY;
	} else {
		//mg_restart();
	}
}

// TODO
// ���������������һ��������(����״̬��)�󷵻�һ��ʱ�䣬
// ���ʱ�������������أ���ǰ��ʾ֡���������ڴ��ĳ���ʱ��
// ���ؽ��յ����ʱ���Ӧ����ʾ��ǰ֡������ָ��ʱ��,Ȼ����һ�ε��ñ�����������һ��,ѭ�������γ������Ķ���
uint mg_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		if (playStatus == PLAY_STS_BEFORE_SHOWLEVEL) {
			playStatus = PLAY_STS_SHOWLEVEL;
			return TIME_SHOWTITLE;
		} else if (playStatus == PLAY_STS_SHOWLEVEL) {
			// �ɵ�ǰ�ȼ�����ת�Ƶ����⻭��
			playStatus = PLAY_STS_QUEST;
			nowShowingPuzzleIdx = 0;
			return TIME_SPEEDS[speed];
		} else if (playStatus == PLAY_STS_QUEST) {
			if (nowShowingPuzzleIdx==level) // ˵���Ѿ���ʾ�����ϵ�е�������
			{
				playStatus = PLAY_STS_WAITINPUT;
				nowAwnserIdx = 0; // �ش�idx��0��ʼ
				// ֪ͨ������ֹͣ��ʱ�����ȴ��û�����
				return TIME_STOP;
			} else {
				// �����յ�ǰ���ٶȼ���������������
				nowShowingPuzzleIdx++;
				return TIME_SPEEDS[speed];
			}
		} 
		else if (playStatus == PLAY_STS_WAITINPUT || playStatus == PLAY_STS_ANSWER_OK) {
			if (nowAwnserIdx > level) // ˵���Ѿ���������������ˣ���ǰ�ٶ��£�
			{
				// ��������һ������
				mg_createPuzzle();

				nowAwnserIdx = 0;

				// ׼������ʾTitle״̬
				playStatus = PLAY_STS_BEFORE_SHOWLEVEL;

				if (speed == MAX_SPEED - 1) // ˵���Ѿ�����굱ǰ�ȼ�������ٶȵ�������
				{
					// ������һ�ȼ���������ٶȿ�ʼ
					level++;
					speed=0;
				} else {
					// ��ǰ�ȼ�������ٶ�
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
		// �����ǰ��GameOver�Ļ��棬��ת��restart
		mg_restart();
	} else {
		// û������Ҫ��Ļ���Ϊ�˱�֤������Ķ�ʱ������ִ�У�������㷵��һ���϶̵�ʱ�伴��
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
			puzzleArr[i] = rand() % 4; // 0,1,2,3�������
		} while(puzzleArr[i] == puzzleArr[i-1]); //�����������ظ�����������ʱ����û�б仯��©�ˣ�
	}
}

// �ú������ڸ��������ṩ��ʾ���ݣ��е�λ��
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