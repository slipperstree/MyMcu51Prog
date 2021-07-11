/******************************************
 * File  : control.c
 * Author: chenling
 * Date  : 2019.11
 * Func  : ForSTM8 库文件版
*******************************************/

#include "common.h"
#include "snake.h"
#include "control.h"
#include "display.h"

#define SCN_OFFSET_X 0
#define SCN_OFFSET_Y 0

// TODO 目前只支持8位
#define BLOCK_SIZE 8

#define MODE_WELCOME 0
#define MODE_WELCOME_DEMO 1
#define MODE_GAME 2
#define MODE_GAMEOVER 3

uint maxDemoScore;
uint totalDemoScore;
uint totalDemoCnt;
uint avgDemoScore;
uint lastDemoScore;
uchar nowMode;

uint ttFrame;
uint ttFlag;
uchar welcomeWaitTime;

// 速度等级，分数越高速度越快
uint nowSpeed;
#define SPEED_DEFAULT   5000     //初始速度
#define SPEED_MAX       2000     //最高速度
#define SPEED_INTERVAL  350      //每个等级速度相差的数值
#define SPEED_DEMO      100       //DEMO速度

uchar* STR_PRESS_ANY_KEY = (u8*)"PressAnyKey...";
uchar* STR_SPACE_ROW = (u8*)"                ";

uchar tmpX, tmpY;
void drawWelcome();
void eventSnake(uint event_id, uint p1, uint p2);

#ifdef SNAKE_AI_DEBUG
uchar isPauseAfterEatAppleMode, isPauseEveryStep, isPause;
void updateBlockAI(uchar x, uchar y);
void updateAllAI();
#endif

void CTL_init() {
    // 显示标题画面
    drawWelcome();

    ttFrame = 0;
    welcomeWaitTime = 0;
    nowSpeed = SPEED_DEFAULT;
    nowMode = MODE_WELCOME;
    snake_init(eventSnake);
}

uchar flashFlag = 0;
uchar flashOnOff = 0;
// 这个函数需要在main函数中循环调用
void CTL_run(){
	uint flashSpeed=0;
	
    ttFrame++;
    ttFlag++;
    
    if (nowMode == MODE_WELCOME_DEMO)
    {
        flashSpeed = 500;
    } else {
        flashSpeed = 5000;
    }
    

    if (ttFlag > flashSpeed)
    {
        ttFlag = 0;
        flashFlag = 1;
        if (flashOnOff == 1)
        {
            flashOnOff = 0;
        } else {
            flashOnOff = 1;
        }
    }else{
        flashFlag = 0;
    }

    switch (nowMode)
    {
        case MODE_WELCOME:
            // 欢迎画面超过一段时间，进入自动演示Demo模式
            if (welcomeWaitTime >= 3)
            {
                welcomeWaitTime = 0;
                snake_restart();
                nowSpeed = SPEED_DEMO;
                nowMode = MODE_WELCOME_DEMO;
            } else {
                // 画面动态效果
                if (flashFlag == 1) {
                    if (flashOnOff == 1)
                    {
                        DISP_ShowString(8,6,STR_PRESS_ANY_KEY,16);
                        welcomeWaitTime++;
                    } else {
                        DISP_ShowString(0,6,STR_SPACE_ROW,16);
                    }   
                }
            }
            break;

        case MODE_WELCOME_DEMO:
            // 随机前进
            if (ttFrame >= nowSpeed)
            {
                ttFrame = 0;
                snake_AI_moveNext();

                #ifdef SNAKE_AI_DEBUG
                updateAllAI();
                #endif
            }

            if (flashFlag == 1) {
                DISP_ShowNum(10,0,maxDemoScore,3,8);
                DISP_ShowNum(0,0,snake_getNowScroe(),3,8);
            }
            
            break;

        case MODE_GAME:
            // 自动前进
            if (ttFrame >= nowSpeed)
            {
                ttFrame = 0;
                snake_moveNext();
            }
            break;
        case MODE_GAMEOVER:
            // Gameover画面动态效果
            if (flashFlag == 1) {
                if (flashOnOff == 1)
                {
                    DISP_ShowString(8,5,STR_PRESS_ANY_KEY,16);
                } else {
                    DISP_ShowString(0,5,STR_SPACE_ROW,16);
                }   
            }
            break;
        default:
            break;
    }
}

