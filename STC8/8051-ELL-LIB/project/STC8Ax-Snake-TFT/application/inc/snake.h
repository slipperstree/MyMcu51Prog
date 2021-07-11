#ifndef __TXS_SNAKE_H_
#define __TXS_SNAKE_H_

#include "common.h"
#include "stdlib.h"

// ### ̰ʳ��ģ�� ����START ####################################################################################

#define uchar unsigned char
#define uint unsigned int

// ��ͼ��С
// 10 pixel per block
// #define SNAKE_SIZE_X 	16
// #define SNAKE_SIZE_Y	13
// 5 pixel per block
#define SNAKE_SIZE_X 	32
#define SNAKE_SIZE_Y	25

// snake_AI_moveNext ��AI�㷨д�˺ü���ʵ�֣�Խ��rom�ռ�ԽС��������Ҫ�ſ�����4�������е�ĳһ��������ͬʱ�ſ���
//#define AI_LEVEL_BABY     // ����(ֻ��������ߣ�������óɼ�11)
//#define AI_LEVEL_KIDS     // ��ͯ(���Ԥ��2����������óɼ�32)
//#define AI_LEVEL_SMART    // ����(��Ѱ·�ҵ�ƻ����������óɼ�65��ƽ��34) // Realse+�Ż��������rom size<8k
#define AI_LEVEL_MASTER     // ���(��Ѱ·�ҵ�ƻ��������ͬʱ�����������ͬ��������óɼ�??��ƽ��??)

// ����ģʽ
//#define SNAKE_AI_DEBUG

// ��Ϸ״̬
#define GAME_STS_PLAY 		0	// ��Ϸ��
#define GAME_STS_GAMEOVER	1	// ��Ϸ����
#define GAME_STS_SETTING	2	// ����
#define GAME_STS_PAUSE		3	// ��ͣ

// ��ͼ��ά����ȡֵ����
#define STS_EMPTY 0				// ��ʾ�õ�Ϊ��
#define STS_APPLE 1		        // ��ʾ�õ�Ϊʳ��

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

// ������Ķ���
// ��ͷ����ͷ50+������ͷ��������(1-4)
// ��������10/20/30/40��+����һ������ķ���(1-4)
// ��β����β60+������β��������(1-4)
//     ʮλ�� : ��ͷ����������4��������Ϣ������β
//     ��λ�� : ����block�ķ���
//     ���ϸ����Ϊ�˷�������Ļ�ͼģ����Ƹ��ÿ�ת���������������ߣ�
//     ����ʾЧ��Ҫ�󲻸�Ҳ���Լ�ȡʮλ��Ϊ1-6����

// ���ʮλ����1-4�������ķ�������1-4Ҫ����һ�£������ĸĳ�6789�Ļ�����ҲҪ�ģ����Ҳ��ܸ���ͷ��β��ͻ
#define STS_BODY_PREV_U 10
#define STS_BODY_PREV_D 20
#define STS_BODY_PREV_L 30
#define STS_BODY_PREV_R 40
// ��ͷ
#define STS_HEAD 50
// ��β
#define STS_TAIL 60

#define STS_NEXT_TO_U DIRECTION_UP
#define STS_NEXT_TO_D DIRECTION_DOWN
#define STS_NEXT_TO_L DIRECTION_LEFT
#define STS_NEXT_TO_R DIRECTION_RIGHT

// ��ʾ�õ�Ϊ�ϰ��Ч��������һ����ֻ��Ϊ���Ժ���չ����Ԥ����
#define STS_WALL 90

// ��ǰ���������Զ�ǰ��
#define NOW_TOWARDS_U DIRECTION_UP	
#define NOW_TOWARDS_D DIRECTION_DOWN		
#define NOW_TOWARDS_L DIRECTION_LEFT		
#define NOW_TOWARDS_R DIRECTION_RIGHT

// ��ʼ���峤��
#define START_BODY_LENGH 3

// �¼�
#define SNAKE_EVENT_RESTART  0
#define SNAKE_EVENT_GAMEOVER 1      // ����1������
#define SNAKE_EVENT_UPDATE_BLOCK 2  // ����1����Ҫ���µ�x���꣬����2����Ҫ���µ�y����
#define SNAKE_EVENT_EAT_APPLE  3    // �Ե�ƻ��������1������
#define SNAKE_EVENT_UPDATE_AI_PATH  4    // ����1����Ҫ���µ�x���꣬����2����Ҫ���µ�y����
typedef void (*pSnakeEventFunc)(unsigned int, unsigned int, unsigned int);
void snake_init(pSnakeEventFunc);
void snake_restart(void);

uchar snake_moveUp(void);
uchar snake_moveDown(void);
uchar snake_moveLeft(void);
uchar snake_moveRight(void);

// ��ǰ����ǰ��һ��
void snake_moveNext(void);

// AI�Զ���ĳ��������һ��
void snake_AI_moveNext(void);

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
    #define AI_STATUS_EMPTY 0
    #define AI_STATUS_ON_THE_WAY 1
    #define AI_STATUS_WAY_TO_DEATH 2
    #define AI_STATUS_WAY_TO_APPLE 3
    // DFS�������̽����ͼʹ�õġ�Ӱ�ӵ�ͼ���������Ͷ���
    // ÿһ������8λ
    // bit[2:0]������һ��·���ķ���1-4����ʵ4������2λ�͹��ˣ�������������(STS_BODY_PREV_X)ҲҪ�ľ���1-4��
    // bit[5:3]������һ��·���ķ���1-4��
    // bit[6:7]
    //    0:δ̽��  1:��·�ϣ�����̽����  2:��̽��������·  3:��̽����ͨ��ƻ��
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
