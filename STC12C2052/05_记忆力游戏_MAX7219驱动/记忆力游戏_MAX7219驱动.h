#include <STC12x20xx.h>

#ifndef __TXS_SNAKE_H_
#define __TXS_SNAKE_H_

// ### ̰ʳ��ģ�� ����START ####################################################################################

#define uchar unsigned char
#define uint unsigned int

// ��ͼ��С
#define SIZE_X 	8
#define SIZE_Y	8

// ��Ϸ״̬
#define GAME_STS_PLAY 		0	// ��Ϸ��
#define GAME_STS_GAMEOVER	1	// ��Ϸ����
#define GAME_STS_SETTING	2	// ����
#define GAME_STS_PAUSE		3	// ��ͣ

// ��ͼ��ά����ȡֵ����
#define STS_EMPTY 0				// ��ʾ�õ�Ϊ��
#define STS_APPLE 1		// ��ʾ�õ�Ϊʳ��
#define STS_HEAD 10				// ��ͷ(ǰ��û��������)
#define STS_BODY_FROM_U 11		// ������������ǰһ���������Լ����Ϸ�
#define STS_BODY_FROM_D 12		// ������������ǰһ���������Լ����·�
#define STS_BODY_FROM_L 13		// ������������ǰһ���������Լ�����
#define STS_BODY_FROM_R 14		// ������������ǰһ���������Լ����ҷ�
#define STS_WALL 20		// ��ʾ�õ�Ϊ�ϰ��Ч��������һ����ֻ��Ϊ���Ժ���չ����Ԥ����

// ��ǰ���������Զ�ǰ��
#define NOW_TOWARDS_U 1		
#define NOW_TOWARDS_D 2		
#define NOW_TOWARDS_L 3		
#define NOW_TOWARDS_R 4		

// ��ʼ���峤��
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