void doBtnCommon(uchar btnNo){
    switch (nowMode)
    {
    case MODE_WELCOME:
        // 从标题画面进入游戏画面
        snake_restart();
        nowMode = MODE_GAME;
        break;
    case MODE_WELCOME_DEMO:
        // // 从demo画面回到标题画面
        // nowMode = MODE_WELCOME;
        // drawWelcome();

        // Demo状态允许移动苹果喂蛇
        switch (btnNo)
        {
        case 1:
            //snake_moveAppleUp();
            snake_reCreateFood();
            break;
        case 2:
            //snake_moveAppleDown();
            nowMode = MODE_WELCOME;
            drawWelcome();
            break;
        case 3:
            snake_moveAppleLeft();
            break;
        case 4:
            snake_moveAppleRight();
            break;
        default:
            break;
        }
        break;
    case MODE_GAME:
        if( (btnNo == 1 && snake_moveUp())      ||
            (btnNo == 2 && snake_moveDown())    ||
            (btnNo == 3 && snake_moveLeft())    ||
            (btnNo == 4 && snake_moveRight())
            ){
            // 手动移动成功，自动前进计时清零
            ttFrame = 0;
        }
        break;
    case MODE_GAMEOVER:
        // 从GAMEOVER画面回到标题画面
        CTL_init();
        nowMode = MODE_WELCOME;
        break;
    default:
        break;
    }
}

void CTL_doBtnUClick(){
    doBtnCommon(1);
}

void CTL_doBtnDClick(){
    doBtnCommon(2);
}

void CTL_doBtnLClick(){
    doBtnCommon(3);
}

void CTL_doBtnRClick(){
    doBtnCommon(4);
}

void CTL_doBtnUKeepPress(){
switch (nowMode)
    {
    case MODE_WELCOME:
        break;
    case MODE_WELCOME_DEMO:
        snake_reCreateFood();
        break;
    case MODE_GAME:
        break;
    case MODE_GAMEOVER:
        break;
    default:
        break;
    }
}

void CTL_drawFrame() {

	// uchar row;

    // OLED_Set_Pos(SCN_OFFSET_X, SCN_OFFSET_Y);
    // OLED_WR_Byte(0xff,OLED_DATA);

	// for (row= 1; row < SNAKE_SIZE_Y; ++row)
	// {
    //     OLED_Set_Pos(row, 0);
    //     OLED_WR_Byte(0x01,OLED_DATA);
	// }

    // OLED_Set_Pos(SNAKE_SIZE_Y, 0);
    // OLED_WR_Byte(0xff,OLED_DATA);
}

void drawWelcome() {

    CTL_clean();

    DISP_ShowString(4*8,1,"Snake AI",16);
    //OLED_ShowCHinese(pos    , 1, 0, 1);
	//OLED_ShowCHinese(pos + w, 1, 1, 1);
	//OLED_ShowCHinese(pos + w + w, 1, 2, 1);
	//OLED_ShowCHinese(32,2,4,4);
	//DISP_ShowString(0,4,"    For STM8    ",16);
	DISP_ShowString(4*8,4,"2019 C.L",16);
}

void drawGameOver(uchar score) {
    CTL_clean();

    // 如果想显示中文界面(占用rom较多))，需要放开下面并注释掉原来的部分，并放开oled_library.h里Hzk中的字库数据
    // uchar col = (128-8*12)/2;
    // OLED_ShowCHinese(col, 0, 11, 4); //游戏结束
    // OLED_ShowCHinese(col, 3, 9, 2); DISP_ShowNum(col+8*5,3,score,2,16); //得分
    // OLED_ShowCHinese(col, 6, 3, 6); //按任意键开始

    // 英文界面
    //DISP_ShowString(0,0,"________________",8);
    DISP_ShowString(3*8,1,"GAME  OVER",16);
    DISP_ShowString(4*8,3,"Score:",16); DISP_ShowNum(8*10,3,score,2,16); //得分
    DISP_ShowString(1*8,5,"PressAnyKey... ",16);
    //DISP_ShowString(0,7,"----------------",8);
}

uchar getBlockStartColumnByX(uchar x){
    return x * BLOCK_SIZE + SCN_OFFSET_X;
}

uchar getBlockEndColumnByX(uchar x){
    return (x + 1) * BLOCK_SIZE + SCN_OFFSET_X - 1;
}

uchar getBlockStartPageByY(uchar y){
    // TODO 先用8位试一试
    return y;
}

void eventSnake(unsigned int event_id, unsigned int p1, unsigned int p2){
    switch (event_id)
    {
    case SNAKE_EVENT_UPDATE_BLOCK:
        DISP_updateGameBlock(p1, p2);
        break;
    #ifdef SNAKE_AI_DEBUG
    case SNAKE_EVENT_UPDATE_AI_PATH:
        if (snake_getNowScroe() >= 5)
        {
            updateBlockAI(p1, p2);
        }
        break;
    #endif
    case SNAKE_EVENT_RESTART:
        CTL_clean();
        nowSpeed = SPEED_DEFAULT;
        break;
    case SNAKE_EVENT_GAMEOVER:

        if (nowMode == MODE_GAME)
        {
            //先停一会死掉的状态
            My_delay_ms(20000);
            My_delay_ms(20000);

            drawGameOver(p1);
            nowMode = MODE_GAMEOVER;
        } else if (nowMode == MODE_WELCOME_DEMO) {
            lastDemoScore = p1;

            // 平均得分
            totalDemoCnt++;
            totalDemoScore += p1;
            avgDemoScore = totalDemoScore / totalDemoCnt;
            
            // 最高得分
            if (p1 > maxDemoScore)
            {
                maxDemoScore = p1;
            }

            for (p1 = 0; p1 < 8; p1++)
            {
                if (p1 % 2 == 1)
                {
                    DISP_ShowString(0,6,"                ",16);

                    DISP_ShowString(0,6,"MX:   AV:   ,   ",16);
                    DISP_ShowNum(3*8,6,maxDemoScore,3,16);
                    DISP_ShowNum(9*8,6,avgDemoScore,3,16);
                    DISP_ShowNum(13*8,6,lastDemoScore,3,16);
                } else {
                    for ( tmpX = 0; tmpX < 16; tmpX++)
                    {
                        for ( tmpY = 6; tmpY < 8; tmpY++)
                        {
                            DISP_updateGameBlock(tmpX, tmpY);
                        }
                        
                    }
                }
                My_delay_ms(8000);
            }

            //先停一会死掉的状态
            My_delay_ms(20000);
            My_delay_ms(20000);
            
            drawWelcome();
            nowMode = MODE_WELCOME;
        }
        
        break;
    case SNAKE_EVENT_EAT_APPLE:
        if (nowMode == MODE_WELCOME_DEMO)
        {
            // 运行DEMO时速度不变
        } else {
            // 手动游戏才改变速度
            nowSpeed -= SPEED_INTERVAL;
            if (nowSpeed <= SPEED_MAX) 
            {
                // 设置速度上限
                nowSpeed = SPEED_MAX;
            }
        }
        
        break;
    default:
        break;
    }
}

#ifdef SNAKE_AI_DEBUG
void updateBlockAI(uchar x, uchar y){
    switch (snake_getPointDataAI(x, y).subDatas.status)
    {
    case AI_STATUS_ON_THE_WAY:
        //OLED_ShowChar(x*8,y,'O',8);
        break;
    case AI_STATUS_WAY_TO_DEATH:
        //OLED_ShowChar(x*8,y,'X',8);
        break;
    case AI_STATUS_WAY_TO_APPLE:
        OLED_ShowChar(x*8,y,'.',8);
        break;
    default:
        break;
    }
}

void updateAllAI(){
    uchar x,y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
            if (snake_getPointData(x,y) == STS_EMPTY)
            {
                OLED_ShowChar(x*8,y,' ',8);
            }
		}
	}
}
#endif

void CTL_clean() {
    DISP_Clear();
}
